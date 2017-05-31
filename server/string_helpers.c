//
// Created by pdata on 09.05.17.
//


#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

 char* concat_with_space( char *s1,  char *s2)
{




    //in real code you would check for errors in malloc here


    strcat(s1, " ");
    strcat(s1, s2);

    return s1;

}
 char* concat_with_new_line( char *s1,  char *s2)
{

    strcat(s1, "\n");
    strcat(s1, s2);


    return s1;
}