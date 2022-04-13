/**
 * COMMANDS
 *
 * This file contains all the builtin functions, if you want you can easily add them simply by writing them here
 * To do this, you need to add them to the list of `builtint_str` and to the list of functions `buildin_func`
 * Of course, you need to add the implementation of the corresponding function
 * */



#ifndef _CTYPE_H
#include <ctype.h>
#endif


#ifndef _STDLIB_H
#include <stdlib.h>
#endif


#ifndef _UNISTD_H
#include <unistd.h>
#endif


#ifndef _STRING_H
#include <string.h>
#endif



/* Function Declarations for builtin shell commands */
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
int shell_plus(char **args);



/* List of buildtin commands, followed by their corresponding functions */
char *builtin_str[] = {
    "cd",
    "help",
    "plus",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &shell_cd,
    &shell_help,
    &shell_plus,
    &shell_exit
};



/**
 * The number of builtin functions
 * @param void - nothing on the input
 * @return int - number of builtin functions
 * */
int shell_num_builtins(void) {
    return sizeof(builtin_str) / sizeof(char *);
}



/* Builtin function implementation */


/**
 * Change directory (cd) command
 * @param char** args - arguments for the command
 * @return 1 - this means that command is fine and running without terminating
 * */
int shell_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("shell");
        }
    }
    return 1;
}



/**
 * Help, print docs for all available commands
 * @param char** args - arguments for the command
 * @return 1 - this means that command is fine and running without terminating
 * */
int shell_help(char **args) {
    int i;
    printf("SHELL\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < shell_num_builtins(); i++) {
        printf("    %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}



/**
 * Checks if the input string is a number
 * @param char* arg - input string 
 * @return int - if the input string is a number returns 1 else 0
 * */
int isnumber(char *arg) {
    int len, i;
    len = strlen(arg);
    for (i = 0; i < len; i++) {
        if (!isdigit(arg[i])) {
            return 0;
        }
    }
    return 1;
}



/**
 * Plus command, adds up two numbers
 * @param char** args - arguments for the command
 * @return 1 - this means that command is fine and running without terminating
 * */
int shell_plus(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "shell: expected arguments to \"plus\"\n");
    } else {
        if (isnumber(args[1]) && isnumber(args[2])) {
            printf("%d\n", atoi(args[1]) + atoi(args[2]));
        } else {
            fprintf(stderr, "shell: unacceptable arguments for \"plus\"\n");
        }
    }
    return 1;
}



/**
 * Exit from the shell
 * @param char** args - arguments for the command
 * @return 0 - this means that we are leaving from the shell
 * */
int shell_exit(char **args) {
    return 0;
}
