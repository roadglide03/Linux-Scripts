static	char *sccsid = "%W% %E%";

/*
 * Fanout - fanout characters to multiple terminals.
 *
 * usage: fanout [-e escape] [-x] [-t] ttya ttyb ttyc ...
 *
 * Options:
 *    -t	enable telnet escapes (default nothing)
 * Not done:
 *    -e<char>	set escape char to <char>
 *
 * Generic features:
 *    ESCAPE N	toggle output to Nth (starting at 0) tty
 *    ESCAPE ?	show fd status
 *    ESCAPE .	disconnect
 *    ESCAPE z	suspend fanout
 *
 * Telnet support:
 *    ESCAPE .	send telnet quit and disconnect
 *    ESCAPE #	send telnet break
 *    ESCAPE b	send telnet break
 *
 * Copyright (c) 1992-1998 Larry McVoy
 */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>	/* for linux: TIOCSTI lives here */
#ifdef	__hpux__
#include <sys/strtio.h>	/* TIOCSTI lives here on hpux 10.20 */
#endif
#include <malloc.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#ifdef	_AIX
#include <sys/select.h>
#endif

#define	SKIP	0x80000000	/* no output to this tty */
#define	CLOSE	0x40000000	/* closed */
#define	ESCAPE	'\007'		/* ~ is used too much, use ^G instead */
#define	HOSTS	"FANOUT_HOSTS"
#define TIOCSTI 0x5412 

char	*getenv();
void	cont();
void	cont(int);
void	done(int ret);
int	escape(int c);
int	fanout(int c);
void	fanoutstr(char *s);
void	gethosts(void);
void	getids(void);
int	hasio(int fd);
int	nextchar(char *cp);
void	putbackchar(int c);
int	safesystem(char *s);
void	sendstr(int fd, char *s);

int	*fds;
char	**ttys;
char	*prog;
char	**ids;
char	**hosts;
char	esc_char = ESCAPE;
int	nttys;
int	telnet = 0;
int	kid;
int	idfd;
struct	termios tio;
struct	termios save;

int 
main(ac, av)
	char	**av;
{
	int	i;
	char	c;
	int	seen_escape = 0;
	int	pipefds[2];
	struct	sigaction sa;

	prog = av[0];

	if (geteuid()) {
		fprintf(stderr, "This program needs to be setuid root.\n");
		sleep(2);
		exit(1);

	}
	if (!(fds = (int*)malloc(sizeof(int) * ac)) ||
	    !(ids = (char**)malloc(sizeof(char*) * ac)) ||
	    !(hosts = (char**)malloc(sizeof(char*) * ac))) {
		perror("malloc");
		done(1);
	}
	while (ac > 1 && av[1][0] == '-') {
		if (!strcmp("-t", av[1])) {
			telnet++, av++, ac--;
		} else if (!strncmp("-e", av[1], 2)) {
			if (strlen(av[1]) == 3) {
				esc_char = av[1][2];
			} else {
				if (!av[2])
					goto usage;
				esc_char = av[2][0];
				av++, ac--;
			}
			av++, ac--;
		} else {
usage:			fprintf(stderr, "usage: %s [-t] tty tty [tty..]\n",
			    prog);
			done(1);
		}
	}
	ttys = &av[1];
	for (i = 0; i < ac - 1; ++i) {
		if ((fds[i] = open(ttys[i], 2)) == -1) {
			perror(ttys[i]);
			done(1);
		}
	}
	fds[nttys = i] = 0;

	/*
	 * Set our stdin in raw mode.  This assumes that the
	 * other tty line disciplines know what to do.
	 *
	 * XXX - for pipes this doesn't work, so skip it.
	 */
	if (tcgetattr(1, &tio) == 0) {
		save = tio;
		tio.c_lflag = 0;
		tio.c_iflag = 0; 
		tio.c_cc[VMIN] = 1;
		tio.c_cc[VTIME] = 0;
		(void)tcsetattr(0, TCSAFLUSH, &tio);
	}

	sa.sa_handler = cont;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGCONT, &sa, 0);
	while (nextchar(&c) == 1) {
		if (c == esc_char) {
			seen_escape = 1;
			continue;
		}
		if (seen_escape) {
			if (escape(c))
				goto out;
			seen_escape = 0;
		} else {
			if (fanout(c) == 0)
				break;
		}
	}
out:
	if (isatty(0) && tcsetattr(0, TCSANOW, &save)) {
		perror("tcgetattr");
		done(1);
	}
	done(0);
	/*NOTREACHED*/
}

/*
 * send to everyone that wants it
 */
int
fanout(c)
	char	c;
{
	int	i;
	int	o = 0;

	for (i = 0; i < nttys; i++) {
		if (fds[i] & CLOSE)
			continue;
		if (fds[i] & SKIP) {
			o++;
			continue;
		}
		if (ioctl(fds[i], TIOCSTI, &c) == -1 && 
		    (errno == ENXIO || errno == EIO)) {	/* closed */
			close(fds[i]);
			fds[i] |= CLOSE;
		} else {
			o++;
		}
#ifdef	annex
		usleep(100);	/* Xylogics annex seems to like this better */
#endif
	}
	return (o);
}

void
fanoutstr(s)
	char	*s;
{
	while (*s) {
		fanout(*s++);
	}
}

void
sendstr(fd, s)
	char	*s;
{
	while (*s) 
		ioctl(fd, TIOCSTI, s++);
}

int
escape(c)
	char	c;
{
	int	i;

	switch (c) {
	    case 'q':	/* done */
	    case '.':	/* done */
		if (telnet) {
			/* Assuming in connected mode */
			fanoutstr("quit\n");
			/* Assuming not in connected mode */
			fanoutstr("\nquit\n");
		}
		return 1;

	    case '#':	/* break */
	    case 'b':	/* break */
		if (telnet)
			fanoutstr("send brk\n\n");
		else
			fprintf(stderr, "%s: need -t for breaks.\n", prog);
		return 0;

	    case '0': case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7': case '8': case '9':
		c -= '0';

		/*
		 * Weird thing for > 1 char numbers.  Do a select to see
		 * if they are going to type in another number.  Only do
		 * it for numbers that make sense, i.e., '1' if we have 
		 * less than 20 ttys.
		 */
		if ((nttys > 9) && (c * 10 <= nttys) && hasio(0)) {
			char	c2;

			if (nextchar(&c2) != 1) {
				perror("read");
			} else {
				if ((c2 >= '0') && (c2 <= '9'))
					c += (c2 - '0') * 10;
				else
					putbackchar(c2);
			}
		}
		if (c >= nttys) {
		    fprintf(stderr, "%s: tty %c out of range, max = %d.\n",
			prog, c, nttys);
		    return 0;
		}
		if (fds[c] & SKIP)
			fds[c] &= ~SKIP;
		else
			fds[c] |= SKIP;
		break;

	    case '?':
		gethosts();
		printf("%-3s%-12s%-12s%s\n",
		    "N", "tty", "title", "status");
		for (i = 0; i < nttys; i++) {
			/*
			 * XXX - this should have a -q to shut it up so
			 * that this can easily be used by other programs.
			 */
			printf("%-3d%-12s%-12s%s, %s\n", i, ttys[i], hosts[i],
			    fds[i] & CLOSE ? "closed" : "open",
			    fds[i] & SKIP ? "no output" : "outout");
		}
		return 0;

	    case 'z':	/* stop */
	    case 0x1a:
		if (tcsetattr(0, TCSANOW, &save)) {
			perror("fanout: tcgetattr");
			done(1);
		}
		kill(getpid(), SIGTSTP);
		return 0;

	    default:
	    	if (c == esc_char) {
			fanout(esc_char);
			return 0;
		}
		fanout(esc_char);
		fanout(c);
	}
	return 0;
}

void
cont(int dummy)
{
	if (tcsetattr(0, TCSANOW, &tio)) {
		perror("fanout: tcgetattr");
		done(1);
	}
	escape('o');
}

/*
 * Read the ids from the pipe
 */
void
getids()
{
	int	j = 0;
	int	i = 0;
	char	buf[100];

	buf[0] = 0;
	while (read(idfd, &buf[i], 1) == 1) {
		if (buf[i] != '\n') {
			i++;
			continue;
		}
		buf[i] = 0;
		ids[j++] = strdup(buf);
		i = 0;
		buf[0] = 0;
	}
	close(idfd);
	idfd = -1;
	if (j != nttys) {
		fprintf(stderr, "%s: Not enough window ids, want=%d got=%d\n",
		    prog, nttys, j);
		fprintf(stderr, "HOSTS=%s\n", getenv(HOSTS));
sleep(10);
	}
}

/*
 * Read the hosts from FANOUT_HOSTS var
 */
void
gethosts()
{
	char	*s, *t;
	int	i = 0;
	static	been_here = 0;

	if (been_here)
		return;
	been_here = 1;
	if (s = getenv(HOSTS)) {
		do {
			hosts[i] = strdup(s);
			if ((t = strchr(hosts[i], ' ')) ||
			    (t = strchr(hosts[i], '\t')))
				*t = 0, s = t + 1;
			else
				s = NULL;
			i++;
		} while (s && i < nttys);
	} else {
		for (i = 0; i < nttys; i++)
			hosts[i] = "?";
	}
}

void
done(ret)
{
	if (kid)
		kill(kid, SIGKILL);
	exit(ret);
}

int	_save_char = -1;

void
putbackchar(c)
{
	_save_char = c;
}

int
nextchar(cp)
	char	*cp;
{
	int	n;

	if (_save_char != -1) {
		*cp = _save_char;
		_save_char = -1;
		return(1);
	} else {
		n = read(0, cp, 1);
		return (n);
	}
}

/*
 * Return true if input available on descriptor
 */
int
hasio(fd)
{
	fd_set	mask;
	struct timeval tv;

	FD_ZERO(&mask);
	FD_SET(fd, &mask);
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	if (select(32, &mask, 0, 0, &tv) <= 0)
		return (0);
	return (1);
}

/*
 * Need this because we need to go back to normal uid.
 * This program is run setuid root.
 */
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/* extern int execl(); */

int
safesystem(s)
char	*s;
{
	int	status;
	pid_t	pid, w;
	register void (*istat)(), (*qstat)();

	if ((pid = fork()) == 0) {
		setuid(getuid());
		(void) execl("/bin/sh", "sh", "-c", s, (char *)0);
		_exit(127);
	}
	if (pid == -1) {
		return (-1);
	}
	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	w = waitpid(pid, &status, 0);
	(void) signal(SIGINT, istat);
	(void) signal(SIGQUIT, qstat);
	return ((w == -1) ? -1: status);
}
