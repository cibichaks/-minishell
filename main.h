#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define BUILTIN 1
#define EXTERNAL 2
#define NO_COMMAND 3
#define COL_SIZE 60
#define CMD_SIZE 10
#define B_CMD_CNT 28
#define E_CMD_CNT 153
#define SUCCESS 0
#define FAIL 1

// coloures that used to varing the shell and input
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

// Structure of an linked list that used to store Stoped process that stoped by a SIGTSTP signal
typedef struct list
{
    char *cmd[100];
    int pid;
    struct list *link;
} llist;

// mini_function file prototypes
void print_prompt(void); // to print the minishell name and current working directory with colours

void mini_handler(int signum); // to handle the SIGINT and SIGTSTP signals without command state

void list_jobs(llist **head); // to see the process that stoped using the SIGTSTP signal

void do_fg(llist **head); // Continue the process with current process that stoped by the SIGTSTP

void do_bg(llist **head); // Continue the process in background that stoped by the SIGTSTP

void scan_input(char *input_string); // this function used to scan the cmd and print the prompt msgs when one command executed this rinx in loop

char *get_command(char *input_string); // to extract the first word of the command

int check_command_type(char *command); // checking the cmd that Internal or External

void execute_internal_commands(char *cmd, char *input_string); // this funtion will execute the Internal cmds

// child_function file prototypes
void execute_external_commands(char *input_string); // this function used to execute the external commands

void child_handler(int signum); // handle the SIGTSTP signal to stop the current process

int add_to_list(llist **head); // this function will add the stoped process to globally initilised linked list

void extract_external_commands(char **external_commands); // this s used to extract the external cmds in an external.txt file

void n_pipe(char *cmd_string); // this function execute the command that precent in pipe

#endif
