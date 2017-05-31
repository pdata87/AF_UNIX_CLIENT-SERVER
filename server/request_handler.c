
#include <stdio.h>
#include <memory.h>
#include "string_helpers.h"
#include "request_handler.h"

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