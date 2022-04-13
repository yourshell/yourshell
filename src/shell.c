/* Name: Shell implemetation */
/* Author: Egor Bronnikov */
/* Last edited: 2-03-2022 */



#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "commands.h"                       /* Builtin functions */



#define SHELL_RL_BUFSIZE    1024            /* Readline buffer size */
#define SHELL_TOK_BUFSIZE   64              /* Token buffer size */
#define SHELL_TOK_DELIM     " \t\r\n\a"     /* Token delimiter */
#define HOSTNAME_MAX        64              /* Hostname max size */



/**
 * Read line from `stdin` and return this value
 * @param void - nothing on the input 
 * @return char* buffer - read string
 */
char *shell_readline(void) {
    int bufsize = SHELL_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        // Read a character
        c = getchar();
        
        // If we hit EOF, replace it with a null character and return
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate
        if (position >= bufsize) {
            bufsize += SHELL_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}



/**
 * Splits the input string into separate tokens (command and arguments)
 * @param char* line - input line
 * @return char** tokens - array of tokens
 */
char **shell_split_line(char *line) {
    int bufsize = SHELL_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SHELL_TOK_DELIM);
    
    // As long as we have tokens available, we write them into the array
    while (token != NULL) {
        tokens[position] = token;
        position++;

        // If we have exceeded the buffer, reallocate
        if (position >= bufsize) {
            bufsize += SHELL_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SHELL_TOK_DELIM);
    }
    
    // End the array of tokens with NULL
    tokens[position] = NULL;

    return tokens;
}



/**
 * Run and execute proccess with all data saved in tokens array
 * @param char** tokens - array of tokens
 * @return int status - this means that shell is running correct (1) or not (0)
 * If not, we are leaving from the loop
 */
int shell_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("shell");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}



/**
 * Select the commands that are built-in and those that are in the system
 * @param char** tokens - array of tokens
 * @see shell_launch(char **args)
 * @return status - command execution status
 */
int shell_execute(char **args) {
    int i;

    if (args[0] == NULL) {
        // An empty command was entered
        return 1;
    }

    // Try all of the builtin commands, and if one is found, execute
    for (i = 0; i < shell_num_builtins(); i++) {
        if(strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return shell_launch(args);
}



/**
 * Main shell loop 
 * @param void - nothing on the input
 * @see shell_readline(void)
 * @see shell_split_line(char *line)
 * @see shell_execute(char **args)
 * @return void - nothing
 */
void shell_loop(void) {
    char *line;
    char **args;
    int status;
    // Get usename
    struct passwd *p = getpwuid(getuid());
    // Get hostname
    char hostname[HOSTNAME_MAX];
    gethostname(hostname, sizeof(hostname));
    
    // Clear screen
    system("clear");
    
    // Before we get out of the shell or encounter some terrible error, then keep reading new lines
    do {
        printf("\033[0;32m%s@%s\033[0;37m:~$ ", p->pw_name, hostname);
        line = shell_readline();
        args = shell_split_line(line);
        status = shell_execute(args);

        free(line);
        free(args);
    } while (status);
}



/* Main function */
int main(int argc, char **argv) {
    // TODO: Load config files here

    // Run commands loop
    shell_loop();
    
    // Shutdown and cleanup
    return EXIT_SUCCESS;
}
