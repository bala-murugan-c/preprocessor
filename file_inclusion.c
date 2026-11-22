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
v26be3b6@system7:~/project/preprocessor$ vi comment_removal.c
v26be3b6@system7:~/project/preprocessor$
v26be3b6@system7:~/project/preprocessor$ cat comment_removal.c
#include <stdio.h>

void comment_removal(const char *src, const char *des)
{
        FILE *fp = fopen(src, "r");
        if (!fp)
        {
                printf("Error occurred when reading the file\n");
                return;
        }
        FILE *tmp = fopen(des, "w");
        if (!tmp)
        {
                printf("Error occurred when creating the output file\n");
                fclose(fp);
                return;
        }

        int next_ch = 0, cur_ch;
        int is_single_cmt = 0, is_multi_cmt = 0, is_string = 0;
        while ((cur_ch = fgetc(fp)) != EOF)
        {
                if (is_string == 0 && is_multi_cmt == 0 && is_single_cmt == 0)  // normal code
                {
                        if (cur_ch == '"')              // string starts
                        {
                                is_string = 1;
                                fputc(cur_ch, tmp);
                        }
                        else if (cur_ch == '/')         // maybe a comment
                        {
                                next_ch = fgetc(fp);
                                if (next_ch == '/')
                                        is_single_cmt = 1;
                                else if (next_ch == '*')
                                        is_multi_cmt = 1;
                                else
                                {
                                        fputc(cur_ch, tmp);     // normal '/'
                                        ungetc(next_ch, fp);    // read next_ch again
                                }
                        }
                        else
                                fputc(cur_ch, tmp);
                }
                else if (is_string)                     // inside a string
                {
                        fputc(cur_ch, tmp);
                        if (cur_ch == '\\')             // copy escaped char too
                        {
                                next_ch = fgetc(fp);
                                if (next_ch != EOF)
                                        fputc(next_ch, tmp);
                        }
                        else if (cur_ch == '"')         // string ends
                                is_string = 0;
                }
                else if (is_single_cmt)                 // inside // comment
                {
                        if (cur_ch == '\n')             // comment ends
                        {
                                fputc('\n', tmp);
                                is_single_cmt = 0;
                        }
                }
                else                                    // inside /* */ comment
                {
                        if (cur_ch == '\n')             // keep line count
                                fputc('\n', tmp);
                        else if (cur_ch == '*')
                        {
                                next_ch = fgetc(fp);
                                if (next_ch == '/')     // comment ends
                                {
                                        fputc(' ', tmp);        // one space
                                        is_multi_cmt = 0;
                                }
                                else
                                        ungetc(next_ch, fp);
                        }
                }
        }
        fclose(fp);
        fclose(tmp);
}
v26be3b6@system7:~/project/preprocessor$ cat comment_removal.h
#ifndef COMMENT_REMOVAL_H
#define COMMENT_REMOVAL_H

void comment_removal(const char *src, const char *des);

#endif
v26be3b6@system7:~/project/preprocessor$ cat file_inclusion.c
#include <stdio.h>
#include <string.h>
#include "file_inclusion.h"

void handle_includes(const char *src, const char *des)
{
        FILE *fp = fopen(src, "r");
        if (!fp)
        {
                printf("Error occurred when reading the file\n");
                return;
        }
        FILE *out = fopen(des, "w");
        if (!out)
        {
                printf("Error occurred when creating the output file\n");
                fclose(fp);
                return;
        }

        char line[1024], name[256];
        int i, k, ch;
        char close;

        while (fgets(line, sizeof(line), fp))
        {
                i = 0;
                while (line[i] == ' ' || line[i] == '\t')   // skip leading spaces
                        i++;

                if (strncmp(line + i, "#include", 8) == 0)
                {
                        i += 8;                             // move past "#include"
                        while (line[i] == ' ' || line[i] == '\t')
                                i++;

                        close = 0;
                        if (line[i] == '<')
                                close = '>';
                        else if (line[i] == '"')
                                close = '"';

                        if (close)                          // valid form: get the file name
                        {
                                i++;
                                k = 0;
                                while (line[i] != close && line[i] != '\n' && line[i] != '\0' && k < 255)
                                        name[k++] = line[i++];
                                name[k] = '\0';

                                FILE *inc = fopen(name, "r");
                                if (inc)                    // file found: paste its contents
                                {
                                        while ((ch = fgetc(inc)) != EOF)
                                                fputc(ch, out);
                                        fclose(inc);
                                        fputc('\n', out);   // so the next line doesn't merge
                                        continue;
                                }
                        }
                }
                fputs(line, out);                           // normal line, or file not found (<stdio.h>)

                if (line[strlen(line) - 1] != '\n')         // last line had no newline
                        fputc('\n', out);
        }
        fclose(fp);
        fclose(out);
}
