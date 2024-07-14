#include "job-array_struct.h"
#include "../string/string.h"
#include "../string-array/string-array.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

JobArray newJobArray() {
    JobArray array;
    array.size = 0;
    array.maxSize = 10;
    array.jIdCount = 1;
    array.jobs = malloc(sizeof(Job) * array.maxSize);
    return array;
}

void addJob(JobArray *array, Job job) {
    if(array->size >= array->maxSize) {
        array->maxSize *= 2;
        array->jobs = realloc(array->jobs, sizeof(Job) * array->maxSize);
    }
    array->jobs[array->size] = job;
    array->size++;
    array->jIdCount++;
}

Job *getJob(JobArray *array, int jId) {
    for (int i = 0; i < array->size; ++i) {
        if(array->jobs[i].jId == jId) {
            return &array->jobs[i];
        }
    }
    return NULL;
}

bool containsJob(JobArray *array, int pId) {
    for(int i = 0; i < array->size; i++) {
        if(array->jobs[i].pId == pId) {
            return true;
        }
    }
    return false;
}

int deleteJob(JobArray *array, int id, bool deleteByPID) {
    bool exists = false;
    bool checkExistence;
    int result = 0;
    for (int i = 0; i < array->size; ++i) {
        if (deleteByPID) {
            checkExistence = array->jobs[i].pId == id;
        } else {
            checkExistence = array->jobs[i].jId == id;
        }
        if (checkExistence) {
            exists = true;
            Job temp = array->jobs[i];
            result = temp.jId;
            array->jobs[i] = array->jobs[i + 1];
            freeJob(&temp);
            i++;
        }
        if (exists && i < array->size - 1) {
            array->jobs[i] = array->jobs[i + 1];
        }
    }
    if (exists) {
        array->size--;
    }
    return result;
}

void killJob(JobArray *array, int jId) {
    Job *job = getJob(array, jId);
    if(job != NULL) {
        if(job->running) {
            kill(-job->pId, SIGTERM);
        } else {
            kill(-job->pId, SIGCONT);
            kill(-job->pId, SIGTERM);
        }
    }
}

void runInBG(JobArray *array, int jId) {
    Job *job = getJob(array, jId);
    if(job != NULL && !job->running) {
        job->running = true;
        job->fg = false;
        kill(-job->pId, SIGCONT);
    }
}

void runInFG(JobArray *array, int *status, volatile sig_atomic_t *childPID, int jId) {
    Job *job = getJob(array, jId);
    Job resetJob;
    if(job != NULL) {
        resetJob = newJob(job->jId, job->pId, job->running, job->prg, job->fg);
        *childPID = job->pId;
        deleteJob(array, jId, false);
        if(!resetJob.running) {
            kill(-resetJob.pId, SIGCONT);
        }
        waitpid(*childPID, status, 0 | WUNTRACED);
        sigset_t sigchld;
        sigemptyset(&sigchld);
        sigaddset(&sigchld, SIGCHLD);
        if(WIFSTOPPED(*status)) {
            sigprocmask(SIG_BLOCK, &sigchld, NULL);
            resetJob.running = false;
            resetJob.fg = true;
            addJob(array, resetJob);
            sigprocmask(SIG_UNBLOCK, &sigchld, NULL);
        } else {
            if(WIFSIGNALED(*status)) {
                printf("[%d] %d terminated by signal %d\n", resetJob.jId, *childPID, WTERMSIG(*status));
            }
            freeJob(&resetJob);
        }

    }
}

void killAllJobs(JobArray *array) {
    sigset_t sigchld;
    sigemptyset(&sigchld);
    sigaddset(&sigchld, SIGCHLD);
    sigprocmask(SIG_BLOCK, &sigchld, NULL);
    for (int i = 0; i < array->size; ++i) {
        if(array->jobs[i].running) {
            kill(-array->jobs[i].pId, SIGHUP);
        } else {
            kill(-array->jobs[i].pId, SIGCONT);
            kill(-array->jobs[i].pId, SIGHUP);
        }
    }
}

void printJobArray(JobArray *array) {
    for (int i = 0; i < array->size; ++i) {
        printJob(&array->jobs[i]);
    }
}

void freeJobArray(JobArray *array) {
    for (int i = 0; i < array->size; ++i) {
        freeJob(&array->jobs[i]);
    }
    free(array->jobs);
}