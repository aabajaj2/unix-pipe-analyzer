#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

struct metrics{
    int lines;
    int bytes;
    int type;
    char *progname;
};

int main(int argc, char **argv)
{
    pid_t id;
    int fds[2], fds1[2];
    int c = 0, j;
    char buf[32];
    char intr[1000] = "";
    int count = 0;
    struct metrics m;

    pipe(fds);
    pipe(fds1);

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

        /* Dup write end of pipe it also closes the stdout */
        dup2(fds[1], 1);

        /* Close extra write end of pipe */
        close(fds[1]);

        if (execvp(aargv[0][0], aargv[0]) < 0) {
            write(2, "execvp() failed for prog1\n", 27);
            exit(-1);
        }
        /* We will never reach this point */
    }

    /* Fork middle process */

    id = fork();

    if (id < 0) {
        printf("fork() for middle process failed\n");
        exit(-1);
    }

    if (id == 0) {
        /* Close write end of pipe */
        close(fds[1]);
        close(fds1[0]);
        while ( (count = read(fds[0], buf, 1)) > 0 ) {
            write(fds1[1], buf, 1);
            strcat(intr, buf);
            if(strcmp(buf, "\n")){
                m.lines++;
            }
        }
        close(fds1[1]);
        close(fds[0]);
        printf("Intermediate = %s\n", intr);     
        exit(0);
    }

    /* Fork second process */

    id = fork();

    if (id < 0) {
        printf("fork() for second process failed\n");
        exit(-1);
    }

    if (id == 0) {
        /* Close write end of pipe */
        close(fds1[1]);

        /* Dup read end of pipe, dup2 closed the stdin too*/
        dup2(fds1[0], 0);

        /* Close extra read end of pipe */
        close(fds1[0]);
        close(fds[0]);
        close(fds[1]);

        if (execvp(aargv[1][0], aargv[1]) < 0) {
            write(2, "execvp() failed for prog2\n", 27);
            exit(-1);
        }
    }

    /* Need to close both ends of pipe in parent */
    close(fds[0]);
    close(fds[1]);
    close(fds1[0]);
    close(fds1[1]);
    
    id = wait(NULL);
    id = wait(NULL);
    id = wait(NULL);
    
    return 0;
}