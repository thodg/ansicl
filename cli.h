#ifndef CLI_H
#define CLI_H

#include "form.h"

#define CLI_SIZE 1000

typedef struct cli
{
  const char *prompt;
  char line[CLI_SIZE];
} s_cli;

void     cli_init (s_cli *cli);
void     cli_prompt (s_cli *cli, const char *prompt);
u_form * cli_readline (s_cli *cli);

#endif
