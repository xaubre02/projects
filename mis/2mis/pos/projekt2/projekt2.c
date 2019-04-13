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
#include <ctype.h>

#define MAX_INPUT_LEN 512      // accept 512 symbols INCLUDING '\n'

/* Global variables */
volatile bool live = true;     // run the shell until the live flag is set to false
char buff[MAX_INPUT_LEN + 1];  // input buffer

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;



struct program_t {             // program metadata
    unsigned arg_limit;
    unsigned argc;
    bool background;
    char **args;
    char *input;
    char *output;
    bool valid;
};

enum parser_states {           // states of the parser
    ps_init,
    ps_args,
    ps_input,
    ps_input_cont,
    ps_output,
    ps_output_cont
};

void print_parsed(struct program_t *program) {
    printf("\n--------------------\nBACKGROUND: %d\n", program->background);
    printf("ARGC: %d\n", program->argc);
    printf("INPUT: %s\n", program->input);
    printf("OUTPUT: %s\n", program->output);
    printf("ARGUMENTS: ");
    for (unsigned i = 0; i < program->argc; i++) {
        printf("%s ", program->args[i]);
    }
    printf("\n------------------------\n");
}

/* Initialize program_t variables. */
void init_program(struct program_t *program) {
    program->arg_limit = 4;
    program->background = false;
    program->argc = 0;

    program->args = malloc(sizeof(char *) * program->arg_limit);
    if (!program->args) {
        fprintf(stderr, "Chyba! Nepodarilo se naalokovat pamet.\n");
    }
    else {
        for (unsigned i = 0; i < program->arg_limit; i++) {
            program->args[i] = NULL;
        }
    }

    program->input = NULL;
    program->output = NULL;
    program->valid = true;
}

/* Clear program_t variables. */
void clear_program(struct program_t *program) {
    program->background = false;

    for (unsigned i = 0; i < program->argc; i++) {
        if(program->args[i]) {
            free(program->args[i]);
            program->args[i] = NULL;
        }
    }

    program->argc = 0;

    if(program->input) {
        free(program->input);
        program->input = NULL;
    }

    if(program->output) {
        free(program->output);
        program->output = NULL;
    }

    program->valid = true;
}

/* Store a string. */
bool store_string(char **dest, unsigned spos, unsigned epos) {
    // str length
    unsigned len = epos - spos + 1;
    // free the memory first
    if(*dest) {
        free(*dest);
    }
    // allocate memory
    *dest = malloc(sizeof(char) * (len + 1));
    if (*dest == NULL) {
        fprintf(stderr, "Chyba! Nepodarilo se naalokovat pamet.\n");
        return false;
    }
    // copy a string to the destination
    memcpy(*dest, buff + spos, len);
    // add terminating symbol
    (*dest)[len] = '\0';
    return true;
}

/* Add program's argument. */
void add_argument(struct program_t *program, unsigned spos, unsigned epos) {
    // re-allocate memory
    if (++program->argc > program->arg_limit) {
        program->arg_limit += 2;
        program->args = (char **) realloc(program->args, sizeof(char *) * program->arg_limit);
    }

    // store a string
    store_string(&(program->args[program->argc-1]), spos, epos);
}

/* Parse command buffer. */
void parse_buffer(struct program_t *program) {
    char c;              // current char
    int state = ps_init; // parser state
    unsigned spos = 0;   // start position
    unsigned epos = 0;   // end position
    unsigned blen = strlen(buff); // buffer length

    // loop throught all chars in the buffer
    for (unsigned i = 0; i < blen && program->valid; i++) {
        c = buff[i];
        epos = i;

        switch (state) {
            case ps_init:
                if (isspace(c))
                    continue;
                else if (c == '&') {
                    if (program->background) 
                        program->valid = false;      // already set
                    else
                        program->background = true;
                }
                else if (c == '<') {
                    if (program->input || i == blen - 1) { // already set or not specified at all
                        program->valid = false;
                        break;
                    }
                    state = ps_input;
                }
                else if (c == '>') {
                    if (program->output || i == blen - 1) { // already set or not specified at all
                        program->valid = false;
                        break;
                    }
                    state = ps_output;
                }
                else {
                    spos = i;
                    state = ps_args;
                    // single symbol
                    if (i == blen - 1) {
                        add_argument(program, spos, epos);
                    }
                }
                break;
            
            case ps_args:
                if (isspace(c)) {
                    add_argument(program, spos, epos-1);
                    state = ps_init;
                }
                else if (c == '&') {
                    if (program->background){ // already set
                        program->valid = false;
                        break;
                    }
                    else
                        program->background = true;

                    add_argument(program, spos, epos-1);
                    state = ps_init;
                }
                else if (c == '<') {
                    if (program->input || i == blen - 1) { // already set or not specified at all
                        program->valid = false;
                        break;
                    }
                    add_argument(program, spos, epos-1);
                    state = ps_input;
                }
                else if (c == '>') {
                    if (program->output || i == blen - 1) { // already set or not specified at all
                        program->valid = false;
                        break;
                    }
                    add_argument(program, spos, epos-1);
                    state = ps_output;
                }
                else if (i == blen - 1) {
                    add_argument(program, spos, epos);
                }
                break;
            
            case ps_input:
                if (c == '>' || c == '<' || c == '&' || i == blen - 1) { // already set or not specified
                    program->valid = false;
                    break;
                }
                else if (!isspace(c)) {
                    spos = i;
                    state = ps_input_cont;
                }
                break;

            case ps_input_cont:
                if (isspace(c)) {
                    store_string(&(program->input), spos, epos-1);
                    state = ps_init;
                }
                else if (c == '&') {
                    if (program->background){ // already set
                        program->valid = false;
                        break;
                    }
                    else
                        program->background = true;

                    store_string(&(program->input), spos, epos-1);
                    state = ps_init;
                }
                else if (c == '<') {
                    program->valid = false; // already set
                }
                else if (c == '>') {
                    if (program->output) { // already set
                        program->valid = false;
                        break;
                    }
                    store_string(&(program->input), spos, epos-1);
                    state = ps_output;
                }
                else if (i == blen - 1) {
                    store_string(&(program->input), spos, epos);
                }
                break;

            case ps_output:
                if (c == '>' || c == '<' || c == '&' || i == blen - 1) { // already set or not specified
                    program->valid = false;
                    break;
                }
                else if (!isspace(c)) {
                    spos = i;
                    state = ps_output_cont;
                }
                break;

            case ps_output_cont:
                if (isspace(c)) {
                    store_string(&(program->output), spos, epos-1);
                    state = ps_init;
                }
                else if (c == '&') {
                    if (program->background){ // already set
                        program->valid = false;
                        break;
                    }
                    else
                        program->background = true;

                    store_string(&(program->output), spos, epos-1);
                    state = ps_init;
                }
                else if (c == '<') {
                    if (program->input) { // already set
                        program->valid = false;
                        break;
                    }
                    store_string(&(program->output), spos, epos-1);
                    state = ps_input;
                }
                else if (c == '>') {
                    program->valid = false; // already set
                }
                else if (i == blen - 1) {
                    store_string(&(program->output), spos, epos);
                }
                break;

            default:
                break;
        }
    }
}

/* Condition wait function. */
void cond_wait() {
    pthread_mutex_lock(&mtx);
    /* no while wait condition */
    pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);
}

/* Condition signal function. */
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

            /* signal and wait */
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
    
    struct program_t program;
    init_program(&program);

    while(true) {
        /* wait */
        cond_wait();
        if (!live) {
            break;
        }

        parse_buffer(&program);

        if (program.valid)
            // execute the program
            print_parsed(&program);
        else {
            fprintf(stderr, "Chyba! Nespecifikovany ci opakujici se argumenty programu.\n");
        }

        // clear the program
        clear_program(&program);

        /* signal */
        cond_signal();
    }
    free(program.args);
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