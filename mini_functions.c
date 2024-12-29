#include "main.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Name  : CIBISAKKARAVARTHI K
Batch : 24012A
file name : mini_functions
Description : description about the functions and all refer to main.h file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern llist *head;
char *input_string;
int child_status, child_pid;
char *external_commands[200];
char prom[100] = "Minishell";
char *builtins[] = {"echo", "printf", "read", "cd",
                    "pwd", "pushd", "popd",
                    "dirs", "let", "eval", "set",
                    "unset", "export", "declare", "typeset",
                    "readonly", "getopts", "source",
                    "exit", "exec", "shopt", "caller", "true",
                    "type", "hash", "bind", "help", NULL};

int main()
{
    system("clear");
    input_string = malloc(1 * 100);
    scan_input(input_string);
}
void print_prompt(void)
{
    char dir_buffer[100];
    printf(ANSI_COLOR_RED "\n%s>>", prom);
    getcwd(dir_buffer, 100);
    printf(ANSI_COLOR_CYAN "%s", (dir_buffer + 11));
    printf(ANSI_COLOR_RESET "$ ");
    fflush(stdout);
}
void mini_handler(int signum)
{
    if ((signum == SIGINT))
    {
        print_prompt();
    }
    else if (signum == SIGTSTP)
    {
        print_prompt();
    }
}
void list_jobs(llist **head)
{
    if (*head == NULL)
    {
        printf("got null in list\n");
        return;
    }
    if ((*head)->link == NULL)
    {

        printf(" [1] %d Suspended %s\n", (*head)->pid, (*head)->cmd);
        return;
    }
    llist *temp = *head;
    int itr = 1;
    while (temp->link)
    {
        printf(" [%d] %d Suspended %s\n", itr++, temp->pid, temp->cmd);
        temp = temp->link;
    }
    printf(" [%d] %d Suspended %s\n", itr++, temp->pid, temp->cmd);
}
void do_fg(llist **head)
{
    if (*head == NULL)
    {
        printf("there is no stoped process\n");
        return;
    }
    if ((*head)->link == NULL)
    {
        printf("[1] %d continued in fore ground %s", (*head)->pid, (*head)->cmd);
        fflush(stdout);
        int fg_child = fork();
        if (fg_child == 0)
        {
            execute_external_commands((*head)->cmd);
        }
        else if (fg_child > 0)
        {
            waitpid(fg_child, NULL, WUNTRACED);
            free((*head));
            (*head) = NULL;
            return;
        }
    }

    llist *temp = *head;
    while (temp->link->link)
    {
        temp = temp->link;
    }
    printf("[1] %d continued in fore ground %s", temp->link->pid, temp->link->cmd);
    int fg_child = fork();
    if (fg_child == 0)
    {
        execute_external_commands(temp->link->cmd);
    }
    else if (fg_child > 0)
    {
        waitpid(fg_child, NULL, WUNTRACED);
        free(temp->link);
        temp->link == NULL;
    }
}
void do_bg(llist **head)
{
    if (*head == NULL)
    {
        printf("there is no stoped process\n");
        return;
    }
    if ((*head)->link == NULL)
    {
        printf("[1] %d Continued %s\n", (*head)->pid, (*head)->cmd);
        int pid = fork();
        if (pid == 0)
        {
            execute_external_commands((*head)->cmd);
        }
        else if (pid > 0)
        {
            free(*head);
            *head = NULL;
            return;
        }
    }
    llist *temp = *head;
    while (temp->link->link)
    {
        temp = temp->link;
    }
    printf("[1] %d continued %s\n", temp->link->pid, temp->link->cmd);
    fflush(stdout);
    free(temp->link);
    temp->link = NULL;
}
void scan_input(char *input_string)
{
    external_commands[0] = malloc(sizeof(char) * COL_SIZE);
    extract_external_commands(external_commands);
    signal(SIGINT, mini_handler);
    signal(SIGTSTP, mini_handler);
    while (1)
    {
        input_string[0] = '\0';
        print_prompt();
        scanf("%[^\n]", input_string);
        getchar();
        if (input_string[0] == 'P')
        {
            char *ptr;
            if ((ptr = strstr(input_string, "PS1=")) != NULL)
            {
                if (*(ptr + 4) != ' ')
                {
                    strcpy(prom, (ptr + 4));
                    continue;
                }
            }
        }
        char *str_for_cut = malloc(1 * 100);
        strcpy(str_for_cut, input_string);
        char *cmd = strtok(str_for_cut, " ");
        if (cmd == NULL)
        {
            continue;
        }
        int cmd_type_macro = check_command_type(cmd);
        if (cmd_type_macro == EXTERNAL)
        {
            child_pid = fork();
            signal(SIGTSTP, child_handler);
            if (child_pid == 0)
            {
                signal(SIGINT, SIG_DFL);
                execute_external_commands(input_string);
            }
            else if (child_pid > 0)
            {
                waitpid(child_pid, &child_status, WUNTRACED);
                signal(SIGTSTP, mini_handler);
            }
        }
        else if (cmd_type_macro == BUILTIN)
        {
            execute_internal_commands(cmd, input_string);
        }
        else
        {
            if (strcmp(cmd, "jobs") == 0)
            {
                list_jobs(&head);
            }
            else if (strcmp(cmd, "bg") == 0)
            {
                do_bg(&head);
            }
            else if (strcmp(cmd, "fg") == 0)
            {
                do_fg(&head);
            }
            else
            {
                printf("%s: command not found : %s\n", "chaks", cmd);
            }
        }
    }
}

char *get_command(char *input_string)
{
    char *first_word = malloc(sizeof(char) * CMD_SIZE);
    char delim = ' ';
    first_word = strtok(input_string, &delim);
    return first_word;
}

int check_command_type(char *command)
{
    int itr = 0;
    while (builtins[itr] != NULL)
    {
        if (strcmp(command, builtins[itr++]) == 0)
        {
            return BUILTIN;
        }
    }
    itr = 0;
    while (external_commands[itr] != NULL)
    {
        if (strcmp(command, external_commands[itr++]) == 0)
        {
            return EXTERNAL;
        }
    }
    return NO_COMMAND;
}

void execute_internal_commands(char *cmd, char *input_string)
{
    char *ptr;
    if (strcmp(cmd, "exit") == 0)
    {
        kill(getpid(), SIGSTOP);
    }
    else if (strcmp(cmd, "pwd") == 0)
    {
        char buffer[100];
        getcwd(buffer, 100);
        printf("%s\n", buffer);
    }
    else if (strcmp(cmd, "cd") == 0)
    {
        char path[100];
        strtok(input_string, " ");
        char *str = strtok(NULL, " ");
        if (str == NULL)
        {
            chdir("/home/chaks");
            return;
        }
        strcpy(path, ptr);
        if (chdir(path) != 0)
        {
            perror("cd");
        }
    }
    else if ((ptr = strstr(cmd, "echo")) != NULL)
    {

        if (*(input_string + 4) == ' ')
        {
            if (strcmp(input_string, "echo $$") == 0)
            {

                printf("process id is %d\n", getpid());
            }
            else if (strcmp(input_string, "echo $?") == 0)
            {

                WEXITSTATUS(child_status);
                printf("last child exit code is %d\n", child_status);
            }
            else if (strcmp(input_string, "echo $SHELL") == 0)
            {

                ptr = getenv("SHELL");
                printf("%s\n", ptr);
            }
        }
    }
}
