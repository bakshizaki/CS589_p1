/*
 * common_methods.c
 *
 *  Created on: 25-Sep-2016
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
#include<stdbool.h>
#include<errno.h>
#include<limits.h>


#include"common_methods.h"


void * get_in_addr(struct sockaddr *addr) {
	if (addr->sa_family == AF_INET)
		return &(((struct sockaddr_in *) addr)->sin_addr);
	else
		return &(((struct sockaddr_in6 *) addr)->sin6_addr);
}


void displayIPAndPort(char * listening_port) {
	struct ifaddrs *ifaddr, *ifa;
	int family, s, n;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == NULL)
			continue;
		family=ifa->ifa_addr->sa_family;
		if(family==AF_INET) {
			s=getnameinfo(ifa->ifa_addr,(family==AF_INET)? sizeof (struct sockaddr_in): sizeof(struct sockaddr_in6),host,NI_MAXHOST,NULL,0,NI_NUMERICHOST);
			if(s!=0) {
				printf("ERROR: getnameinfo: %s\n",gai_strerror(s));
				exit(EXIT_FAILURE);
			}
			if(strcmp(ifa->ifa_name,"eth0")==0) {
				printf("Ethernet IP Address: %s\n",host);
				break;
			}
			if(strcmp(ifa->ifa_name,"wlan0")==0) {
				printf("WLAN IP Address: %s\n",host);
			}

		}
	}

	printf("Port: %s\n",listening_port);
//	exit(EXIT_SUCCESS);

}

int string_tokenizer(char *input_string,char *delimeter, char *tokens[], int token_array_size) { // split string into tokens are returns number of tokens
	char msg[BUFSIZ];
	int token_count = 0;
//	char *token_array[10];
	strcpy(msg, input_string);
	char *p = strtok(msg, delimeter);
	int i;

	while (p != NULL) {
		if (token_count == token_array_size)
				break;
		tokens[token_count]=(char *) malloc(strlen(p)*sizeof(char));
		strcpy(tokens[token_count],p);
		token_count++;
		p = strtok(NULL, delimeter);
	}
	if (p!=NULL) {
			if(DEBUG) printf("more than allowed tokens\n");
			return token_count;
		}
		if(DEBUG) {
		for(i=0;i<token_count;i++)
			printf("%s\n",tokens[i]);
		}
		return token_count;

}

int send_all(int socket, char *buf, int *len) {
	int total = 0;
	int bytesleft = *len;
	int n;
	while (total < *len) {
		n = send(socket, buf + total, bytesleft, 0);
		if (n == -1) {
			perror("send");
			break;
		}
		total += n;
		bytesleft -= n;
	}
	*len = total;
	return n == -1 ? -1 : 0;
}

int parseInt(const char *str, int *val) {
	char *temp;
	int rc = 1;
	errno = 0;
	*val = (int)strtol(str, &temp, 0);

	if (temp == str || *temp != '\0' || ((*val == LONG_MIN || *val == LONG_MAX) && errno == ERANGE))
		rc = 0;

	return rc;
}
