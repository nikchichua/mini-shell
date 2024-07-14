#include "job_struct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Job newJob(int jId, int pId, bool running, char *prg, bool fg) {
    Job job;
    job.jId = jId;
    job.pId = pId;
    job.running = running;
    job.prg = malloc(sizeof(char) * (strlen(prg) + 1));
    strcpy(job.prg, prg);
    job.fg = fg;
    return job;
}

void printJob(Job *job) {
    printf("[%d] %d %s %s %s",
           job->jId,
           job->pId,
           job->running ? "Running" : "Stopped",
           job->prg,
           job->fg ? "\n" : "&\n"
    );
}

void freeJob(Job *job) {
    free(job->prg);
    job = NULL;
}
