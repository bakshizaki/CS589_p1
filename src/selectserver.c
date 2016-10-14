/*
 * selectserver.c
 *
 *  Created on: 04-Sep-2016
 *      Author: zaki
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <netinet/tcp.h>

#include"selectserver.h"
#include"command_handling.h"
#include"common_methods.h"
#include"ip_list.h"
#include"help_functions.h"

//#define PORT "9034"
#define FILENAME "zaki.txt"
#define MAXBUFSIZE BUFSIZ

void sendIPListToSocket(int socket);
int terminateConnectionServer(char *id, int *ret_socket);
//void sendFileToSocket(int receiver_socket, char *filename);
void quitServer();

ip_list *server_ip_list;

void startServer(char *LISTENING_PORT) {
	struct addrinfo hints, *ai_result, *p;
	int gai_result;
	int listener_socket, new_sock, temp_socket;
	fd_set fds_master, fds_read;
	int yes = 1;
	int fdmax;
	int i;
	struct sockaddr remote_addr;
	socklen_t remote_addr_len;
	char remote_addr_string[INET6_ADDRSTRLEN];
	char rec_buf[MAXBUFSIZE];
	int rec_bytes;
	char print_buf[MAXBUFSIZE];

	printf("---Starting---\n");
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((gai_result = getaddrinfo(NULL, LISTENING_PORT, &hints, &ai_result)) != 0) {
		fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(gai_result));
		exit(EXIT_FAILURE);
	}

	for (p = ai_result; p != NULL; p = p->ai_next) {
		listener_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener_socket == -1) {
			perror("socket");
			continue;
		}
		setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
		if (bind(listener_socket, p->ai_addr, p->ai_addrlen) == -1) {
			perror("bind");
			close(listener_socket);
			continue;
		}
		break;
	}
	if (p == NULL) {
		printf("could not get socket\n");
		exit(EXIT_FAILURE);
	}

	if (listen(listener_socket, 10) == -1) {
		perror("listen");
		exit(EXIT_SUCCESS);
	}
	printf("listening to socket:%d\n", listener_socket);
	FD_ZERO(&fds_master);
	FD_ZERO(&fds_read);
	FD_SET(listener_socket, &fds_master);
	FD_SET(STDIN_FILENO, &fds_master);
	fdmax = listener_socket > STDIN_FILENO ? listener_socket : STDIN_FILENO;
	write(1, ">>", 2);
	fdmax = listener_socket;
	while (1) {
		fds_read = fds_master;
		if (select(fdmax + 1, &fds_read, NULL, NULL, NULL) == -1) {
			perror("select");
		}
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &fds_read)) {
				if (i == listener_socket) {
					int s;
					char host[NI_MAXHOST];
					char *server_ip;
					char temp_host[1025];

					remote_addr_len = sizeof remote_addr;
					new_sock = accept(listener_socket, &remote_addr, &remote_addr_len);
					if (new_sock == -1) {
						perror("accept");
					} else {
						printf("new incoming connection on socket:%d\n", new_sock);
						if (new_sock > fdmax)
							fdmax = new_sock;
						FD_SET(new_sock, &fds_master);
						inet_ntop(remote_addr.sa_family, get_in_addr(&remote_addr), remote_addr_string, sizeof remote_addr_string);
						server_ip = (char *) malloc(strlen(remote_addr_string) * sizeof(char));
						strcpy(server_ip, remote_addr_string);
						s = getnameinfo(&remote_addr, sizeof(remote_addr), host, NI_MAXHOST, NULL, 0, NI_NAMEREQD);
						if (s != 0) {
							printf("ERROR: getnameinfo: %s\n", gai_strerror(s));
//							exit(EXIT_FAILURE);
						} else
							printf("hostname:%s\n", host);
						strcpy(temp_host,host);
						if(DEBUG) printf("hostname length:%d\n",strlen(host));
						printf("Connection received from %s at socket %d\n", remote_addr_string, new_sock);
						//now send all ip in list to this new host
						sendIPListToSocket(new_sock);

						addToIPList(&server_ip_list, server_ip, temp_host, NULL, new_sock);
						write(1, ">>", 2);
//						sprintf(print_buf, "%s joined chat at socket %d\n", remote_addr_string, new_sock);
//						for (j = 0; j <= fdmax; j++) {
//							if (FD_ISSET(j, &fds_master)) {
//								if (j != listener_socket) {
//									if (send(j, print_buf, strlen(print_buf), 0) == -1)
//										perror("send");
//								}
//							}
//						}
					}
				} else if (i == STDIN_FILENO) {
					int command = -1;
					char *arg1 = NULL, *arg2 = NULL;

					rec_bytes = read(i, rec_buf, sizeof rec_buf);
					if (rec_bytes == -1) {
						perror("read");
						continue;
					}
					rec_buf[rec_bytes - 1] = '\0'; //remove that fucking \n
//					printf("recv: %s\n", msg_buf);
					if (process_command(rec_buf, &command, &arg1, &arg2) == -1) {
						continue; //invalid command, skip the next part
					}
					switch (command) {

				case CMD_HELP:
					if (DEBUG)
					printf("CMD_HELP\n");
					if (arg1 != NULL || arg2 != NULL) {
						printf("Error Extra arguments\n");
						showHelpHelp();
						break;
					}

					showHelp();
					break;
				case CMD_CREATOR:
					if (arg1 != NULL || arg2 != NULL) {
						printf("Error Extra arguments\n");
						showCreatorHelp();
						break;
					}

					if (DEBUG)
					printf("CMD_CREATOR\n");
					printf("Name: Muhammed Zaki Muhammed Husain Bakshi\n");
					printf("UBIT name: mbakshi\n");
					printf("email: mbakshi@buffalo.edu\n");
					break;
				case CMD_DISPLAY:
					if (DEBUG)
					printf("CMD_DISPLAY\n");
					if (arg1 != NULL || arg2 != NULL) {
						printf("Error Extra arguments\n");
						showDisplayHelp();
						break;
					}

					displayIPAndPort(LISTENING_PORT);
					break;
				case CMD_REGISTER:
					if (DEBUG)
					printf("CMD_REGISTER\n");
					printf("REGISTER command is not valid on server\n");
					break;
				case CMD_CONNECT:
					if (DEBUG)
					printf("CMD_CONNECT\n");
					printf("Connect command is not valid on server\n");
					break;
				case CMD_LIST:
					if (DEBUG)
					printf("CMD_LIST\n");
					if (arg1 != NULL || arg2 != NULL) {
						printf("Error Extra arguments\n");
						showListHelp();
						break;
					}

					printIPList(&server_ip_list);
					break;
				case CMD_TERMINATE:
					if (DEBUG)
					printf("CMD_TERMINATE\n");
					if (terminateConnectionServer(arg1, &temp_socket) == 0) {
							printf("ERROR: Problem Terminating connection\n");
							break;
						}
						removeBySocket(&server_ip_list, temp_socket);
						FD_CLR(temp_socket, &fds_master);
						printf("Connection Successfully Terminated\n");
					break;
				case CMD_QUIT:
					if (DEBUG)
					printf("CMD_QUIT\n");
					quitServer();
					break;
				case CMD_GET:
					if (DEBUG)
					printf("CMD_GET\n");
					printf("GET Command is not valid on server\n");
					break;
				case CMD_PUT:
					if (DEBUG)
					printf("CMD_PUT\n");
					printf("PUT Command is not valid on server\n");
					break;

					}
					write(1, ">>", 2);
				} else {
					rec_bytes = recv(i, rec_buf, sizeof rec_buf, 0);
					rec_buf[rec_bytes] = '\0';
					if (rec_bytes <= 0) {
						if (rec_bytes < 0)
							perror("recv");
						else {
							//TODO: replace the below number with appropriate macro
							char send_buf[BUFSIZ];
							int sent_bytes;
							int j;
							printf("connection closed from %d socket\n", i);
							FD_CLR(i, &fds_master);
							SearchAndGetDeleteString(&server_ip_list, NULL, NULL, NULL, i, send_buf);
							sent_bytes = strlen(send_buf);
//							sprintf(print_buf, "%d left the chat\n", i);
							for (j = 0; j <= fdmax; j++) {
								if (FD_ISSET(j, &fds_master)) {
									if (j != listener_socket && j != STDIN_FILENO && j != i) {
										if (send_all(j, send_buf, &sent_bytes) == -1)
											printf("Could not send DEL Command to all\n");
									}
								}
							}
							removeBySocket(&server_ip_list, i);
						}
					} else { //received from hosts
						char *tokens[MAX_TOKENS];
						int number_of_tokens;
						int j;
						ip_list *temp_node;
						//TODO: replace the below number with appropriate macro
						char send_buf[BUFSIZ];
						int sent_bytes;

						if (DEBUG) {
							sprintf(print_buf, "%d:%s\n", i, rec_buf);
							write(1, print_buf, strlen(print_buf));
						}
						number_of_tokens = string_tokenizer(rec_buf, " ", tokens, sizeof tokens);
						if (strcmp(tokens[0], "PORT") == 0) {
							if (number_of_tokens != 2) {
								if (DEBUG)
									printf("invalid PORT message received %d\n", number_of_tokens);
								continue;
							}
							setPortOfSocket(&server_ip_list, i, tokens[1]);
							temp_node = findBySocket(&server_ip_list, i);
							getAddString(temp_node, send_buf);
							if (DEBUG)
								printf("AddString:%s\n", send_buf);
							for (j = 0; j <= fdmax; j++) {
								if (FD_ISSET(j, &fds_master)) {
									if (j != listener_socket && j != i && j != STDIN_FILENO) {
										sent_bytes = strlen(send_buf);
										if (send_all(j, send_buf, &sent_bytes) == -1)
											printf("ERROR: sendIPListToSocket\n");
									}
								}
							}

						}
//						for (j = 0; j <= fdmax; j++) {
//							if (FD_ISSET(j, &fds_master)) {
//								if (j != listener_socket && j != i) {
//									if (send(j, print_buf, strlen(print_buf), 0) == -1)
//										perror("send");
//								}
//							}
//						}
						write(1, ">>", 2);
					}
				}
			}
		}
	}

}

void sendIPListToSocket(int socket) {
	ip_list *temp_node;
	//TODO: replace the below number with appropriate macro
	char send_buf[BUFSIZ];
	int sent_bytes;

	for (temp_node = server_ip_list; temp_node != NULL; temp_node = temp_node->next) {
		getAddString(temp_node, send_buf);
		if (DEBUG)
			printf("AddString:%s\n", send_buf);
		sent_bytes = strlen(send_buf);
		if (send_all(socket, send_buf, &sent_bytes) == -1)
			printf("ERROR: sendIPListToSocket\n");
	}

}

int terminateConnectionServer(char *id, int *ret_socket) {

	int conn_id;
	int iterator = 0;
	ip_list *temp_node = server_ip_list;
if(DEBUG)	printf("ID:%d\n", conn_id);

	if(id==NULL) {
		printf("ERROR: Provide a connection ID for TERMINATE command\n");
		return 0;
	}


	if(parseInt(id, &conn_id)==0) {
		printf("Not a valid connection ID\n");
		return 0;
	}
	while (temp_node != NULL && iterator < conn_id - 1) {
		temp_node = temp_node->next;
		iterator++;
	}
	if (temp_node == NULL) {
		printf("ERROR: ID out of range\n");
		return 0;
	}
	close(temp_node->socket);
	*ret_socket = temp_node->socket;
	return 1;
}

void quitServer() {
	ip_list *temp_node = server_ip_list;
	while (temp_node != NULL) {
		close(temp_node->socket);
		temp_node = temp_node->next;
	}
	exit(EXIT_SUCCESS);
}
