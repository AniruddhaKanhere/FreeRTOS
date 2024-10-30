#ifndef _CONSUMER_H_
#define _CONSUMER_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RATE  100
#define BUFLEN 1000

int sendAtRate(void * data, int length);
#endif