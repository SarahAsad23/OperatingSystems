/*
* Sarah Asad
* CSS 430
* Program 1C: Pipes
*/

#include <stdlib.h>   //for exit
#include <stdio.h>    //for perror
#include <unistd.h>   //for fork and pipe
#include <sys/wait.h> //for wait
#include <iostream>   //for cerr and cout

using namespace std;

//ps -A | grep argv[1] | wc - l

int main(int argc, char** argv) {

    enum { READ, WRITE }; //pipe file descriptors: READ = 0 and WRITE = 1
    pid_t pidOne, pidTwo, pidThree; //process id's 
    int pipeFdsOne[2], pipeFdsTwo[2]; //pipe file descriptors set to size 2
    
    if (argc != 2) {
        perror("Too few arguments");
        exit(EXIT_FAILURE);
    }

    pidOne = fork(); //for first time to creat child process

    if (pidOne < 0) { //error checking 
        perror("Fork Error");
        exit(EXIT_FAILURE);
    }
    else if (pidOne == 0) {  
        int success;
        success = pipe(pipeFdsOne); //create first pipe this is between child and grandchild
        pidTwo = fork(); //fork second time to create grand child 

        if (success < 0) { //error checking to see if pipe created successsfully 
            perror("Pipe Error");
            exit(EXIT_FAILURE);
        }
        if (pidTwo < 0) { //fork error checkng
            perror("Fork Error");
            exit(EXIT_FAILURE);
        }
        else if (pidTwo == 0) { 
            int success;
            success = pipe(pipeFdsTwo); //create secind pipe. between grandchild an great grandchild
            pidThree = fork(); //fork for the thrid time to create great grand child process
            if (success < 0) { //fork error checking 
                perror("Pipe Error");
                exit(EXIT_FAILURE);
            }
            if (pidThree < 0) { // fork error checking
                perror("Fork Error");
                exit(EXIT_FAILURE);
            }
            else if (pidThree == 0) { //GREATGRANDCHILD
                close(pipeFdsTwo[READ]);
                //greatgrand child writes to the write end of the second pipe
                dup2(pipeFdsTwo[WRITE], 1);
                //close open pipe before we exec
                close(pipeFdsTwo[WRITE]);
                //execute command and check for error
                int execSuccess = execlp("ps", "ps", "-A", 0);
                if (execSuccess < 0) {
                    perror("Error Executing");
                    exit(EXIT_FAILURE);
                }
            }
            else { //GRANDCHILD
                wait(NULL); //grandchild waiting for great grandchild
                //close unused pipes 
                close(pipeFdsOne[READ]);
                close(pipeFdsTwo[WRITE]);
                //granchild reads from read end of pipe two 
                dup2(pipeFdsTwo[READ], 0);
                //grandchild writes to write end of pipe one 
                dup2(pipeFdsOne[WRITE], 1);
                //close used pipes before we exec 
                close(pipeFdsTwo[READ]);
                close(pipeFdsOne[WRITE]);
                //execute command and check for error
                int execSuccess = execlp("grep", "grep", argv[1], 0);
                if (execSuccess < 0) {
                    perror("Error Executing");
                    exit(EXIT_FAILURE);
                }
            }
        }
        else { //CHILD
            wait(NULL); //child waiting for grandchilsd
            //close unused pipe end
            close(pipeFdsOne[WRITE]);
            //child reads from read end of pipe one 
            dup2(pipeFdsOne[READ], 0); 
            //close used pipe before we exec 
            close(pipeFdsOne[READ]);
            //execute command and check for error 
            int execSuccess = execlp("wc", "wc", "-l", 0);
            if (execSuccess < 0) {
                perror("Error Executing");
                exit(EXIT_FAILURE);
            }
        }
    }
    else {
        wait(NULL); //parent waits for children to complete             
    }
    exit(EXIT_SUCCESS); //successfully completed!
}