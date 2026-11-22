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
