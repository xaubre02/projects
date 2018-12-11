/**
 * Kostra programu pro 3. projekt IZP 2015/16
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 * http://is.muni.cz/th/172767/fi_b/5739129/web/web/slsrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/**
 Inicializace shluku 'c'. Alokuje pamet pro kapacitu objektu (cap).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
 */
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->size = 0;
    c->capacity = cap;
    c->obj = malloc(sizeof(struct obj_t) * cap);

    if(c->obj == NULL)
        c->capacity = 0;
}

/**
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);
    c->obj=NULL;
    c->size = 0;
    c->capacity = 0;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/**
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/**
 * Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 * nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if(c->size >= c->capacity)
    {
        c = resize_cluster(c, c->capacity + CLUSTER_CHUNK);
    }

    c->obj[c->size] = obj;
    c->size++;
}

/**
 * Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/**
 * Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 * Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 * Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    for(int i = 0; i < c2->size; i++)
    {
        append_cluster(c1, c2->obj[i]);
    }
    sort_cluster(c1);
}

/**********************************************************************/
/** Prace s polem shluku */

/**
 * Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 * (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 * pocet shluku v poli.
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    clear_cluster(&carr[idx]);
    for(int i = idx; i < narr-1; i++)
    {
        carr[i] = carr[i+1];
    }
    narr--;

    return narr;
}

/**
 * Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    return sqrtf((o1->x - o2->x) * (o1->x - o2->x) + (o1->y - o2->y) * (o1->y - o2->y));
}

/**
 * Pocita vzdalenost dvou shluku. Vzdalenost je vypoctena na zaklade nejblizsiho
 * souseda.
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float minimal = 3000;
    float dist;

    for(int i=0; i < c1->size; i++)
    {
        for(int j=0; j < c2->size; j++)
        {
            dist = obj_distance(&c1->obj[i], &c2->obj[j]);
            if(dist < minimal)
            {
                minimal = dist;
            }
        }
    }
    return minimal;
}

/**
 * Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 * hleda dva nejblizsi shluky (podle nejblizsiho souseda). Nalezene shluky
 * identifikuje jejich indexy v poli 'carr'. Funkce nalezene shluky (indexy do
 * pole 'carr') uklada do pameti na adresu 'c1' resp. 'c2'.
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float minimal = 3000;
    float dist;

    for(int i=0; i < (narr-1); i++)
    {
        for(int j=i+1; j < narr; j++)
        {
            dist = cluster_distance(&carr[i], &carr[j]);
            if(dist < minimal)
            {
                minimal = dist;
                *c1 = i;
                *c2 = j;
            }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/**
 * Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
 */
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/**
 *Tisk shluku 'c' na stdout.
 */
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/**
 * Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 * jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 * polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 * kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 * V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
 */
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    FILE *soubor = fopen(filename, "r");
    if(soubor == NULL)
    {
        fprintf(stderr, "Nepovedlo se otevrit soubor: '%s'\n", filename);

        return -1;
    }

    int count;
    int check = fscanf(soubor, "count=%d", &count);
    if(check != 1)
    {
        fprintf(stderr, "Neplatny format prvniho radku!\n");
        *arr = NULL;
        fclose(soubor);
        return -1;
    }
    struct cluster_t *pomarr = malloc(sizeof(struct cluster_t) * count);
    struct obj_t pomobj;

    for(int i = 0; i<count; i++)
    {
        init_cluster(&pomarr[i], 1);
        fscanf(soubor, "%d %f %f", &pomobj.id, &pomobj.x, &pomobj.y);

        if(pomobj.x>1000 || pomobj.x<0 || pomobj.y>1000 || pomobj.y<0)
        {
            for(int j = 0; j<=i; j++)
            {
                clear_cluster(&pomarr[j]);
            }
            free(pomarr);
            fclose(soubor);
            *arr = NULL;
            fprintf(stderr, "U %d. objektu neni X/Y z intervalu [0,1000]\n", i+1);
            return -1;
        }

        append_cluster(&pomarr[i], pomobj);
    }
    *arr = pomarr;

    if(fclose(soubor) == EOF)
    {
        fprintf(stderr, "Nepovedlo se zavrit soubor: '%s'\n", filename);
        *arr = NULL;
        return -1;
    }
    return count;
}

/**
 * Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 * Tiskne se prvnich 'narr' shluku.
 */
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;
    char* chyba = '\0';
    int narr;

    if(argc == 1)
    {
        printf("Program se spousti v nasledujici podobe:\n\n./proj3 SOUBOR [N]\n\n"
               "Argumenty programu:\n"
               " ** SOUBOR je jmeno souboru se vstupnimi daty.\n"
               " ** N je volitelny argument definujici cilovy pocet shluku. N > 0. Vychozi hodnota je 1.\n");
        return 0;
    }
    int count = load_clusters(argv[1], &clusters);
    if(count>0)
    {
        if(argc == 2)
        {
            narr = 1;
        }
        else if(argc == 3)
        {
            narr = strtol(argv[2], &chyba, 10);
        }
        else if(argc > 3)
        {
            fprintf(stderr, "Neplatny vstup!\n"
                            "Spustte program bez argumentu pro napovedu.\n");
            return -1;
        }
        if(argc == 2 || *chyba == '\0')
        {
            if(narr > count)
            {
                narr = count;
            }
            int x1;
            int x2;
            while(count != narr)
            {
                find_neighbours(clusters, count, &x1, &x2);
                merge_clusters(&clusters[x1], &clusters[x2]);
                count = remove_cluster(clusters, count, x2);
            }
            print_clusters(clusters, narr);
            for(int i = 0; i<narr; i++)
            {
                clear_cluster(&clusters[i]);
            }
        }
        else
        {
            fprintf(stderr, "Neplatny vstup!\n"
                            "Spustte program bez argumentu pro napovedu.\n");
            return -1;
        }
        free(clusters);
    }
    else
    {
        return -1;
    }

    return 0;
}
