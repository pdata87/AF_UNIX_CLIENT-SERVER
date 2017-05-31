//
// Created by pdata on 09.05.17.
//




#include "command_parser.h"
#ifndef ZADANIE_REQUEST_HANDLER_H
#endif //ZADANIE_REQUEST_HANDLER_H


typedef struct request{
    int bytes_recv_from_client;
    char  client_input[1024];
    int no_of_parsed_commands;
    int response_status;
    char * response_text;
    command * commands_list;
} request;

request *  handle_client_request(int client_fd);
void process_request(request *req, char *script_path);
const char  *  execute_bash_script(char *system_command);
void free_commands_list(command *head);
void free_client_request(request *client_request);
int parse_xml(request *client_request, int size);
