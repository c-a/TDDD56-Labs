// Simple little unit for timing using the gettimeofday() call.
// By Ingemar 2009

#include <stdlib.h>
#include <sys/time.h>
#include "milli.h"

static struct timeval timeStart;
static char hasStart = 0;

int GetMilliseconds()
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	if (!hasStart)
	{
		hasStart = 1;
		timeStart = tv;
	}
	return (tv.tv_usec - timeStart.tv_usec) / 1000 + (tv.tv_sec - timeStart.tv_sec)*1000;
}

int GetMicroseconds()
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	if (!hasStart)
	{
		hasStart = 1;
		timeStart = tv;
	}
	return (tv.tv_usec - timeStart.tv_usec) + (tv.tv_sec - timeStart.tv_sec)*1000000;
}

double GetSeconds()
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	if (!hasStart)
	{
		hasStart = 1;
		timeStart = tv;
	}
	return (double)(tv.tv_usec - timeStart.tv_usec) / 1000000.0 + (double)(tv.tv_sec - timeStart.tv_sec);
}

// If you want to start from right now.
void ResetMilli()
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	hasStart = 1;
	timeStart = tv;
}

// If you want to start from a specific time.
void SetMilli(int seconds, int microseconds)
{
	hasStart = 1;
	timeStart.tv_sec = seconds;
	timeStart.tv_usec = microseconds;
}
