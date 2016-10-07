/*
 * command_handling.h
 *
 *  Created on: 23-Sep-2016
 *      Author: zaki
 */

#ifndef COMMAND_HANDLING_H_
#define COMMAND_HANDLING_H_

#define MAXTOKENS 3
#define NUMBEROFCOMMANDS 10

#define CMD_HELP		0
#define CMD_CREATOR		1
#define CMD_DISPLAY		2
#define CMD_REGISTER	3
#define CMD_CONNECT		4
#define CMD_LIST		5
#define CMD_TERMINATE	6
#define CMD_QUIT		7
#define CMD_GET			8
#define CMD_PUT			9


void convertToLower(char *p);
int process_command(char *msg, int *command, char **arg1, char **arg2);




#endif /* COMMAND_HANDLING_H_ */
