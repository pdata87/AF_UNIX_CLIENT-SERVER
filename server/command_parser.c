//
// Created by pdata on 29.04.17.
//

#include <memory.h>
#include <stdio.h>
#include <malloc.h>
#include "request_handler.h"



int parse_xml(request *client_request, int size){


    xmlDocPtr doc  = xmlReadMemory(client_request->client_input, size, "in-memory-xml", NULL, 0);


    if (doc == NULL) {
        // XML cannot be properly parsed

        strcpy(client_request->response_text,"XML cannot be properly parsed, make sure you are sending properly formed xml document with root node ex: <xml><list/><status><if>lo</if></status></xml> \n");
        return -1 ; // -1

    }
    else {
        client_request->no_of_parsed_commands = set_commands_list(client_request->commands_list, doc);
        xmlFreeDoc(doc);
        return client_request->no_of_parsed_commands;
    }

}

// this function converts xml nodes into command list structure
int set_commands_list(command *commands_list, xmlDoc *xml_document){

    int commands_counter = 0;

    xmlNode * root = xml_document->children;
    xmlNode * current_command = root->children;
    xmlNode * current_command_element;
    command  * tmp_command;

    while(current_command !=NULL){
        commands_counter++;
        // initialize new command structure

        tmp_command = push_new_command_to_list(commands_list, (char *) current_command->name);
        current_command_element = current_command->children;

        while(current_command_element != NULL) {
            switch (current_command_element->type) {
                case XML_ELEMENT_NODE:

                    current_command_element = current_command_element->children;
                    break;

                case XML_TEXT_NODE:

                    printf("\t %s \n", current_command_element->content);
                    push_command_argument(tmp_command, current_command_element);
                    current_command_element = current_command_element->parent->next;

                    break;

                default:

                    break;
            }

        }
        //
        current_command = current_command->next;

    }
    return commands_counter;

}


command * push_new_command_to_list(command * command_list, char *command_text){


    command * new_command  = command_list;

    while(new_command->next!=NULL){
        new_command = new_command->next;
    }

    new_command->next = calloc(1,sizeof(command));
    strcpy(new_command->next->command_text,command_text);
    new_command->next->command_arguments_counter =0;
    new_command->next->command_arguments = NULL;
    new_command->next->next = NULL;

    return new_command->next;



}
// This function add new command arguments  into curent  command arguments list structure
void push_command_argument(command * cmd,xmlNode * element){

    command_argument * new_argument = calloc(1,sizeof(command_argument));
    new_argument->next = NULL;
    cmd->command_arguments_counter++;
    strcpy(new_argument->command_argument_value,(char *)element->content);
    strcpy(new_argument->command_argument_name,(char *)element->parent->name);

    if(cmd->command_arguments == NULL){
        // First element in list
        cmd->command_arguments = new_argument;
        cmd->command_arguments->first= new_argument;
        cmd->command_arguments->last = new_argument;
    }
    else{
        // check if first element equals to last. If not last element becomes current
        if(cmd->command_arguments->first == cmd->command_arguments->last){
            cmd->command_arguments->next = new_argument;
            cmd->command_arguments->last = new_argument;
        }else {
            cmd->command_arguments->last->next=new_argument;
            cmd->command_arguments->last = new_argument;
        }

    }

}


