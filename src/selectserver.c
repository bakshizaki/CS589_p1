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

#include"selectserver.h"
#include"command_handling.h"
#include"common_methods.h"
#include"ip_list.h"
#include"help_functions.h"

//#define PORT "9034"
#define FILENAME "zaki.txt"
#define MAXBUFSIZE BUFSIZ

void sendIPListToSocket(int socket);
void sendFileToSocket(int receiver_socket);

ip_list *server_ip_list;

void startServer(char *LISTENING_PORT) {
	struct addrinfo hints, *ai_result, *p;
	int gai_result;
	int listener_socket, new_sock;
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

						printf("Connection received from %s at socket %d\n", remote_addr_string, new_sock);
						//now send all ip in list to this new host
						sendIPListToSocket(new_sock);

						addToIPList(&server_ip_list, server_ip, host, NULL, new_sock);
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
					int j;
				case CMD_HELP:
					printf("CMD_HELP\n");
					break;
				case CMD_CREATOR:
					printf("CMD_CREATOR\n");
					printf("Name: Muhammed Zaki Muhammed Husain Bakshi\n");
					printf("UBIT name: mbakshi\n");
					printf("email: mbakshi@buffalo.edu\n");
					break;
				case CMD_DISPLAY:
					printf("CMD_DISPLAY\n");
					displayIPAndPort(LISTENING_PORT);
					break;
				case CMD_REGISTER:
					printf("CMD_REGISTER\n");
					break;
				case CMD_CONNECT:
					printf("CMD_CONNECT\n");
					break;
				case CMD_LIST:
					printf("CMD_LIST\n");
					printIPList(&server_ip_list);
					break;
				case CMD_TERMINATE:
					printf("CMD_TERMINATE\n");
					break;
				case CMD_QUIT:
					printf("CMD_QUIT\n");
					break;
				case CMD_GET:
					printf("CMD_GET\n");
					break;
				case CMD_PUT:

					printf("CMD_PUT\n");
					sendFileToSocket(atoi(arg1));
					/*sprintf(print_buf, "some bullshit to send");
					 for (j = 0; j <= fdmax; j++) {
					 if (FD_ISSET(j, &fds_master)) {
					 if (j != listener_socket && j != STDIN_FILENO) {
					 printf("sending something\n");
					 if (send(j, print_buf, strlen(print_buf), 0) == -1)
					 perror("send");
					 }
					 }
					 }*/
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
						int iterator, j;
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

void sendFileToSocket(int receiver_socket) {
	int file_fd;
	struct stat file_stat;
	char file_size_string[100];
	off_t file_offset;
	int remaining_bytes;
	int sent_bytes;
	int msg_len;

	file_fd = open(FILENAME, O_RDONLY);
	if (file_fd == -1) {
		fprintf(stderr, "ERROR open():%s\n", strerror(errno));
		return;
	}
	if (fstat(file_fd, &file_stat) < 0) {
		fprintf(stderr, "ERROR fstat():%s\n", strerror(errno));
		return;
	}
	printf("File size:%d bytes\n",(int)file_stat.st_size);
	sprintf(file_size_string,"$FSIZE %d",(int)file_stat.st_size);
	msg_len=strlen(file_size_string);
	send_all(receiver_socket, file_size_string, &msg_len);

	file_offset=0;
	remaining_bytes=file_stat.st_size;
	if(DEBUG) printf("FileSocket:%d\n",receiver_socket);


//	sent_bytes=sendfile(receiver_socket, file_fd,&file_offset, file_stat.st_size);
//	if(sent_bytes==-1)
//		perror("sendfile");
//	if(sent_bytes!=file_stat.st_size) {
//		printf("Incomplete file transfer %d\n",sent_bytes);
//	}

	while((sent_bytes=sendfile(receiver_socket, file_fd,&file_offset, BUFSIZ))>0 && (remaining_bytes>0)) {
		if(sent_bytes==-1)
			perror("ERROR sendfile");
		remaining_bytes=remaining_bytes-sent_bytes;
		printf("sent bytes:%d\t remaining bytes:%d\n",sent_bytes,remaining_bytes);

	}
	printf("Done sending\n");
	close(file_fd);


}
