//
// Created by João Pedro Berton Palharini on 10/7/18.
//

#include "Utils.h"
#include "Includes.h"

bool parse_file_into_str(const char *file_name, char *shader_str, int max_len) {
    shader_str[0] = '\0'; // reset string
    FILE *file = fopen(file_name, "r");
    if ( !file ) {
        return false;
    }
    int current_len = 0;
    char line[2048];
    strcpy(line, ""); // remember to clean up before using for first time!
    while (!feof(file)) {
        if (nullptr != fgets( line, 2048, file )) {
            current_len += strlen( line ); // +1 for \n at end
            strcat( shader_str, line );
        }
    }
    if (EOF == fclose(file)) { // probably unnecesssary validation
        return false;
    }
    return true;
}