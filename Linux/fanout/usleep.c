#if !defined(lint) && defined(SCCSIDS)
static	char sccsid[] = "@(#)usleep.c 1.5 88/02/08 SMI"; /* from UCB 5.1 85/06/05 */
#endif

/* Copyright 1988, 1993 SMCC */

/*
 * Copyright (c) 1985 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#ifdef SUNOS5

#include <sys/time.h>
#include <signal.h>

#define USPS	1000000		/* number of microseconds in a second */
#define TICK	10000		/* system clock resolution in microseconds */

#define	setsa(sa, a) \
	sa.sa_handler = a; sigemptyset(&sa.sa_mask); sa.sa_flags = 0

static int ringring;

usleep(n)
	unsigned n;
{
	static void sleepx();
	sigset_t mask, omask;
	struct itimerval itv, oitv;
	register struct itimerval *itp = &itv;
	struct sigaction sa, osa;

	if (n == 0)
		return;
	timerclear(&itp->it_interval);
	timerclear(&itp->it_value);
	if (setitimer(ITIMER_REAL, itp, &oitv) < 0)
		return;
	itp->it_value.tv_sec = n / USPS;
	itp->it_value.tv_usec = n % USPS;
	if (timerisset(&oitv.it_value)) {
		if (timercmp(&oitv.it_value, &itp->it_value, >)) {
			oitv.it_value.tv_sec -= itp->it_value.tv_sec;
			oitv.it_value.tv_usec -= itp->it_value.tv_usec;
			if (oitv.it_value.tv_usec < 0) {
				oitv.it_value.tv_usec += USPS;
				oitv.it_value.tv_sec--;
			}
		} else {
			itp->it_value = oitv.it_value;
			oitv.it_value.tv_sec = 0;
			oitv.it_value.tv_usec = 2 * TICK;
		}
	}
	setsa(sa, sleepx);
	(void) sigaction(SIGALRM, &sa, &osa);
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_BLOCK, &mask, &omask);
	sigdelset(&omask, SIGALRM);
	ringring = 0;
	(void) setitimer(ITIMER_REAL, itp, (struct itimerval *)0);
	while (!ringring)
		sigsuspend(&omask);
	(void) sigaction(SIGALRM, &osa, (struct sigaction *)0);
	(void) sigprocmask(SIG_SETMASK, &omask, 0);
	(void) setitimer(ITIMER_REAL, &oitv, (struct itimerval *)0);
}

static void
sleepx()
{

	ringring = 1;
}

#else	/* not sun0s5 - assume BSD */
	/* not needed */
#endif

