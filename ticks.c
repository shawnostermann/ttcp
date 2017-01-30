/* 
 * ticks.c - draw pretty tick marks
 * 
 * Author:	Shawn Ostermann
 * 		Dept. of Computer Sciences
 * 		Purdue University
 * Date:	Sun Feb 21 21:33:27 1993
 *
 * Copyright (c) 1993 Shawn Ostermann
 */


#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include "timeval.h"

extern int speed;
char *outfmt(double b);



float pertick;
int currpos;
int count;

#define DEFAULT_LINELENGTH 65
int linelength = DEFAULT_LINELENGTH;

void
inittick(int max)
{
/* 	int i; */

	currpos = 0.0;
	count = 0;

	if (speed) linelength -= 10;/* make room for speed digits */
	pertick = (float) max / (float)linelength;

#ifdef OLD
	fprintf(stderr,"|");
	for (i=0; i < LINELENGTH; ++i) fprintf(stderr,"-");
	fprintf(stderr,"|");
	fprintf(stderr,"%c|", '\015');
	fflush(stderr);
#endif
}



#ifdef SLOW_VERSION
/*return "recent" throughput in BYTES/second */
static float
calc_tput(
    int bytes)
{
#define GOBACK_MS 2000
    struct sample {
	struct timeval t;
	unsigned nbytes;
	struct sample *next;
    };
    static struct sample *samples = NULL;
    struct sample *ps, *plast;
    unsigned ttl = 0;
    u_long etime_ms = 0;

    /* make a new sample */
    ps = malloc(sizeof(struct sample));
    ps->next = NULL;
    gettimeofday(&ps->t,NULL);
    ps->nbytes = bytes;

    /* chain it in */
    ps->next = samples;
    samples = ps;
       
    /* go back GOBACK mseconds */
    plast = NULL;
    for (ps=samples; ps; ) {
	unsigned et = tv_ago_msecs(ps->t);
	if (et <= GOBACK_MS) {
	    ttl += ps->nbytes;
	    etime_ms = et;
	    plast = ps;
	    ps = ps->next;
	} else {
	    struct sample *pn = ps->next;
	    if (plast) {
		plast->next = NULL; /* bisect the list */
		plast = NULL;
	    }
	    free(ps);
	    if (ps == samples)
		samples = NULL; /* whole list is empty */
	    ps = pn;
	}
    }

    /* if no elapsed time, no work done */
    if (etime_ms == 0) {
	return(0.0);
    }

    return(((float) ttl)/ ((float)etime_ms / 1000));
}
#endif


/*return "recent" throughput in BYTES/second */
/*
 * modified for faster networks and slower hosts to be less compute intensive
 * 
 */
static float
calc_tput(
    int bytes)
{
#define GOBACK_MS 2000
#define BUCKET_SIZE 100 /* in Milliseconds */
    struct sample {
	struct timeval t;
	unsigned nbytes;
	struct sample *next;
    };
    static struct sample *samples = NULL;
    struct timeval time_now;
    struct sample *ps, *plast;
    unsigned ttl = 0;
    u_long etime_ms = 0;

    /* check current time, then drop digits to 100MS resolution */
    gettimeofday(&time_now,NULL);
    int resolution = 1000000 / BUCKET_SIZE;
    time_now.tv_usec /= resolution;
    time_now.tv_usec *= resolution;

    /* if new sample goes in the same bucket, just add it in, otherwise make a new one */
    /* this saves a lot of work on really fast networks and the slight cost of granularity accuracy */
    if (samples && (tv_cmp(time_now,samples->t)==0)) {
	/* add to first sample */
	ps = samples;
	ps->nbytes += bytes;

    } else {
	/* make a new sample */
	ps = malloc(sizeof(struct sample));
	ps->next = NULL;
	ps->t = time_now;
	ps->nbytes = bytes;

	/* chain it in */
	ps->next = samples;
	samples = ps;
    }
       
    /* go back GOBACK mseconds */
    plast = NULL;
    for (ps=samples; ps; ) {
	unsigned et = tv_ago_msecs(ps->t);
	if (et <= GOBACK_MS) {
	    ttl += ps->nbytes;
	    etime_ms = et;
	    plast = ps;
	    ps = ps->next;
	} else {
	    struct sample *pn = ps->next;
	    if (plast) {
		plast->next = NULL; /* bisect the list */
		plast = NULL;
	    }
	    free(ps);
	    if (ps == samples)
		samples = NULL; /* whole list is empty */
	    ps = pn;
	}
    }

    /* if no elapsed time, no work done */
    if (etime_ms == 0) {
	return(0.0);
    }

    return(((float) ttl)/ ((float)etime_ms / 1000));
}




void
drawtick(int len, int nbytes)
{
	int i;
	int newpos;
	double tput = 0;

	count += len;

	newpos = (int) ((float) count / pertick);
	if (speed) 
	    tput = calc_tput(nbytes);
	if (newpos != currpos) {
	    if (speed) {
		fprintf(stderr,"%s/s ", outfmt(tput));
	    }
	    fprintf(stderr,"|");
	    for (i=0; i < linelength; ++i) {
		if (i<newpos)
		    fprintf(stderr,"#");
		else
		    fprintf(stderr,"-");
	    }
	    fprintf(stderr,"| %c", '\015');
	}

	currpos = newpos;

	fflush(stderr);
}


void
tickdone(void)
{
	fprintf(stderr,"\n");
}






void
dospeed(int bytes)
{
    float tput = calc_tput(bytes);
    static int ticker = 0;
    static char ticks[] = {'-', '/', '|', '\\' };
    static struct timeval last = {0,0};

    if (tv_iszero(last) || (tv_ago_msecs(last) > 250)) {
	ticker = (ticker+1) % 4;
	fprintf(stderr,"\r%s/s %c  ", outfmt(tput), ticks[ticker]);
	gettimeofday(&last, NULL);
    }

    fflush(stderr);
}
