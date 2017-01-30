/* routines and macros for dealing with timevals */
/* Shawn Ostermann - Mon Apr 26, 1999 */

typedef unsigned long u_long;

/* for initting from constants (note arg3 type!) */
struct timeval *tv_init(struct timeval *p, u_long sec, u_long msec);

/* adding and subtracting times */
void tv_minus_equals(struct timeval *plhs, const struct timeval rhs);
void tv_plus_equals(struct timeval *plhs, const struct timeval rhs);

/* inline addition and subtraction */
struct timeval tv_plus(const struct timeval lhs, const struct timeval rhs);
struct timeval tv_minus(const struct timeval lhs, const struct timeval rhs);

/* general comparison */
/*  1: lhs >  rhs */
/*  0: lhs == rhs */
/* -1: lhs <  rhs */
int tv_cmp(struct timeval lhs, struct timeval rhs);

/* elapsed time various units (now - pasttime) */
int tv_ago_secs(struct timeval pasttime);
int tv_ago_msecs(struct timeval pasttime);

/* convert fields to normalized values */
void tv_normalize(struct timeval *p);

/* for printing */
char *tv_format(const struct timeval tv);




/* comparison macros */
#define tv_ge(lhs,rhs) (tv_cmp((lhs),(rhs)) >= 0)
#define tv_gt(lhs,rhs) (tv_cmp((lhs),(rhs)) >  0)
#define tv_le(lhs,rhs) (tv_cmp((lhs),(rhs)) <= 0)
#define tv_lt(lhs,rhs) (tv_cmp((lhs),(rhs)) <  0)
#define tv_eq(lhs,rhs) (tv_cmp((lhs),(rhs)) == 0)


/* other macros */
#define tv_iszero(tv)  (((tv).tv_sec == 0)&&((tv).tv_usec == 0))
#define tv_nonzero(tv) (((tv).tv_sec != 0)||((tv).tv_usec != 0))
#define tv_clear(ptv)  (ptv)->tv_sec = (ptv)->tv_usec = 0


/* global constants */
extern const struct timeval tv_ZERO;


/* handy constants to avoid booboos */
#define US_PER_SEC 1000000	/* microseconds per second */
#define MS_PER_SEC 1000		/* milliseconds per second */
