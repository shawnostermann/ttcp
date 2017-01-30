#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include "timeval.h"

const struct timeval tv_ZERO = {0,0};

/* timeval manipulation routines */
/* subtract the rhs from the lhs, result in lhs */

#define ERROR_CHECKS
#define BAD_ORDER_FATAL

void
tv_minus_equals(struct timeval *plhs, const struct timeval const_rhs)
{
    struct timeval rhs = const_rhs;
    
    /* better normalize both sides, just in case */
    tv_normalize(plhs);
    tv_normalize(&rhs);

    /* sanity check, lhs MUST BE more than rhs */
#ifdef ERROR_CHECKS
    if (tv_lt(*plhs,rhs)) {
	fprintf(stderr,"tvsub(%s,%s) bad timestamp order!\n",
		tv_format(*plhs), tv_format(rhs));
#ifdef BAD_ORDER_FATAL
	abort();
	exit(-1);
#else
	fprintf(stderr,"returning 0!\n");
	tv_clear(plhs);
	return;
#endif
    }
#endif /* ERROR_CHECKS */

    
    if (plhs->tv_usec >= rhs.tv_usec) {
	plhs->tv_usec -= rhs.tv_usec;
    } else {
	/* borrow one */
	plhs->tv_usec += US_PER_SEC - rhs.tv_usec;
	plhs->tv_sec -= 1;
    }
    plhs->tv_sec -= rhs.tv_sec;
}


/* add the RHS to the LHS, answer in *plhs */
void
tv_plus_equals(
    struct timeval *plhs,
    const struct timeval rhs)
{
    plhs->tv_sec += rhs.tv_sec;
    plhs->tv_usec += rhs.tv_usec;

    tv_normalize(plhs);
}


/*  1: lhs >  rhs */
/*  0: lhs == rhs */
/* -1: lhs <  rhs */
/* N.B. assumes both timevals are normalized! */
int
tv_cmp(const struct timeval const_lhs,
       const struct timeval const_rhs)
{
    struct timeval lhs = const_lhs;
    struct timeval rhs = const_rhs;

    /* better normalize, just in case! */
    tv_normalize(&lhs);
    tv_normalize(&rhs);

    if (lhs.tv_sec > rhs.tv_sec) {
	return(1);
    }

    if (lhs.tv_sec < rhs.tv_sec) {
	return(-1);
    }

    /* ... else, seconds are the same */
    if (lhs.tv_usec > rhs.tv_usec)
	return(1);
    else if (lhs.tv_usec == rhs.tv_usec)
	return(0);
    else
	return(-1);
}


/* how many seconds ago did 'pasttime' happen? */
int
tv_ago_secs(struct timeval pasttime)
{
    return(tv_ago_msecs(pasttime)/1000);
}


/* how many milliseconds ago did 'pasttime' happen? */
int
tv_ago_msecs(struct timeval pasttime)
{
    struct timeval now, elapsed;

    /* never happened */
    if (tv_iszero(pasttime))
	return(-1);

    /* current time */
    gettimeofday(&now,NULL);

    /* subtract past time */
    elapsed = tv_minus(now,pasttime);

    return(elapsed.tv_sec*1000 + elapsed.tv_usec/1000);
}


void
tv_normalize(
    struct timeval *p)
{
    if ((p->tv_sec < 0) || (p->tv_usec < 0)) {
	fprintf(stderr,"Warning: tv_normalize called on (%ld,%ld)\n",
		(long)p->tv_sec, (long)p->tv_usec);
	abort();
	exit(-1);
    }

    /* convert usec field to 0-999999 */
    if (p->tv_usec >= US_PER_SEC) {
	long moresecs = p->tv_usec / US_PER_SEC;
	long newusecs = p->tv_usec % US_PER_SEC;

	/* normalize */
	p->tv_sec += moresecs;
	p->tv_usec = newusecs;
    }
}


struct timeval *
tv_init(
    struct timeval *p,
    u_long sec,
    u_long msec)
{
    /* watch for overflow! (msec could be too large to convert to usecs!) */
    if (msec > 1000) {
	sec += msec / 1000;
	msec = msec % 1000;
    }

    p->tv_sec = sec;
    p->tv_usec = 1000 * msec;	/* stored field is in microseconds! */

    return(p);
}


char *
tv_format(
    struct timeval tv)
{
    static char buf1[20];
    static char buf2[20];
    static char *buf = NULL;

    /* alternate buffer use for double printing */
    buf = (buf == buf1)?buf2:buf1;

    sprintf(buf, "%lu.%06lu", (u_long)tv.tv_sec, (u_long)tv.tv_usec);

    return(buf);
}


/* inline addition RET = lhs + rhs */
struct timeval
tv_plus(
    const struct timeval lhs,
    const struct timeval rhs)
{
    struct timeval ans;

    ans = lhs;
    tv_plus_equals(&ans, rhs);

    return(ans);
}


/* inline subtraction RET = lhs - rhs */
struct timeval
tv_minus(const struct timeval lhs,
	 const struct timeval rhs)
{
    struct timeval ans;

    ans = lhs;
    tv_minus_equals(&ans, rhs);

    return(ans);
}
