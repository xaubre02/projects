/* --------------------------
 * Predmet: POS 2018/2019
 * Projekt: ticket algoritmus
 * Datum:   2019-04-07
 * Autor:   Tomas Aubrecht
 * Login:   xaubre02
 * -------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

// TODO: unixové konce řádků (0x0A) a kódování ISO8859-2
int getticket(void) {
    
}

void await(int aenter) {
    
}

void advance(void) {
    
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
    long m, n;
    if (argc < 3 || !str2long(argv[1], &n) || !str2long(argv[2], &m)) {
        printf("Program vyzaduje dva povinne parametry: cisla N a M, kde N je pocet vlaken a M je pocet pruchodu kritickou sekci.");
        return -1;
    }

    return 0;
}