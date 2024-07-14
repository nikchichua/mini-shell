#include "job-array_struct.h"
#ifndef JOB_ARRAY_H
#define JOB_ARRAY_H
JobArray newJobArray();
void addJob(JobArray *array, Job job);
bool containsJob(JobArray *array, int pId);
int deleteJob(JobArray *array, int id, bool deleteByPID);
void killJob(JobArray *array, int jId);
void runInBG(JobArray *array, int jId);
void runInFG(JobArray *array, int *status, volatile sig_atomic_t *childPID, int jId);
void killAllJobs(JobArray *array);
void printJobArray(JobArray *array);
void freeJobArray(JobArray *array);
#endif