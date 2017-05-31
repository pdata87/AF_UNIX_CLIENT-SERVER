//
// Created by pdata on 18.05.17.
//
#include <libconfig.h>
#include <stdlib.h>

config_t cfg;

const char *str;
const int *port;
config_setting_t *setting;

int init_configuration(char * config_path){


    config_init(&cfg);






    if(!config_read_file(&cfg, config_path))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;










}
char *  get_config_option(char * option_name){

    if(config_lookup_string(&cfg, option_name, &str)){
        printf("Succesfully read of configuration variable  : %s : %s\n", option_name,str);
        return str;
    }
    else
        fprintf(stderr, "No %s setting in configuration file.\n",option_name);
        return "";

}

void destroy_configuration(){
    config_destroy(&cfg);

}

int config_load_failure(char *config_path){
    if(config_path != NULL){
        if(init_configuration(config_path)!=0){
            printf("Make sure that %s is correct path of configuration file and file is readable", config_path);
            return EXIT_FAILURE;
        }else{
            return  EXIT_SUCCESS;
        }

    }
    else {

        printf("Make sure that you are running program with config file path  passed as first argument");
        printf("Trying default config file");
        init_configuration("config");
        return EXIT_SUCCESS;
    }
}