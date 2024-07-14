#include <stdbool.h>

#ifndef JOB_STRUCT_H
#define JOB_STRUCT_H
typedef struct Job {
    int jId;
    int pId;
    bool running;
    char *prg;
    bool fg;
} Job;
#endif