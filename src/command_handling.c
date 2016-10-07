/*
 * command_handling.c
 *
 *  Created on: 23-Sep-2016
 *      Author: zaki
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"command_handling.h"

char *command_array[10] = { "help", "creator", "display", "register", "connect", "list", "terminate", "quit", "get", "put" };


void convertToLower(char *p) {
	for (; *p; ++p)
		*p = tolower(*p);
}

int process_command(char *cmd_msg, int *command, char **arg1, char **arg2) {
	char msg[50];
	int token_count = 0;
	char *token_array[3];
	strcpy(msg,cmd_msg);

	char *p = strtok(msg, " ");
	char *command_string;
	int i;

	while (p != NULL) {
		if (token_count == MAXTOKENS)
			break;
		token_array[token_count++] = p;
		p = strtok(NULL, " ");
	}
//	for(i=0;i<token_count;i++)
//		printf("%s\n",token_array[i]);
	if (p!=NULL) {
		printf("more than allowed arguments in command\n");
		return 0;
	}
	command_string = token_array[0];
//	printf("cms:%s\n",command_string);
	convertToLower(command_string);
	for (i = 0; i < NUMBEROFCOMMANDS; i++) {
		if (strcmp(command_string, command_array[i]) == 0) {
//			printf("found\n");
			*command = i;
		}
	}
	if (*command < 0 || *command > 9) {
		printf("not a valid command\n");
		return 0;
	}
	if (token_count >= 2) {
		*arg1=(char *)malloc(sizeof(char) * sizeof token_array[1]);
		strcpy(*arg1,token_array[1]);
	}
	if (token_count == 3) {
		*arg2=(char *)malloc(sizeof(char) * sizeof token_array[2]);
		strcpy(*arg2,token_array[2]);
	}
	return 1;
}
