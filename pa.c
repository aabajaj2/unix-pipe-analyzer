#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
    pid_t id;
    int fds[2], fds1[2], fds2[2];
    int c = 0, j;
    char buf[32];
    int count = 0;

    pipe(fds);
    pipe(fds1);
    pipe(fds2); 

    /*parsing the arguments*/

    char *aargv[argc][argc];
    int row = 0; 
    int column = 0;   
    for(c = 1; c<argc; c++){
        column = 0;
        while (strcmp(argv[c], "|") != 0){
            aargv[row][column] = (char *) malloc (sizeof(char) * 40);
            aargv[row][column] = argv[c];
            c++;
            column++;

            if(c == argc){
                break;
            }      
        }
        aargv[row][column] = '\0';
        row++;
    }
    printf("%s %s %s %s\n", aargv[0][0], aargv[0][1], aargv[1][0], aargv[1][1]);
    
    /* Fork first process */
    id = fork();

    if (id < 0) {
        printf("fork() for first process failed\n");
        exit(-1);
    }

    if (id == 0) {
        /* Close read end of pipe */
        close(fds[0]);

        /* Close stdout */
        close(1);

        /* Dup write end of pipe */
        dup(fds[1]);

        /* Close extra write end of pipe */
        close(fds[1]);

        if (execvp(aargv[0][0], aargv[0]) < 0) {
            write(2, "execvp() failed for prog1\n", 27);
            exit(-1);
        }

        /* We will never reach this point */
    }

    /* Fork second process */

    id = fork();

    if (id < 0) {
        printf("fork() for second process failed\n");
        exit(-1);
    }

    if (id == 0) {
        /* Close write end of pipe */
        close(fds[1]);

        /* Close stdin */
        close(0);

        /* Dup read end of pipe */
        dup(fds[0]);

        /* Close extra read end of pipe */
        close(fds[0]);

        if (execvp(aargv[1][0], aargv[1]) < 0) {
            write(2, "execvp() failed for prog2\n", 27);
            exit(-1);
        }
    }

    /* Need to close both ends of pipe in parent */
    close(fds[0]);
    close(fds[1]);
    
    id = wait(NULL);
    id = wait(NULL);
    
    return 0;
}