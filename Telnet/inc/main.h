//=================================================================================//
//	Arquivo : main.h
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

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include <stdio.h>
#include <string.h>

//==================================================================================//
//	Includes UNIX Socket
//==================================================================================//

#include <sys/socket.h>
#include <arpa/inet.h>

//==================================================================================//
//	Includes GTK3
//==================================================================================//

#include <gtk/gtk.h>
#include <glib.h>

//==================================================================================//
//	Defines
//==================================================================================//

#define UI_FILE "/home/maikeu/workspace-uc/Telnet/src/ui.glade"
#define TOP_WINDOW "window"

//==================================================================================//
// Tipos Locais
//==================================================================================//

typedef enum {
	RESET = 0, SET = 1
} flag_t;

typedef struct {
	flag_t connected;
	flag_t updateEnd;
	int socket;
	int dataLen;
	char message[2000];
} telnet_t;

//==================================================================================//
//	Protótipos
//==================================================================================//

static void * connection_handler(gpointer data);
static gboolean update_ui(gpointer data);




#endif /* INC_MAIN_H_ */
