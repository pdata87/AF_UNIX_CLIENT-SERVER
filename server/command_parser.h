//
// Created by pdata on 29.04.17.
//


#ifndef ZADANIE_COMMAND_PARSER_H
#define ZADANIE_COMMAND_PARSER_H

#endif //ZADANIE_COMMAND_PARSER_H
#include <libxml2/libxml/tree.h>


typedef struct command{
    char command_text[20];
    int command_arguments_counter; // number of additional arguments
    struct command_argument* command_arguments; // pointer to argument's list
    struct command* next;

} command;
// arguments of commands
typedef struct command_argument{
    char command_argument_name[20];
    char  command_argument_value[20];
    struct command_argument * next;
    struct command_argument * last;
    struct command_argument * first;
} command_argument;




void free_commands_list(command *head);
command * push_new_command_to_list(command * command_list, char *command_text);
void push_command_argument(command * cmd,xmlNode * element);
int set_commands_list(command *commands_list, xmlDoc *xml_document);
