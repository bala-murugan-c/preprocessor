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
