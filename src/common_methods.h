/*
 * common_methods.h
 *
 *  Created on: 25-Sep-2016
 *      Author: zaki
 */
#include<stdbool.h>


#ifndef COMMON_METHODS_H_
#define COMMON_METHODS_H_

#define MAX_TOKENS 10
#define MAX_MSGS 10
#define DEBUG 1

void displayIPAndPort(char * listening_port);
void * get_in_addr(struct sockaddr *addr);
int string_tokenizer(char *input_string,char *delimeter, char *tokens[], int token_array_size);
int send_all(int socket, char *buf, int *len);
bool parseInt(const char *str, int *val);

#endif /* COMMON_METHODS_H_ */
