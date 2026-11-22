#include <stdio.h>
#include <string.h>
#include "comment_removal.h"
#include "file_inclusion.h"
#include "macro_handler.h"

int main(int argc, char **argv)
{
        if (argc != 2)
        {
                printf("USAGE: mypreprocessor <file_name.c>\n");
                return 1;
        }

        char file_name[256];
        size_t len = strlen(argv[1]);

        if (len < 3 || len >= sizeof(file_name) || strcmp(argv[1] + len - 2, ".c") != 0)
        {
                printf("Error: input must be a .c file\n");
                return 1;
        }

        strcpy(file_name, argv[1]);
        file_name[len - 1] = 'i';                       // test.c -> test.i

        handle_includes(argv[1], "temp1.tmp");          // 1. paste included files
        comment_removal("temp1.tmp", "temp2.tmp");      // 2. remove comments
        handle_macros("temp2.tmp", file_name);          // 3. replace macros

        remove("temp1.tmp");                            // delete temp files
        remove("temp2.tmp");

        return 0;
}
