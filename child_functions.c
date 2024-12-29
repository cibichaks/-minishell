#include "main.h"
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Name  : CIBISAKKARAVARTHI K
Batch : 24012A
file name : child_functions
Description : description about functions and all refer to main.h file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

llist *head = NULL;
extern char child_pid, child_status;
extern char *input_string;
extern char *external_commands;
void execute_external_commands(char *input_string)
{
    char pipe_or_not = 0;
    if (strstr(input_string, "|") != NULL)
    {
        pipe_or_not = 1;
    }
    if (pipe_or_not)
    {
        n_pipe(input_string);
    }
    else
    {
        char str_for_strtok[100];
        strcpy(str_for_strtok, input_string);
        char *tokens, *cmd_args[20], itr = 1;
        cmd_args[0] = malloc(sizeof(char) * CMD_SIZE);
        cmd_args[0] = tokens = strtok(str_for_strtok, " ");
        while ((tokens = strtok(NULL, " ")) != NULL)
        {
            cmd_args[itr] = malloc(sizeof(char) * CMD_SIZE);
            strcpy(cmd_args[itr++], tokens);
        }
        cmd_args[itr] = NULL;

        if (execvp(cmd_args[0], cmd_args + 0) == -1)
        {
            perror("execvp failed");
        }
    }
}

void child_handler(int signum)
{
    if (add_to_list(&head) == SUCCESS)
    {
        printf("\n[1] + %d suspended %s\n", child_pid, input_string);
    }
}

int add_to_list(llist **head)
{

    llist *newnode = malloc(sizeof(llist));
    if (newnode == NULL)
    {
        return FAIL;
    }
    newnode->pid = child_pid;

    strcpy((newnode->cmd), input_string);

    newnode->link = NULL;
    if (*head == NULL)
    {
        *head = newnode;
    }
    else
    {
        llist *temp = *head;
        while (temp->link != NULL)
        {
            temp = temp->link;
        }
        temp->link = newnode;
    }
    return SUCCESS;
}

void extract_external_commands(char **external_commands)
{
    FILE *fd = fopen("external.txt", "r");
    if (fd == NULL)
    {
        printf("ERROR : while openinng the builtin cmd file");
    }
    int i = 0;
    while (fscanf(fd, "%s\n", external_commands[i++]) != EOF)
    {
        external_commands[i] = malloc(sizeof(char) * COL_SIZE);
    }
}

void n_pipe(char *cmd_string)
{
    char *argv[30];
    int argc = 0;

    char *token = strtok(cmd_string, " ");
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    argv[argc] = NULL;

    int pipe_fd[2 * (10 - 1)];
    pid_t pids[10];
    int cmd_count = 0;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            cmd_count++;
        }
    }
    cmd_count++;

    for (int i = 0; i < cmd_count - 1; i++)
    {
        if (pipe(pipe_fd + i * 2) == -1)
        {
            printf("error in fork");
            exit(EXIT_FAILURE);
        }
    }

    int cmd_start = 0;
    for (int i = 0; i < cmd_count; i++)
    {
        char *cmd_args[10];
        int j = 0;
        while (argv[cmd_start] != NULL && strcmp(argv[cmd_start], "|") != 0)
        {
            cmd_args[j++] = argv[cmd_start++];
        }
        cmd_args[j] = NULL;
        cmd_start++;

        if ((pids[i] = fork()) == -1)
        {
            printf("ERROR : eror in fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0)
        {
            if (i > 0)
            {
                dup2(pipe_fd[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < cmd_count - 1)
            {
                dup2(pipe_fd[i * 2 + 1], STDOUT_FILENO);
            }
            for (int k = 0; k < 2 * (cmd_count - 1); k++)
            {
                close(pipe_fd[k]);
            }
            if (execvp(cmd_args[0], cmd_args) == -1)
            {
                printf("ERROE : exevp");
                exit(EXIT_FAILURE);
            }
        }
    }

    for (int i = 0; i < 2 * (cmd_count - 1); i++)
    {
        close(pipe_fd[i]);
    }

    for (int i = 0; i < cmd_count; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
}