//
// Created by pdata on 29.04.17.
//

#include <memory.h>
#include <stdio.h>
#include <malloc.h>

#include "command.h"
#include "request_handler.h"

#define XML_PARSING_ERROR             -1







int parse_client_request(request * request, int size){

    int parsing_status = -1;

    xmlDocPtr doc  = xmlReadMemory(request->client_input, size, "in-memory-xml", NULL, 0);


    if (doc == NULL) {
        // XML cannot be properly parsed


        return XML_PARSING_ERROR; // -1


    }
    else {





        set_commands_list(request->commands_list, doc);




        xmlFreeDoc(doc);



    }

}

void freeList(command * head){
    command * tmp;

    while(head!=NULL){
        tmp=head;
        if(tmp->command_arguments>0){
            command_argument * cmd_arg = tmp->command_arguments;
            command_argument *tmp_arg;
            while(cmd_arg!=NULL){
                tmp_arg=cmd_arg;
                cmd_arg = cmd_arg->next;
                free(tmp_arg);
            }
        }

        head = head->next;
        free(tmp);
    }
}
void  set_commands_list(command * commands_list, xmlDoc *xml_document){

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





}




