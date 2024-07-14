#include "job_struct.h"

#ifndef JOB_H
#define JOB_H
Job newJob(int jId, int pId, bool running, char *prg, bool fg);
void printJob(Job *job);
void freeJob(Job *job);
#endif