
#include <stdio.h>
#include <memory.h>
#include <sys/socket.h>
#include "string_helpers.h"
#include "request_handler.h"
#include "configuration.h"

void process_request(request *req, char *script_path) {

    command * current_command = req->commands_list->next;

    char * script_text = calloc(1024,sizeof(char));
    char * output;

        while(current_command !=NULL){
                // get bash script
                script_text = concat_with_space(script_text,script_path);
                // add commands sent by client as script arguments
                // first argument is a specific command, next are interfaces
                script_text = concat_with_space(script_text,current_command->command_text);
                command_argument * tmp_a = current_command->command_arguments;
                for(int i=0;i<current_command->command_arguments_counter;i++){
                    // add arguments send by client <xml><command><argument></argument></command></xml>
                    script_text = concat_with_space(script_text,tmp_a->command_argument_value);
                    tmp_a=tmp_a->next; // go to next attribute (example : <if>wlan0</if>
                }
                //execute bash script and assign its result into output temporary
                output = execute_bash_script(script_text);
                // set memory with empty string
                memset(script_text,0,1024);
                strcpy(req->response_text,concat_with_new_line(req->response_text,output))   ;

            // go to next parsed command
            current_command = current_command->next;

        }
        free(script_text);

}
// this function parses data sent by client
request *  handle_client_request(int client_fd) {

    request * client_request= calloc(1,sizeof(request));
    client_request->commands_list = calloc(1,sizeof(command));
    client_request->response_text = calloc(1024,sizeof(char));
    client_request->response_status = -1;
    client_request->bytes_recv_from_client = recv(client_fd, client_request->client_input, sizeof(client_request->client_input), 0);

    switch( client_request->bytes_recv_from_client){

        case -1 :
            perror("Failed to receive DATA from client: ");

            client_request->response_status = -1;
            strcpy(client_request->response_text,"");
            break;

        case 0 :
            printf(" Connection closed by client\n");
            client_request->response_status = 0;
            strcpy(client_request->response_text,"Client closed connection");
            break ;



        default:
            // if client send data, try parse
            // number of properly (in case of any ) parsed commands
            client_request->no_of_parsed_commands = parse_xml(client_request, strlen(client_request->client_input));

            if(client_request->no_of_parsed_commands > 0){
                client_request->response_status = client_request->no_of_parsed_commands;
                process_request(client_request, get_config_option("bash_script_path"));

            }



            break;

    }
    // send response to client
    send(client_fd, client_request->response_text, 1024, 0);

    // clean xml lib structures;
    xmlCleanupParser();
    xmlDictCleanup();
    xmlCleanupGlobals();
    xmlCleanupMemory();
    return client_request;

}

const char  *  execute_bash_script(char * system_command){
    FILE *pp;
    //open new process and execute command
    pp = popen(system_command, "r");
    char *buff_ptr, buff[1024] = "";
    buff_ptr = buff;
    if (pp != NULL) {

        while (1) {
            char *line;
            char buf[1000];
            line = fgets(buf, sizeof buf, pp);
            if (line == NULL) break;
            strcat(buff_ptr,line);
        }
        pclose(pp);
    }

    return buff_ptr;

}

// free memory functions
void free_client_request(request *client_request){

    free_commands_list(client_request->commands_list);
    free(client_request->response_text);
    free(client_request);
}
void free_commands_list(command *head){
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
