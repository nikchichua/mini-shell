#include "../job/job.h"
#ifndef JOB_ARRAY_STRUCT_H
#define JOB_ARRAY_STRUCT_H
typedef struct JobArray {
    int size;
    int maxSize;
    int jIdCount;
    Job *jobs;
} JobArray;
#endif