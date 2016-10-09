/*
 * help_functions.c
 *
 *  Created on: 26-Sep-2016
 *      Author: zaki
 */

#include"help_functions.h"

void showHelp() {
	 showHelpHelp();
	 showCreatorHelp();
	 showDisplayHelp();
	 showRegisterHelp();
	 showConnectHelp();
	 showListHelp();
	 showTerminateHelp();
	 showQuitHelp();
	 showGetHelp();
	 showPutHelp();

}

void showHelpHelp() {
	printf("HELP\nusage: HELP\ndescription: Shows information about the available user commands\n\n");
}

void showCreatorHelp() {
	printf("CREATOR\nusage: CREATOR\ndescription: Displays creators information\n\n");
 }

void showDisplayHelp() {
	printf("DISPLAY\nusage: DISPLAY\ndescription: Displays IP address and Port on which this process is listening for incoming connections.\n\n");
 }

void showRegisterHelp() {
	printf("REGISTER\nusage: REGISTER <server IP> <port no>\ndescription: Registers this process with server using its IP address and listening port number\n\n");
 }

void showConnectHelp() {
	printf("CONNECT\nusage: CONNECT <destination> <port no>\ndescription: Connects this process with the destination peer listening on specified port number\n\n");
 }

void showListHelp() {
	printf("LIST\nusage: LIST\ndescription: Displays a list of all the connections this process is a part of.\n\n");
 }

void showTerminateHelp() {
	printf("TERMINATE\nusage: TERMINATE <connection id>\ndescription: Terminates the connection with the specified <connection id> listed in LIST command\n\n");
 }

void showQuitHelp() {
	printf("QUIT\nusage: QUIT\ndescription: Closes all connections and terminates the process\n\n");
 }

void showGetHelp() {
	printf("GET\nusage: GET <connection id> <filename>\ndescription: Downloads the file <filename> from the host specified by <connection id>\n\n");
 }

void showPutHelp() {
	printf("PUT\nusage: PUT <connection id> <filename>\ndescription: Uploads the file <filename> to the host specified by <connection id>\n\n");
 }


