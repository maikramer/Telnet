//=================================================================================//
//	Arquivo : main.c
//	Projeto : Telnet
//	Autor : Maikeu Locatelli
//	Copyright : Locatelli Engenharia
//
//	Descricão: Programa para comunicação via Telnet com o módulo ESP8266
//=================================================================================//
//	This file is part of Telnet
//	IntTeste is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//==================================================================================//
//	Includes
//==================================================================================//

#include <main.h>

//==================================================================================//
//	Variáveis Globais GTK
//==================================================================================//

GtkWidget* glbl_connect;
GtkWidget* gbtn_connect;
GtkTextView* gtbox;
GtkTextBuffer* textBuffer;
GtkTextMark* textMark;

//==================================================================================//
//	Variáveis Globais Telnet
//==================================================================================//

telnet_t telnet = { RESET, RESET, 0, 0, { 0 } };

//==================================================================================//
//	Handlers GTK
//==================================================================================//

// Ao fechar a janela
void on_window_destroy(GtkWidget *widget, gpointer data) {
	gtk_main_quit();
}

// Quando o botão conectar é pressionado
void on_btn_connect_clicked(GtkWidget *widget, gpointer data) {
	struct sockaddr_in server;
	if (telnet.connected == RESET) {
		//Create socket
		telnet.socket = socket(AF_INET, SOCK_STREAM, 0);
		if (telnet.socket == -1) {
			printf("Could not create socket");
		}

		server.sin_addr.s_addr = inet_addr("192.168.0.13");
		server.sin_family = AF_INET;
		server.sin_port = htons(2222);

		//Connect to remote server
		if (connect(telnet.socket, (struct sockaddr *) &server, sizeof(server))
				< 0) {
			perror("connect failed. Error");
			return;
		}

		puts("Conectado\n");
		telnet.connected = SET;
		g_thread_new("Telnet", (GThreadFunc) connection_handler, NULL);
		/*
		 if (pthread_create(&telnet, NULL, connection_handler, NULL) < 0) {
		 perror("could not create thread");
		 return;
		 }*/

		gtk_label_set_text(GTK_LABEL(glbl_connect), "Conectado");
		gtk_button_set_label(GTK_BUTTON(gbtn_connect), "Desconectar");
	} else {
		gtk_label_set_text(GTK_LABEL(glbl_connect), "Desonectado");
		gtk_button_set_label(GTK_BUTTON(gbtn_connect), "Conectar");
		telnet.connected = RESET;
		puts("desconectado\n");
	}
}

static GtkWidget* create_window(void) {
	GtkWidget *window;
	GtkBuilder *builder;
	GError* error = NULL;

	/* Load UI from file */
	builder = gtk_builder_new();
	if (!gtk_builder_add_from_file(builder, UI_FILE, &error)) {
		g_critical("Couldn't load builder file: %s", error->message);
		g_error_free(error);
	}

	/* Auto-connect signal handlers */
	gtk_builder_connect_signals(builder, NULL);

	/* Get the window object from the ui file */
	window = GTK_WIDGET(gtk_builder_get_object (builder, TOP_WINDOW));
	if (!window) {
		g_critical("Widget \"%s\" is missing in file %s.",
		TOP_WINDOW,
		UI_FILE);
	}

	// Referencia os widgets na janela
	glbl_connect = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_connect"));
	gbtn_connect = GTK_WIDGET(gtk_builder_get_object(builder, "btn_connect"));
	gtbox = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "tbox"));

	textBuffer = gtk_text_buffer_new(NULL);
	textMark = gtk_text_mark_new(NULL, TRUE);
	gtk_text_view_set_buffer(gtbox, textBuffer);

	g_object_unref(builder);

	return window;
}

int main(int argc, char *argv[]) {
	GtkWidget *window;
	gtk_init(&argc, &argv);

	window = create_window();

	gtk_widget_show(window);

	gtk_main();

	return 0;
}

static void * connection_handler(gpointer data) {

	//Receive a message from client
	while ((telnet.dataLen = recv(telnet.socket, telnet.message, 2000, 0)) > 0) {
		//Echo da mensagem recebida
		telnet.updateEnd = RESET;
		g_idle_add(update_ui, NULL);
		while (telnet.updateEnd == RESET)
			;
		if (telnet.connected == RESET) {
			shutdown(telnet.socket, SHUT_RDWR);
			close(telnet.socket);
			return 0;
		}
	}

	if (telnet.dataLen == 0) {
		puts("Client disconnected");
		fflush(stdout);
	} else if (telnet.dataLen == -1) {
		perror("recv failed");
	}

	return 0;
}

static gboolean update_ui(gpointer data) {
	gtk_text_buffer_insert_at_cursor(textBuffer, telnet.message,
			telnet.dataLen);
	textMark = gtk_text_buffer_get_insert(textBuffer);
	gtk_text_view_scroll_mark_onscreen(gtbox, textMark);
	telnet.updateEnd = SET;
	return FALSE;
}
