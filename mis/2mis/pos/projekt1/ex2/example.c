
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1 /* XPG 4.2 - needed for usleep() */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

/*
 * Sd�len� prom�nn� je zde typu int, ale �asto lze v�hodn� pou��t typ
 * sig_atomic_t, kter� je vzhledem k p�eru�en� obsluhou sign�lu atomick�.
 */

volatile int intr;

/*
 * Obslu�n� funkce
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
	 * Mno�ina setint pro umo�n�n� do�asn�ho blokov�n� sign�lu SIGINT
	 */

	sigemptyset(&setint);
	sigaddset(&setint, SIGINT);

	/*
	 * Blokov�n� sign�lu SIGINT p�i inicializaci; zde to nen� nutn�,
	 * ale u jin�ch program� je dobr� pamatovat na p��chod sign�lu
	 * v nevhodnou dobu i p�i inicializaci
	 */

	sigprocmask(SIG_BLOCK, &setint, NULL);

	/*
	 * �vodn� zpr�va
	 */

	fprintf(stderr, "Stiskn�te 5kr�t Ctrl-C... ");

	/*
	 * Zm�na obsluhy sign�lu SIGINT na vlastn� funkci catcher()
	 */

	sigact.sa_handler = catcher;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	if (sigaction(SIGINT, &sigact, NULL) == -1) {
		perror("sigaction()");
		return(1);
	}

	/*
	 * Konec blokov�n� sign�lu SIGINT p�i inicializaci
	 */

	sigprocmask(SIG_UNBLOCK, &setint, NULL);

	/*
	 * Hlavn� smy�ka
	 */

	ch = 'A';
	while (ch <= 'E') {

		/*
		 * Pokud nen� pro sd�lenou prom�nnou pou�it typ sig_atomic_t,
		 * obsluha sign�lu nesm� b�t aktivov�na v dob� jej� modifikace
		 * nebo �ten� (p�i �ten� pokud ji obsluha i modifikuje); zde
		 * by p�i pou�it� sig_atomic_t bylo mo�n� do�asn� blokov�n�
		 * sign�lu SIGINT vynechat, jeliko� p�i zpracov�n� sign�lu
		 * mezi testem prom�nn� intr a jej�m nulov�n�m by do�lo
		 * maxim�ln� ke ztr�t� v�cen�sobn�ho p��chodu sign�lu, �emu�
		 * stejn� nen� mo�n� zabr�nit
		 */

		/*
		 * Za��tek kritick� sekce
		 */

		sigprocmask(SIG_BLOCK, &setint, NULL);
		if (intr) {
			intr = 0;
			ch++;
		}
		sigprocmask(SIG_UNBLOCK, &setint, NULL);

		/*
		 * Konec kritick� sekce
		 */

		fprintf(stderr, "%c", ch);

		/*
		 * Na BSD syst�mech m��e b�t funkce usleep() p�eru�ena
		 * s v�sledkem -1 a errno == EINTR
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

