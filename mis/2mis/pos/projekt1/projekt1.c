/* --------------------------
 * Predmet: POS 2018/2019
 * Projekt: ticket algoritmus
 * Datum:   2019-04-07
 * Autor:   Tomas Aubrecht
 * Login:   xaubre02
 * -------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>


/* Global variables */
long m, n;
volatile int curr_ticket = 0;
pthread_mutex_t ticket_lock = PTHREAD_MUTEX_INITIALIZER; // curr_ticket lock
pthread_mutex_t ticket_dispatcher = PTHREAD_MUTEX_INITIALIZER; // getticket() lock
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // await(), advance()

struct threadt_t {
    int id;
    int seed;
    int ticket;
    pthread_t thread;
};

int getticket(void) {
    int tmp_ticket;
    static volatile int next_ticket = 0;
    /* lock */
    if (pthread_mutex_lock(&ticket_dispatcher)){
        fprintf(stderr, "Chyba! Nepodarilo se zamcit zamek.\n");
    }

    tmp_ticket = next_ticket++;

    /* unlock */
    if (pthread_mutex_unlock(&ticket_dispatcher)){
        fprintf(stderr, "Chyba! Nepodarilo se odemcit zamek.\n");
    }

    return tmp_ticket;
}

void await(int aenter) {
    /* lock */
    if (pthread_mutex_lock(&ticket_lock)){
        fprintf(stderr, "Chyba! Nepodarilo se zamcit zamek.\n");
    }

    while (aenter != curr_ticket) {
        /* wait */
        if (pthread_cond_wait(&cond, &ticket_lock)) {
            fprintf(stderr, "Chyba! Nepodarilo se cekat.\n");
        }
    }
}

void advance(void) {
    /* unlock */
    if (pthread_mutex_unlock(&ticket_lock)){
        fprintf(stderr, "Chyba! Nepodarilo se odemcit zamek.\n");
    }
    
    curr_ticket++;

    if (pthread_cond_broadcast(&cond)) {
        fprintf(stderr, "Chyba! Nepodarilo se odblokovat ostatni vlakna.\n");
    }
}

void wait_random(unsigned seed) {
    struct timespec ts = {0};
    ts.tv_nsec = rand_r(&seed) % 500000000L; /* <0,0.5>*/
    /* sleep for the specified period of time (and or continue after interrupt) */
    errno = 0;
    while(nanosleep(&ts, &ts) == -1 && errno == EINTR);
}

/* Thread routine. */
void *routine(void *data) {
    struct threadt_t *thread = data;

    while ((thread->ticket = getticket()) < m) {
        wait_random((unsigned)thread->seed);
        await(thread->ticket);

        printf("%d\t(%d)\n", thread->ticket, thread->id);
        fflush(stdout);
        
        advance();
        wait_random((unsigned)thread->seed);
    }

    return NULL;
}

/* Convert a string to a long. */
bool str2long(char *str, long *num) {
    errno = 0;
    char *eptr;
    *num = strtol(str, &eptr, 10);
    return !(*eptr != '\0'|| errno == EINVAL || errno == ERANGE);
}

/* Main function. */
int main(int argc, char *argv[]) {
    /* parse arguments */
    if (argc < 3 || !str2long(argv[1], &n) || !str2long(argv[2], &m) || m < 0 || n < 0) {
        fprintf(stderr, "Chyba! Program vyzaduje dva povinne parametry: kladna cela cisla N a M,\nkde N je pocet vlaken a M je pocet pruchodu kritickou sekci.\n");
        return EXIT_FAILURE;
    }

    /* allocate required memory */
    struct threadt_t *threads = malloc(sizeof(struct threadt_t) * n);
    if (threads == NULL) {
        fprintf(stderr, "Chyba! Nepodarilo se alokovat pamet pro vsechna vlakna.\n");
        return EXIT_FAILURE;
    }

    /* create threads */
    struct timeval cur_time;
    for (int i = 0; i < n; i++) {
        /* get current time */
        gettimeofday(&cur_time, NULL);

        threads[i].id = i + 1; /* <1,N> */
        threads[i].seed = cur_time.tv_sec * (int)1e6 + cur_time.tv_usec; /* current time in microseconds */
        threads[i].ticket = -1;

        while(pthread_create(&threads[i].thread, NULL, routine, &threads[i])) {
            if (errno != EAGAIN) {           
                fprintf(stderr, "Chyba! Nepodarilo se vytvorit vlakno.\n");
                free(threads);
                return EXIT_FAILURE;
            }
        }
    }

    /* get return values*/
    for (int i = 0; i < n; i++) {
        if (pthread_join(threads[i].thread, NULL)) {
            fprintf(stderr, "Chyba! Nenulova navratova hodnota vlakna cislo %d\n", threads[i].id);
        }
    }

    /* free and exit */
    free(threads);
    return EXIT_SUCCESS;
}