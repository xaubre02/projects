
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1 /* XPG 4.2 - needed for usleep() */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

/*
 * Sdílená promìnná je zde typu int, ale èasto lze výhodnì pou¾ít typ
 * sig_atomic_t, který je vzhledem k pøeru¹ení obsluhou signálu atomický.
 */

volatile int intr;

/*
 * Obslu¾ná funkce
 */

void catcher(int sig)
{
	intr = 1;
}

int main(void)
{
	struct sigaction sigact;
	sigset_t setint;
	char ch;

	/*
	 * Mno¾ina setint pro umo¾nìní doèasného blokování signálu SIGINT
	 */

	sigemptyset(&setint);
	sigaddset(&setint, SIGINT);

	/*
	 * Blokování signálu SIGINT pøi inicializaci; zde to není nutné,
	 * ale u jiných programù je dobré pamatovat na pøíchod signálu
	 * v nevhodnou dobu i pøi inicializaci
	 */

	sigprocmask(SIG_BLOCK, &setint, NULL);

	/*
	 * Úvodní zpráva
	 */

	fprintf(stderr, "Stisknìte 5krát Ctrl-C... ");

	/*
	 * Zmìna obsluhy signálu SIGINT na vlastní funkci catcher()
	 */

	sigact.sa_handler = catcher;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	if (sigaction(SIGINT, &sigact, NULL) == -1) {
		perror("sigaction()");
		return(1);
	}

	/*
	 * Konec blokování signálu SIGINT pøi inicializaci
	 */

	sigprocmask(SIG_UNBLOCK, &setint, NULL);

	/*
	 * Hlavní smyèka
	 */

	ch = 'A';
	while (ch <= 'E') {

		/*
		 * Pokud není pro sdílenou promìnnou pou¾it typ sig_atomic_t,
		 * obsluha signálu nesmí být aktivována v dobì její modifikace
		 * nebo ètení (pøi ètení pokud ji obsluha i modifikuje); zde
		 * by pøi pou¾ití sig_atomic_t bylo mo¾né doèasné blokování
		 * signálu SIGINT vynechat, jeliko¾ pøi zpracování signálu
		 * mezi testem promìnné intr a jejím nulováním by do¹lo
		 * maximálnì ke ztrátì vícenásobného pøíchodu signálu, èemu¾
		 * stejnì není mo¾né zabránit
		 */

		/*
		 * Zaèátek kritické sekce
		 */

		sigprocmask(SIG_BLOCK, &setint, NULL);
		if (intr) {
			intr = 0;
			ch++;
		}
		sigprocmask(SIG_UNBLOCK, &setint, NULL);

		/*
		 * Konec kritické sekce
		 */

		fprintf(stderr, "%c", ch);

		/*
		 * Na BSD systémech mù¾e být funkce usleep() pøeru¹ena
		 * s výsledkem -1 a errno == EINTR
		 */

		if (usleep(250000) == -1) {
			if (errno == EINTR)
				fprintf(stderr, "(EINTR)");
			else
				fprintf(stderr, "(%d)", errno);
		}
	}

	/*
	 * Konec programu
	 */

	printf("\n");
	return 0;
}

