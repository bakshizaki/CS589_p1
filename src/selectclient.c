/*
 * selectclient.c
 *
 *  Created on: 23-Sep-2016
 *      Author: zaki
 */
#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>

#include "selectclient.h"
#include"command_handling.h"
#include"common_methods.h"
#include"help_functions.h"
#include"ip_list.h"

#define MAXBUFSIZE BUFSIZ
#define REC_FILENAME "rec_file.txt"

int registerToServer(char *server_ip, char *server_port, int *server_socket, char* client_listening_port);
int connectToHost(char *host_ip, char *host_port, int *host_socket, char* client_listening_port);
int terminateConnection(char *id, int *ret_socket);
void quit();

ip_list *client_ip_list, *peer_list;
int is_registered = 0; //0=false
int number_of_connections = 0;

void startClient(char * LISTENING_PORT) {
	struct addrinfo hints, *ai_result, *p;
	int gai_result;
	int listener_socket, new_sock, temp_socket;
	struct sockaddr remote_address, self_address;
	socklen_t remote_addr_len;
	char remote_address_string[INET6_ADDRSTRLEN], self_address_string[INET6_ADDRSTRLEN];
	fd_set fds_master, fds_read;
	char rec_buf[MAXBUFSIZE];
	int fdmax = 0;
	int rec_bytes;
	int yes = 1;
	int server_socket = -1;
	int remaining_file_bytes;
	int is_doing_file_transfer=0;
	FILE *received_file;

	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((gai_result = getaddrinfo(NULL, LISTENING_PORT, &hints, &ai_result)) != 0) {
		fprintf(stderr, "ERROR in getaddrinfo: %s", gai_strerror(gai_result));
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
		printf("ERROR: could not connect\n");
		exit(EXIT_FAILURE);
	}

	if (listen(listener_socket, BACKLOG) == -1) {
		perror("listen");
		exit(EXIT_SUCCESS);
	}

	printf("Client listening on port %s\n", LISTENING_PORT);
//	inet_ntop(p->ai_family, get_in_addr(p->ai_addr), self_address_string, sizeof self_address_string);
//	printf("My address:%s\n", self_address_string);
	FD_ZERO(&fds_master);
	FD_ZERO(&fds_read);
	FD_SET(listener_socket, &fds_master);
	FD_SET(STDIN_FILENO, &fds_master);
	fdmax = listener_socket > STDIN_FILENO ? listener_socket : STDIN_FILENO;

	while (1) {
		int i;
		fds_read = fds_master;
		memset(rec_buf, 0, sizeof rec_buf);
		if (select(fdmax + 1, &fds_read, NULL, NULL, NULL) == -1) {
			perror("select");
			continue;
		}
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &fds_read)) {
				if (i == listener_socket) {
					int s;
					char host[NI_MAXHOST];
					char *host_ip;

					remote_addr_len = sizeof remote_address;
					new_sock = accept(listener_socket, &remote_address, &remote_addr_len);
					if (new_sock == -1) {
						perror("accept");
					} else {
						printf("new incoming connection on socket:%d\n", new_sock);
						if (new_sock > fdmax)
							fdmax = new_sock;
						FD_SET(new_sock, &fds_master);
						inet_ntop(remote_address.sa_family, get_in_addr(&remote_address), remote_address_string, sizeof remote_address_string);
						host_ip = (char *) malloc(strlen(remote_address_string) * sizeof(char));
						strcpy(host_ip, remote_address_string);
						s = getnameinfo(&remote_address, sizeof(remote_address), host, NI_MAXHOST, NULL, 0, NI_NAMEREQD);
						if (s != 0) {
							printf("ERROR: getnameinfo: %s\n", gai_strerror(s));
//							exit(EXIT_FAILURE);
						} else
							printf("hostname:%s\n", host);

						printf("Connection received from %s at socket %d\n", remote_address_string, new_sock);

						addToIPList(&peer_list, host_ip, host, NULL, new_sock);
						write(1, ">>", 2);

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
						break;
					case CMD_CREATOR:
						if (arg1 != NULL || arg2 != NULL) {
							printf("Error CMD_CREATOR: Extra arguments\n");
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
						displayIPAndPort(LISTENING_PORT);
						break;
					case CMD_REGISTER:
						if (is_registered == 1) {
							printf("Client already registered\n");
							break;
						}
						if (registerToServer(arg1, arg2, &server_socket, LISTENING_PORT) == 1) {
							if (DEBUG)
								printf("server socket:%d\n", server_socket);
							FD_SET(server_socket, &fds_master);
							if (server_socket > fdmax)
								fdmax = server_socket;
						}
						if (DEBUG)
							printf("CMD_REGISTER\n");
						break;
					case CMD_CONNECT:
						if (number_of_connections == 3) {
							printf("Can't connect to more than 3 hosts\n");
							break;
						}

						if (connectToHost(arg1, arg2, &temp_socket, LISTENING_PORT) == 1) {
							if (DEBUG)
								printf("host socket:%d\n", temp_socket);
							FD_SET(temp_socket, &fds_master);
							if (temp_socket > fdmax)
								fdmax = temp_socket;
						}

						if (DEBUG)
							printf("CMD_CONNECT\n");
						break;
					case CMD_LIST:
						if (DEBUG)
							printf("CMD_LIST\n");
						printf("-------Client-IP-List-------\n");
						printIPList(&client_ip_list);
						printf("-------Client-Peer-List-------\n");
						printIPList(&peer_list);
						break;
					case CMD_TERMINATE:
						if (terminateConnection(arg1, &temp_socket) == 0) {
							printf("ERROR: Problem Terminating connection\n");
						}
						removeBySocket(&peer_list, temp_socket);
						FD_CLR(temp_socket, &fds_master);
						if (DEBUG)
							printf("CMD_TERMINATE\n");
						break;
					case CMD_QUIT:
						quit();
						if (DEBUG)
							printf("CMD_QUIT\n");
						break;
					case CMD_GET:
						if (DEBUG)
							printf("CMD_GET\n");
						break;
					case CMD_PUT:
						if (DEBUG)
							printf("CMD_PUT\n");
						break;

					}
					printf(">>");
					fflush(stdout);

				} else if (i == server_socket) {
					char *messages[MAX_MSGS];
					int number_of_messages;
					int iterator;

					rec_bytes = recv(i, rec_buf, sizeof rec_buf, 0);
					if (rec_bytes == -1) {
						perror("recv");
						continue;
					}
					if (rec_bytes == 0) {
						printf("Connection terminated by server\n");
						FD_CLR(server_socket, &fds_master);
						removeBySocket(&client_ip_list, server_socket);
						removeBySocket(&peer_list, server_socket);
						is_registered = 0;
						continue;
					}
					rec_buf[rec_bytes] = '\0';
					if (DEBUG)
						printf("From server: %s\n", rec_buf);

					if (rec_buf[0] == '$') {
						number_of_messages = string_tokenizer(rec_buf, "$", messages, sizeof messages);

						for (iterator = 0; iterator < number_of_messages; ++iterator) {
							char *tokens[MAX_TOKENS];
							int number_of_tokens;
							number_of_tokens = string_tokenizer(messages[iterator], " ", tokens, sizeof tokens);
							if (strcmp(tokens[0], "ADD") == 0) {
								if (number_of_tokens != 4) {
									if (DEBUG)
										printf("invalid ADD message received Tokens:%d\n", number_of_tokens);
									continue;
								}
								if (isExistInIPList(&client_ip_list, tokens[1], tokens[2], tokens[3]) == 0)
									addToIPList(&client_ip_list, tokens[1], tokens[2], tokens[3], -1);

							}
							if (strcmp(tokens[0], "DEL") == 0) {
								if (number_of_tokens != 4) {
									if (DEBUG)
										printf("invalid DEL message received, Tokens:%d\n", number_of_tokens);
									continue;
								}
								if (removeFromIPList(&client_ip_list, tokens[1], tokens[2], tokens[3]) != 1)
									printf("Could not remove %s from client_ip_list\n", tokens[1]);
							}
							if (strcmp(tokens[0], "FSIZE") == 0) {
								if (number_of_tokens != 2) {
									if (DEBUG)
										printf("invalid FSIZE message received, Tokens:%d\n", number_of_tokens);
									continue;
								}

								parseInt(tokens[1], &remaining_file_bytes);
								received_file = fopen(REC_FILENAME, "w");
								is_doing_file_transfer=1;
							}

							memset(tokens, 0, sizeof tokens);
						}
					} else { //not a command but file received
						if(is_doing_file_transfer==1) {
							fwrite( rec_buf, sizeof(char), rec_bytes, received_file);
							remaining_file_bytes -= rec_bytes;
							printf("Received %d bytes \t %d bytes left\n",rec_bytes,remaining_file_bytes);
						}
						if(remaining_file_bytes==0) {
							printf("Received complete file\n");
							is_doing_file_transfer=0;
							close(received_file);
						}
					}

				} else { //received from other hosts

					char *tokens[MAX_TOKENS];
					int number_of_tokens;

					rec_bytes = recv(i, rec_buf, sizeof rec_buf, 0);
					if (rec_bytes == -1) {
						perror("recv");
						continue;
					}
					if (rec_bytes == 0) {
						printf("Connection terminated by host at socket:%d\n", i);
						FD_CLR(i, &fds_master);
						removeBySocket(&peer_list, i);
						number_of_connections--;
						continue;
					}
					rec_buf[rec_bytes] = '\0';
					printf("%s\n", rec_buf);

					number_of_tokens = string_tokenizer(rec_buf, " ", tokens, sizeof tokens);
					if (strcmp(tokens[0], "PORT") == 0) {
						if (number_of_tokens != 2) {
							if (DEBUG)
								printf("invalid PORT message received %d\n", number_of_tokens);
							continue;
						}
						setPortOfSocket(&peer_list, i, tokens[1]);
					}
//					rec_bytes = recv(i, rec_buf, sizeof rec_buf, 0);
//					if (rec_bytes == -1) {
//						perror("recv");
//						continue;
//					}
//					if (rec_bytes == 0) {
//						printf("Connection terminated by server\n");
//						continue;
//					}
//					rec_buf[rec_bytes] = '\0';
//					printf("%s\n", rec_buf);

				}
			}
		}
	}

}

int registerToServer(char *server_ip, char *server_port, int *server_socket, char* client_listening_port) {
	struct addrinfo hints, *ai_result, *p;
	char buf[MAXBUFSIZE];
	int socket_fd;
	int gai_res;
	char server_addr[INET6_ADDRSTRLEN];
	int bytes_to_send;
	char host[NI_MAXHOST];
	int s;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((gai_res = getaddrinfo(server_ip, server_port, &hints, &ai_result)) != 0) {
		fprintf(stderr, "ERROR getaddrinfo:%s\n", gai_strerror(gai_res));
		return 0;
	}

	for (p = ai_result; p != NULL; p = p->ai_next) {
		socket_fd = (socket(p->ai_family, p->ai_socktype, p->ai_protocol));
		if (socket_fd == -1) {
			perror("ERROR socket");
			continue;
		}
		if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_fd);
			perror("ERROR connect");
			continue;
		}
		break;
	}

	if (p == NULL) {
		printf("connect failed exiting\n");
		return 0;
	}

	inet_ntop(p->ai_family, get_in_addr(p->ai_addr), server_addr, sizeof server_addr);
	printf("Connected to %s\n", server_addr);
	strcpy(server_ip, server_addr);
	s = getnameinfo(p->ai_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, 0);
	if (s != 0) {
		printf("ERROR: getnameinfo: %s\n", gai_strerror(s));
		return 0;
	}
	memset(buf, '\0', MAXBUFSIZE);
	sprintf(buf, "PORT %s", client_listening_port);
	bytes_to_send = strlen(buf);
	if (send_all(socket_fd, buf, &bytes_to_send) == -1) {
		printf("Couldnt send all bytes\n");
		return 0;
	}

	addToStartIPList(&client_ip_list, server_ip, host, server_port, socket_fd);
	addToStartIPList(&peer_list, server_ip, host, server_port, socket_fd);
//	printIPList(&client_ip_list);
	freeaddrinfo(ai_result);
	*server_socket = socket_fd;
	is_registered = 1;
	return 1;

}

int connectToHost(char *host_ip, char *host_port, int *host_socket, char* client_listening_port) {
	struct addrinfo hints, *ai_result, *p;
	char buf[MAXBUFSIZE];
	int socket_fd;
	int gai_res;
	char host_addr[INET6_ADDRSTRLEN];
	int bytes_to_send;
	char host[NI_MAXHOST];
	int s;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((gai_res = getaddrinfo(host_ip, host_port, &hints, &ai_result)) != 0) {
		fprintf(stderr, "ERROR getaddrinfo:%s\n", gai_strerror(gai_res));
		return 0;
	}

	p = ai_result;
	inet_ntop(p->ai_family, get_in_addr(p->ai_addr), host_addr, sizeof host_addr);
	strcpy(host_ip, host_addr);
	if (isIPPortInList(&peer_list, host_ip, host_port) == 1) {
		printf("Already connected to peer\n");
		return 0;
	}

	if (isIPPortInList(&client_ip_list, host_ip, host_port) == 0) {
		printf("The requested host is not registered with server\n");
		return 0;
	}

	for (p = ai_result; p != NULL; p = p->ai_next) {
		socket_fd = (socket(p->ai_family, p->ai_socktype, p->ai_protocol));
		if (socket_fd == -1) {
			perror("ERROR socket");
			continue;
		}
		if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_fd);
			perror("ERROR connect");
			continue;
		}
		break;
	}

	if (p == NULL) {
		printf("connect failed exiting\n");
		return 0;
	}

	inet_ntop(p->ai_family, get_in_addr(p->ai_addr), host_addr, sizeof host_addr);
	printf("Connected to %s\n", host_addr);
	strcpy(host_ip, host_addr);
	s = getnameinfo(p->ai_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, 0);
	if (s != 0) {
		printf("ERROR: getnameinfo: %s\n", gai_strerror(s));
		return 0;
	}
	memset(buf, '\0', MAXBUFSIZE);
	sprintf(buf, "PORT %s", client_listening_port);
	bytes_to_send = strlen(buf);
	if (send_all(socket_fd, buf, &bytes_to_send) == -1) {
		printf("Couldnt send all bytes\n");
		return 0;
	}

	addToIPList(&peer_list, host_ip, host, host_port, socket_fd); //here host is hostname
	freeaddrinfo(ai_result);
	*host_socket = socket_fd;
	number_of_connections++;
	return 1;

}

int terminateConnection(char *id, int *ret_socket) {
	char *endp;
	int conn_id = (int) strtol(id, &endp, 10);
	int iterator = 0;
	ip_list *temp_node = peer_list;
	printf("ID:%d\n", conn_id);
	//TODO: check of valid id
//	if(endp==id || *endp== '\0') {
//		printf("ERROR: Invalid ID\n");
//		return 0;
//	}

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

void quit() {
	ip_list *temp_node = peer_list;
	while (temp_node != NULL) {
		close(temp_node->socket);
		temp_node = temp_node->next;
	}
	exit(EXIT_SUCCESS);
}
