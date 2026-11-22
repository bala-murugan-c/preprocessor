#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "macro_handler.h"

#define MAX 50
#define SIZE 2048

struct macro
{
        char name[32];
        int nparam;             // -1 = simple macro, 0 or more = has arguments
        char param[4][16];
        char body[256];
};

static struct macro table[MAX];
static int count = 0;

static int is_word(int c)
{
        return isalnum(c) || c == '_';
}

static struct macro *find(const char *name)
{
        for (int i = 0; i < count; i++)
                if (strcmp(table[i].name, name) == 0)
                        return &table[i];
        return NULL;
}

static void add(char *out, const char *s)       // append s to out
{
        if (strlen(out) + strlen(s) < SIZE)
                strcat(out, s);
}

// i points just after "#define"
static void store_macro(const char *line, int i)
{
        struct macro m;
        int k = 0;

        memset(&m, 0, sizeof(m));
        m.nparam = -1;

        while (line[i] == ' ' || line[i] == '\t')
                i++;
        while (is_word(line[i]) && k < 31)              // name
                m.name[k++] = line[i++];

        if (line[i] == '(')                             // '(' right after name: has arguments
        {
                m.nparam = 0;
                i++;
                while (line[i] && line[i] != ')' && m.nparam < 4)
                {
                        while (line[i] == ' ')
                                i++;
                        k = 0;
                        while (is_word(line[i]) && k < 15)
                                m.param[m.nparam][k++] = line[i++];
                        m.nparam++;
                        while (line[i] == ' ')
                                i++;
                        if (line[i] == ',')
                                i++;
                        else
                                break;
                }
                if (line[i] == ')')
                        i++;
        }

        while (line[i] == ' ' || line[i] == '\t')
                i++;
        strncpy(m.body, line + i, sizeof(m.body) - 1);  // body
        m.body[strcspn(m.body, "\n")] = '\0';           // remove the newline

        if (count < MAX)
                table[count++] = m;
}

// copy in to out, replacing macros
static void replace(const char *in, char *out)
{
        int i = 0;
        out[0] = '\0';

        while (in[i])
        {
                char word[64], one[2] = { in[i], '\0' };

                if (!(isalpha(in[i]) || in[i] == '_'))  // not a word: copy as it is
                {
                        add(out, one);
                        i++;
                        continue;
                }

                int k = 0;                              // read a whole word
                while (is_word(in[i]) && k < 63)
                        word[k++] = in[i++];
                word[k] = '\0';

                struct macro *m = find(word);
                if (!m)
                        add(out, word);
                else if (m->nparam < 0)                 // simple macro
                        add(out, m->body);
                else                                    // macro with arguments
                {
                        int j = i, a = 0, n = 0;
                        char arg[4][64];

                        memset(arg, 0, sizeof(arg));
                        while (in[j] == ' ')
                                j++;
                        if (in[j] != '(')               // no '(': not a call
                        {
                                add(out, word);
                                continue;
                        }
                        j++;
                        while (in[j] && in[j] != ')')   // collect arguments
                        {
                                if (in[j] == ',')
                                {
                                        a++;
                                        n = 0;
                                }
                                else if (a < 4 && n < 63)
                                        arg[a][n++] = in[j];
                                j++;
                        }
                        if (in[j] != ')')               // no ')': not a call
                        {
                                add(out, word);
                                continue;
                        }

                        for (int p = 0; m->body[p];)    // put arguments into the body
                        {
                                if (isalpha(m->body[p]) || m->body[p] == '_')
                                {
                                        char w[64];
                                        int wk = 0, found = -1;
                                        while (is_word(m->body[p]) && wk < 63)
                                                w[wk++] = m->body[p++];
                                        w[wk] = '\0';
                                        for (int x = 0; x < m->nparam; x++)
                                                if (strcmp(w, m->param[x]) == 0)
                                                        found = x;
                                        add(out, found >= 0 ? arg[found] : w);
                                }
                                else
                                {
                                        char c[2] = { m->body[p++], '\0' };
                                        add(out, c);
                                }
                        }
                        i = j + 1;                      // continue after ')'
                }
        }
}

void handle_macros(const char *src, const char *des)
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

        char line[1024], result[SIZE];
        int i;

        while (fgets(line, sizeof(line), fp))
        {
                i = 0;
                while (line[i] == ' ' || line[i] == '\t')
                        i++;

                if (strncmp(line + i, "#define", 7) == 0)
                {
                        store_macro(line, i + 7);
                        fputc('\n', out);               // blank line keeps the line count
                }
                else
                {
                        replace(line, result);
                        fputs(result, out);
                }
        }
        fclose(fp);
        fclose(out);
}
