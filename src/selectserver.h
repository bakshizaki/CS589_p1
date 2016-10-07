/*
 * selectserver.h
 *
 *  Created on: 25-Sep-2016
 *      Author: zaki
 */

#ifndef SELECTSERVER_H_
#define SELECTSERVER_H_

void startServer(char *LISTENING_PORT);
void sendIPListToSocket(int socket);
//void *get_in_addr_server(struct sockaddr *addr);
//void displayIPAndPort(char * listening_port) ;

#endif /* SELECTSERVER_H_ */
