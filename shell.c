#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "string/string.h"
#include "string-array/string-array.h"
#include "job/job.h"
#include "job-array/job-array.h"

sig_atomic_t status;
volatile sig_atomic_t childPID;
JobArray jobs;

__sighandler_t *Signal(int signal, __sighandler_t handler) {
    struct sigaction action, old_action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if (sigaction(signal, &action, &old_action) < 0) {
        perror("Signal error");
    }
    return (void (**)(int)) (old_action.sa_handler);
}

void SIGNAL_HANDLER(int signal) {
    switch (signal) {
        case SIGINT: {
            if(childPID != 0 && !containsJob(&jobs, childPID)) {
                kill(-childPID, SIGINT);
            }
            break;
        }
        case SIGTSTP: {
            if(childPID != 0 && !containsJob(&jobs, childPID)) {
                childPID = kill(-childPID, SIGTSTP);
            }
            break;
        }
        case SIGCHLD: {
            sigset_t sigchld;
            sigemptyset(&sigchld);
            sigaddset(&sigchld, SIGCHLD);
            int pid = childPID;
            int jid = jobs.jIdCount;
            while ((pid = waitpid(-1, &status, 0 | WNOHANG)) > 0) {
                sigprocmask(SIG_BLOCK, &sigchld, NULL);
                jid = deleteJob(&jobs, pid, true);
                if(WIFSIGNALED(status) && jid != 0) {
                    printf("[%d] %d terminated by signal %d\n", jid, pid, WTERMSIG(status));
                    status = 0;
                }
                sigprocmask(SIG_UNBLOCK, &sigchld, NULL);
            }
            break;
        }
        default: break;
    }
}

void executeCommand(StringArray command) {
    char **argv = getStrings(&command);
    if(isDirectory(&command) && execv(argv[0], argv) == -1) {
        perror(argv[0]);
    } else {
        char *original = malloc(sizeof(char) * (strlen(argv[0]) + 1));
        strcpy(original, argv[0]);
        prependToFirst(&command, "/usr/bin/");
        if(execv(argv[0], argv) == -1) {
            replaceFirst(&command, original);
            prependToFirst(&command, "/bin/");
            if(execv(argv[0], argv) == -1) {
                printf("%s: command not found\n", original);
            }
        }
        free(original);
    }
    exit(0);
}

void handleProcess(StringArray command, bool bg) {
    if((childPID = fork()) == 0) {
        setpgid(0, 0);
        executeCommand(command);
    } else {
        sigset_t sigchld;
        sigemptyset(&sigchld);
        sigaddset(&sigchld, SIGCHLD);
        if(bg) {
            sigprocmask(SIG_BLOCK, &sigchld, NULL);
            String programPath = toString(&command);
            addJob(&jobs, newJob(jobs.jIdCount, childPID, true, programPath.characters, false));
            freeString(&programPath);
            printf("[%d] %d\n", jobs.jIdCount - 1, childPID);
        } else {
            int pid = waitpid(childPID, &status, 0 | WUNTRACED);
            sigprocmask(SIG_BLOCK, &sigchld, NULL);
            if(WIFSTOPPED(status)) {
                String programPath = toString(&command);
                addJob(&jobs, newJob(jobs.jIdCount, pid, false, programPath.characters, true));
                freeString(&programPath);
            }
            if(WIFSIGNALED(status)) {
                printf("[%d] %d terminated by signal %d\n", jobs.jIdCount, childPID, WTERMSIG(status));
            }
        }
        sigprocmask(SIG_UNBLOCK, &sigchld, NULL);
    }
}

void runShell() {
    jobs = newJobArray();
    while(true) {
        bool exit = false;
        usleep(10000);
        printf("> ");
        String input = newString();
        int character;
        while((character = getc(stdin)) != '\n') {
            if(character == EOF) {
                freeString(&input);
                goto end;
            }
            addChar(&input, (char) character);
        }
        StringArray command = getCommand(&input);
        int size = command.size;
        char *arg1 = command.strings[0];
        char *arg2 = command.strings[1];
        if(size == 1) {
            if(strcmp(arg1, "exit") == 0) {
                exit = true;
            } else if(strcmp(arg1, "jobs") == 0) {
                printJobArray(&jobs);
            } else if(strcmp(arg1, "cd") == 0) {
                chdir(getenv("HOME"));
            } else {
                goto execute;
            }
            goto clean_up;
        } else if(size == 2) {
            if(strcmp(arg1, "cd") == 0) {
                chdir(arg2);
                goto clean_up;
            } else if(strlen(arg2) >= 2 && arg2[0] == '%') {
                for (int i = 1; i < strlen(arg2); ++i) {
                    if(!isdigit(arg2[i])) goto execute;
                }
                arg2[0] = '+';
                int jId = (int) strtol(arg2, NULL, 10);
                if(strcmp(arg1, "kill") == 0) {
                    killJob(&jobs, jId);
                } else if(strcmp(arg1, "bg") == 0) {
                    runInBG(&jobs, jId);
                } else if(strcmp(arg1, "fg") == 0) {
                    runInFG(&jobs, &status, &childPID, jId);
                } else {
                    goto execute;
                }
                goto clean_up;
            }
        }
        execute:;
        bool bg = false;
        if(command.size > 1 && strcmp(command.strings[command.size - 1], "&") == 0) {
            removeLastString(&command);
            bg = true;
        }
        if(command.size > 0) handleProcess(command, bg);
        clean_up:
        freeStringArray(&command);
        freeString(&input);
        if(exit) break;
    }
    end:;
    killAllJobs(&jobs);
    freeJobArray(&jobs);
    printf("\n");
}

int main() {
    Signal(SIGINT, SIGNAL_HANDLER);
    Signal(SIGTSTP, SIGNAL_HANDLER);
    Signal(SIGCHLD, SIGNAL_HANDLER);
    runShell();
    return 0;
}