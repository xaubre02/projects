/* --------------------------
 * Predmet: POS 2018/2019
 * Projekt: shell
 * Datum:   2019-04-13
 * Autor:   Tomas Aubrecht
 * Login:   xaubre02
 * -------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MAX_INPUT_LEN 512      // accept 512 symbols INCLUDING '\n'

volatile bool live = true;     // run the shell until the live flag is set to false
volatile bool reading = true;  // buffer sync
char buff[MAX_INPUT_LEN + 1];  // input buffer

pthread_mutex_t mtx  = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void cond_wait() {
    pthread_mutex_lock(&mtx);
    /* no while wait condition */
    pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);
}

void cond_signal() {
    pthread_mutex_lock(&mtx);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mtx);
}

/* Input processing routine. */
void *input_routine(void* args) {
    (void) args;
    ssize_t bytes;

    while(live) {
        printf("$ ");
        fflush(stdout);
        // get input
        bytes = read(0, buff, MAX_INPUT_LEN + 1);
        if (bytes <= MAX_INPUT_LEN) {
            // empty command
            if (buff[0] == '\n') {
                continue;
            } 

            // add terminating symbol (-1 -> replace '\n')
            buff[bytes-1] = '\0';

            // exit command
            if (!strcmp(buff, "exit")) {
                live = false;
                cond_signal();
                break;
            }

            /* signal */
            cond_signal();
            cond_wait();
        }
        // too long input -> ignore
        else {
            fprintf(stderr, "Chyba! Prilis dlouhy vstup.\n");
            // read the rest
            if (buff[MAX_INPUT_LEN] != '\n') {
                while(getc(stdin) != '\n');
            }
            continue;
        }
    }
    
    return NULL;
}

/* Command processing routine. */
void *comnd_routine(void* args) {
    (void) args;
    
    while(true) {
        /* wait */
        cond_wait();
        if (!live) {
            break;
        }

        printf("Buffer: %s\n", buff);
        fflush(stdout);
        cond_signal();
    }
    
    return NULL;
}

/* Main function. */
int main(void) {

    pthread_t thrd_input; // input processing thread
    pthread_t thrd_comnd; // command processing thread

    // create threads
    while(pthread_create(&thrd_input, NULL, input_routine, NULL)) {
        if (errno != EAGAIN) {           
            fprintf(stderr, "Chyba! Nepodarilo se vytvorit vlakno pro zpracovani vstupu.\n");
            return EXIT_FAILURE;
        }
    }

    while(pthread_create(&thrd_comnd, NULL, comnd_routine, NULL)) {
        if (errno != EAGAIN) {           
            fprintf(stderr, "Chyba! Nepodarilo se vytvorit vlakno pro zpracovani prikazu.\n");
            return EXIT_FAILURE;
        }
    }

    /* get return values*/
    if (pthread_join(thrd_input, NULL)) {
        fprintf(stderr, "Chyba! Nenulova navratova hodnota vlakna pro zpracovani vstupu.\n");
    }

    if (pthread_join(thrd_comnd, NULL)) {
        fprintf(stderr, "Chyba! Nenulova navratova hodnota vlakna pro zpracovani prikazu.\n");
    }

    return EXIT_SUCCESS;
}