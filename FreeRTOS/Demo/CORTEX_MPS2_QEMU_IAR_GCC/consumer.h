#ifndef _CONSUMER_H_
#define _CONSUMER_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Rate in bytes per tick
#define CONSUMER_RATE   50000
#define CONSUMER_BUFLEN 100000

int sendAtRate(void * data, int length);
#endif