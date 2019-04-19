
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cli.h"

s_cli g_cli;

void cli_init (s_cli *cli)
{
        if (isatty(0)) {
                cli->prompt = "cfacts> ";
                using_history();
        }
        else
                cli->prompt = 0;
}

void cli_prompt (s_cli *cli, const char *prompt)
{
        if (isatty(0))
                cli->prompt = prompt;
        else
                cli->prompt = 0;
}

static void trim_newline (char *str)
{
        unsigned int i = 0;
        while (str[i])
                i++;
        if (i > 0 && str[i - 1] == '\n')
                str[i - 1] = 0;
}

u_form * cli_read_file_line (s_cli *cli, FILE *fp)
{
        if (fgets(cli->line, CLI_SIZE, fp)) {
                trim_newline(cli->line);
                puts(cli->line);
                return new_string(strlen(cli->line), cli->line);
        }
        return NULL;
}

u_form * cli_readline (s_cli *cli)
{
        if (cli->prompt) {
                char *line = readline(cli->prompt);
                u_form *s;
                if (line == 0)
                        return NULL;
                s = new_string(strlen(line), line);
                add_history(line);
                return s;
        }
        return cli_read_file_line(cli, stdin);
}
