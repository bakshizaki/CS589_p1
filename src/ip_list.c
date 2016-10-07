/*
 * ip_list.c
 *
 *  Created on: 25-Sep-2016
 *      Author: zaki
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"ip_list.h"

void addToIPList(ip_list **list, char *ip_address, char *hostname, char *port, int socket) {
	ip_list *p;
	ip_list *temp_node = (ip_list *) malloc(sizeof(ip_list));
	if (temp_node == NULL) {
		printf("node creation failed\n");
		return;
	}
	if (ip_address != NULL) {
		temp_node->ip_address = (char *) malloc(strlen(ip_address) * sizeof(char));
		strcpy(temp_node->ip_address, ip_address);
	}

	if (hostname != NULL) {
		temp_node->hostname = (char *) malloc(strlen(hostname) * sizeof(char));
		strcpy(temp_node->hostname, hostname);
	}

	if (port != NULL) {
		temp_node->port = (char *) malloc(strlen(port) * sizeof(char));
		strcpy(temp_node->port, port);
	}
	temp_node->socket = socket;
	temp_node->next=NULL;
	if (*list == NULL) {
		*list = temp_node;
		return;
	}
	p = *list;
	while (p->next != NULL) {
		p = p->next;
	}
	p->next = temp_node;
}

void addToStartIPList(ip_list **list, char *ip_address, char *hostname, char *port, int socket) {
	ip_list *p;
	ip_list *temp_node = (ip_list *) malloc(sizeof(ip_list));
	if (temp_node == NULL) {
		printf("node creation failed\n");
		return;
	}
	if (ip_address != NULL) {
		temp_node->ip_address = (char *) malloc(strlen(ip_address) * sizeof(char));
		strcpy(temp_node->ip_address, ip_address);
	}

	if (hostname != NULL) {
		temp_node->hostname = (char *) malloc(strlen(hostname) * sizeof(char));
		strcpy(temp_node->hostname, hostname);
	}

	if (port != NULL) {
		temp_node->port = (char *) malloc(strlen(port) * sizeof(char));
		strcpy(temp_node->port, port);
	}
	temp_node->socket = socket;
	temp_node->next= *list;
	*list=temp_node;
	return;
}



int removeFromIPList(ip_list **list, char *ip_address, char *hostname, char *port) {
	ip_list *prev_node, *current_node, *temp_node;
	prev_node = NULL;
	current_node = *list;

	if (list == NULL)
		return 0;

	if (strcmp(current_node->ip_address, ip_address) == 0 && strcmp(current_node->hostname, hostname) == 0 && strcmp(current_node->port, port) == 0) {
		temp_node = current_node;
		*list = current_node->next;
		free(temp_node);
		return 1;
	}
	while (current_node != NULL) {
		if (strcmp(current_node->ip_address, ip_address) == 0 && strcmp(current_node->hostname, hostname) == 0 && strcmp(current_node->port, port) == 0)
			break;
		prev_node = current_node;
		current_node = current_node->next;
	}
	if (strcmp(current_node->ip_address, ip_address) != 0 || strcmp(current_node->hostname, hostname) != 0 || strcmp(current_node->port, port) != 0) {
		printf("Could not find element\n");
		return 0;
	}
	temp_node = current_node;
	prev_node->next = current_node->next;
	free(temp_node);
	return 1;


/*	if (ip_address != NULL) {
		if (removeByIP(list, ip_address))
			return 1;
		else
			return 0;
	}
	if (hostname != NULL) {
		if (removeByHostname(list, hostname))
			return 1;
		else
			return 0;
	}
	if (port != NULL) {
		if (removeByPort(list, port))
			return 1;
		else
			return 0;
	}
	if (socket != 0) {
		if (removeBySocket(list, socket))
			return 1;
		else
			return 0;
	}
	return 0;*/
}

int isExistInIPList(ip_list **list, char *ip_address, char *hostname, char *port) {
	ip_list *current_node;
	current_node = *list;

	if (list == NULL)
		return 0;

	if (strcmp(current_node->ip_address, ip_address) == 0 && strcmp(current_node->hostname, hostname) == 0 && strcmp(current_node->port, port) == 0) {
		return 1;
	}
	while (current_node != NULL) {
		if (strcmp(current_node->ip_address, ip_address) == 0 && strcmp(current_node->hostname, hostname) == 0 && strcmp(current_node->port, port) == 0)
			break;
		current_node = current_node->next;
	}
	if(current_node==NULL)
		return 0;
	if (strcmp(current_node->ip_address, ip_address) != 0 || strcmp(current_node->hostname, hostname) != 0 || strcmp(current_node->port, port) != 0) {
		printf("Could not find element\n");
		return 0;
	}
	return 1;
}

int isIPPortInList(ip_list **list, char *ip_address, char *port) {
	ip_list *current_node;
	current_node = *list;

	if (list == NULL)
		return 0;

	if (strcmp(current_node->ip_address, ip_address) == 0 && strcmp(current_node->port, port) == 0) {
		return 1;
	}
	while (current_node != NULL) {
		if (strcmp(current_node->ip_address, ip_address) == 0 && strcmp(current_node->port, port) == 0)
			break;
		current_node = current_node->next;
	}
	if(current_node==NULL)
		return 0;
	if (strcmp(current_node->ip_address, ip_address) != 0 || strcmp(current_node->port, port) != 0) {
		printf("Could not find element\n");
		return 0;
	}
	return 1;
}

int removeByIP(ip_list **list, char *ip_address) {
	ip_list *prev_node, *current_node, *temp_node;
	prev_node = NULL;
	current_node = *list;
	if (strcmp(current_node->ip_address, ip_address) == 0) {
		temp_node = current_node;
		*list = current_node->next;
		free(temp_node);
		return 0;
	}
	while (current_node != NULL) {
		if (strcmp(current_node->ip_address, ip_address) == 0)
			break;
		prev_node = current_node;
		current_node = current_node->next;
	}
	if (strcmp(current_node->ip_address, ip_address) != 0) {
		printf("Could not find element by IP");
		return 0;
	}
	temp_node = current_node;
	prev_node->next = current_node->next;
	free(temp_node);
	return 1;
}

int removeByHostname(ip_list **list, char *hostname) {
	ip_list *prev_node, *current_node, *temp_node;
	prev_node = NULL;
	current_node = *list;
	if (strcmp(current_node->hostname, hostname) == 0) {
		temp_node = current_node;
		*list = current_node->next;
		free(temp_node);
		return 0;
	}
	while (current_node != NULL) {
		if (strcmp(current_node->hostname, hostname) == 0)
			break;
		prev_node = current_node;
		current_node = current_node->next;
	}
	if (strcmp(current_node->hostname, hostname) != 0) {
		printf("Could not find element by Hostname");
		return 0;
	}
	temp_node = current_node;
	prev_node->next = current_node->next;
	free(temp_node);
	return 1;
}

int removeByPort(ip_list **list, char *port) {
	ip_list *prev_node, *current_node, *temp_node;
	prev_node = NULL;
	current_node = *list;
	if (strcmp(current_node->port, port) == 0) {
		temp_node = current_node;
		*list = current_node->next;
		free(temp_node);
		return 0;
	}
	while (current_node != NULL) {
		if (strcmp(current_node->port, port) == 0)
			break;
		prev_node = current_node;
		current_node = current_node->next;
	}
	if (strcmp(current_node->port, port) != 0) {
		printf("Could not find element by Port");
		return 0;
	}
	temp_node = current_node;
	prev_node->next = current_node->next;
	free(temp_node);
	return 1;
}

int removeBySocket(ip_list **list, int socket) {
	ip_list *prev_node, *current_node, *temp_node;
	prev_node = NULL;
	current_node = *list;
	if (current_node->socket == socket) {
		temp_node = current_node;
		*list = current_node->next;
		free(temp_node);
		return 0;
	}
	while (current_node != NULL) {
		if (current_node->socket == socket)
			break;
		prev_node = current_node;
		current_node = current_node->next;
	}
	if (current_node->socket != socket) {
		printf("Could not find element by Socket");
		return 0;
	}
	temp_node = current_node;
	prev_node->next = current_node->next;
	free(temp_node);
	return 1;
}

int SearchAndGetAddString(ip_list **list, char *ip_address, char *hostname, char *port, int socket, char *ret_string) {
	char *add_string = (char *) malloc(250 * sizeof(char));
	ip_list *current_node = NULL;
	if (ip_address != NULL) {
		current_node = findByIP(list, ip_address);
	}
	if (current_node == NULL && socket != 0) {
		current_node = findBySocket(list, socket);
	}
	if (current_node == NULL && hostname != NULL) {
		current_node = findByHostname(list, hostname);
	}
	if (current_node == NULL && port != NULL) {
		current_node = findByPort(list, port);
	}

	if (current_node == NULL) {
		printf("getAddString: could not find record with any parameter\n");
		return 0;
	}

	sprintf(add_string, "add %s %s %s", current_node->ip_address, current_node->hostname, current_node->port);
	strcpy(ret_string, add_string);
	return 1;
}

int SearchAndGetDeleteString(ip_list **list, char *ip_address, char *hostname, char *port, int socket, char *ret_string) {
	char *del_string = (char *) malloc(250 * sizeof(char));
	ip_list *current_node = NULL;
	if (ip_address != NULL) {
		current_node = findByIP(list, ip_address);
	}
	if (current_node == NULL && socket != 0) {
		current_node = findBySocket(list, socket);
	}
	if (current_node == NULL && hostname != NULL) {
		current_node = findByHostname(list, hostname);
	}
	if (current_node == NULL && port != NULL) {
		current_node = findByPort(list, port);
	}

	if (current_node == NULL) {
		printf("getDeleteString: could not find record with any parameter\n");
		return 0;
	}

	sprintf(del_string, "$DEL %s %s %s", current_node->ip_address, current_node->hostname, current_node->port);
	strcpy(ret_string, del_string);
	return 1;
}

ip_list * findByIP(ip_list **list, char *ip_address) {
	ip_list *current_node;
	current_node = *list;
	while (current_node != NULL) {
		if (strcmp(current_node->ip_address, ip_address) == 0)
			break;
		current_node = current_node->next;
	}
	if (current_node == NULL) {
		printf("findByIP: Could not find element by IP\n");
		return NULL;
	}
	return current_node;
}

ip_list * findByHostname(ip_list **list, char *hostname) {
	ip_list *current_node;
	current_node = *list;
	while (current_node != NULL) {
		if (strcmp(current_node->hostname, hostname) == 0)
			break;
		current_node = current_node->next;
	}
	if (current_node == NULL) {
		printf("findByHostname:  Could not find element by Hostname\n");
		return NULL;
	}
	return current_node;
}

ip_list * findByPort(ip_list **list, char *port) {
	ip_list *current_node;
	current_node = *list;
	while (current_node != NULL) {
		if (strcmp(current_node->port, port) == 0)
			break;
		current_node = current_node->next;
	}
	if (current_node == NULL) {
		printf("findByPort: Could not find element by Port\n");
		return NULL;
	}
	return current_node;
}

ip_list * findBySocket(ip_list **list, int socket) {
	ip_list *current_node;
	current_node = *list;
	while (current_node != NULL) {
		if (current_node->socket == socket)
			break;
		current_node = current_node->next;
	}
	if (current_node == NULL) {
		printf("findBySocket: Could not find element by Socket\n");
		return NULL;
	}
	return current_node;
}

void printIPList(ip_list **list) {
	ip_list *current_node;
	current_node = *list;
	int id=1;
	printf("id  Hostname                      IP address          Port No.  Socket\n");
	while (current_node != NULL) {
//		printf("%-4d%-30s%-20s%-8s\n",id++,current_node->hostname,current_node->ip_address,current_node->port);
		printf("%-4d%-30s%-20s%-10s%-6d\n",id++,current_node->hostname,current_node->ip_address,current_node->port,current_node->socket);
		current_node = current_node->next;
	}
}
void setPortOfSocket(ip_list **list, int socket, char *port) {
	ip_list *temp_node = findBySocket(list, socket);
	if (port != NULL) {
		temp_node->port = (char *) malloc(strlen(port) * sizeof(char));
		strcpy(temp_node->port, port);
	}
}

void getAddString(ip_list *list_node, char *ret_string) {
	if (list_node == NULL)
		return;
	char *add_string = (char *) malloc(250 * sizeof(char));
	sprintf(add_string, "$ADD %s %s %s", list_node->ip_address, list_node->hostname, list_node->port);
	strcpy(ret_string, add_string);
}
