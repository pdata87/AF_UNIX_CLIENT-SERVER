//
// Created by pdata on 18.05.17.
//

#include <stdlib.h>

#ifndef ZADANIE_CONFIGURATION_H
#define ZADANIE_CONFIGURATION_H

#endif //ZADANIE_CONFIGURATION_H

int init_configuration(char * config_path);
char *  get_config_option(char * option_name);
void destroy_configuration();
int config_load_failure(char *config_path);