/*
 * mbakshi_proj1.c
 *
 *  Created on: 23-Sep-2016
 *      Author: zaki
 *      http://stackoverflow.com/questions/14176123/correct-usage-of-strtol
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<limits.h>
#include<stdbool.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>


#include"selectclient.h"
#include"selectserver.h"
#include"ip_list.h"
#include"common_methods.h"

void show_usage(char *);
bool parseLong(const char *, long *);
void input_validation(int, char *[]);

#define SERVER 1
#define CLIENT 2

char * LISTENING_PORT;
int mode;

int main(int argc, char *argv[]) {
//	char *tokens[10],*commands[10];
//	int number_of_tokens = string_tokenizer("$help me zaki$dont help me","$", commands, 10);
//	int iterator;
//	for (iterator = 0; iterator < number_of_tokens; iterator++) {
//		printf("%s\n", commands[iterator]);
//	}
//	number_of_tokens = string_tokenizer(commands[0]," ", tokens, 10);
//	for (iterator = 0; iterator < number_of_tokens; iterator++) {
//			printf("%s\n", tokens[iterator]);
//		}
//	exit(EXIT_SUCCESS);

	/*	ip_list *head=NULL;
	 char ret_string[250];
	 printf("Starting\n");
	 addToIPList(&head,"192.168.0.1","localhost","9034",12);
	 addToIPList(&head,"192.168.0.2","sunandan","6060",60);
	 addToIPList(&head,"192.168.0.3","rahul","4555",4);
	 addToIPList(&head,"192.168.0.9","shawn","1818",18);
	 printIPList(&head);
	 removeFromIPList(&head,NULL,"localhost",NULL,0);
	 printIPList(&head);
	 getAddString(&head,"192.168.0.3",NULL,NULL,18,ret_string);
	 printf("ADD string:%s\n",ret_string);
	 memset(ret_string,0,sizeof ret_string);
	 getDeleteString(&head,NULL,NULL,NULL,60,ret_string);
	 printf("DEL string:%s\n",ret_string);

	 exit(EXIT_SUCCESS);
	 */

//	char *msg="GeT ME ZAKI";
//	int command=-1;
//	char *arg1=NULL,*arg2=NULL;
//	printf("-------Starting-------\n");
//	process_command(msg,&command,&arg1,&arg2);
//	printf("%d\n",command);
//	printf("%s\n",arg1);
//	printf("%s\n",arg2);
//	exit(EXIT_SUCCESS);
	input_validation(argc, argv);
//	printf("mode:%d \t port:%s\n",mode,PORT);
	switch (mode) {
	case SERVER:
		startServer(LISTENING_PORT);
		break;
	case CLIENT:
		startClient(LISTENING_PORT);
		break;
	default:
		printf("Invalid mode\n");
		exit(EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

void input_validation(int argc, char* argv[]) {
	long port;
	char pmode;

	if (argc != 3) {
		printf("ERROR: incorrect number of arguments\n");
		show_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (strcmp(argv[1], "c") && strcmp(argv[1], "s")) {
		printf("ERROR: incorrect mode of operation selected\n");
		show_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	pmode = argv[1][0];

	if (!parseLong(argv[2], &port)) {
		printf("ERROR: invalid port number, use an integer in range 1000-65535 for port number\n");
		show_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	if (port < 1000 || port > 65535) {
		printf("ERROR: incorrect port number selected, please select a number in range 1000-65535\n");
		show_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	if (DEBUG)
		printf("valid command received\nmode:%c\nport number:%ld\n", pmode, port);

	if (pmode == 'c')
		mode = CLIENT;
	else if (pmode == 's')
		mode = SERVER;
	LISTENING_PORT = argv[2];

}

void show_usage(char * process_name) {

	printf("Usage:\n");
	printf("Command: %s <mode> <port_number>\n", process_name);
	printf("<mode>: select mode of process. s for server, c for client\n");
	printf("<port_number>: port number which this process will listen to.\n");

}

bool parseLong(const char *str, long *val) {
	char *temp;
	bool rc = true;
	errno = 0;
	*val = strtol(str, &temp, 0);

	if (temp == str || *temp != '\0' || ((*val == LONG_MIN || *val == LONG_MAX) && errno == ERANGE))
		rc = false;

	return rc;
}

