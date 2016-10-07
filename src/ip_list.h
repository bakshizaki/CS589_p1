/*
 * ip_list.h
 *
 *  Created on: 25-Sep-2016
 *      Author: zaki
 */


#ifndef IP_LIST_H_
#define IP_LIST_H_



typedef struct ip_list {
	char *ip_address;
	char *hostname;
	char *port;
	int socket;
	struct ip_list *next;
}ip_list;

void addToIPList(ip_list **list, char *ip_address, char *hostname, char *port, int socket);
void addToStartIPList(ip_list **list, char *ip_address, char *hostname, char *port, int socket);
int removeFromIPList(ip_list **list, char *ip_address, char *hostname, char *port);
int isExistInIPList(ip_list **list, char *ip_address, char *hostname, char *port);
int isIPPortInList(ip_list **list, char *ip_address, char *port);
int SearchAndGetAddString(ip_list **list, char *ip_address, char *hostname, char *port, int socket, char *ret_string);
int SearchAndGetDeleteString(ip_list **list, char *ip_address, char *hostname, char *port, int socket, char *ret_string);
void getAddString(ip_list *list_node,char *ret_string);
void printIPList(ip_list **list);
ip_list * findBySocket(ip_list **list, int socket);
ip_list * findByIP(ip_list **list, char *ip_address);
ip_list * findByHostname(ip_list **list, char *hostname);
ip_list * findByPort(ip_list **list, char *port);
int removeByIP(ip_list **list, char *ip_address);
int removeByHostname(ip_list **list, char *hostname);
int removeByPort(ip_list **list, char *port);
int removeBySocket(ip_list **list, int socket);

void setPortOfSocket(ip_list **list,int socket,char *port);

#endif /* IP_LIST_H_ */
