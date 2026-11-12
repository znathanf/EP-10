#include "teamlist.h"
#include <stdio.h>
#include <stdlib.h>

// Implementação do TAD TeamList com LISTA DUPLAMENTE ENCADEADA.

typedef struct node Node;

struct node {
    Team *team;
    Node *next;
    Node *prev;
};

struct teamlist {
    Node *first;  // primeiro nó (NULL se a lista está vazia)
    Node *last;   // último nó: torna o tl_append imediato
    int size;     // quantidade de times armazenados
};

TeamList *tl_create(void) {
    TeamList *tl = malloc(sizeof(TeamList));
    if (tl == NULL)
        return NULL;

    tl->first = NULL;
    tl->last = NULL;
    tl->size = 0;
    return tl;
}

void tl_append(TeamList *tl, Team *t) {
    Node *n = malloc(sizeof(Node));
    if (n == NULL) {
        fprintf(stderr, "tl_append: sem memória\n");
        exit(EXIT_FAILURE);
    }
    n->team = t;
    n->next = NULL;
    n->prev = tl->last;

    if (tl->last == NULL)
        tl->first = n;        // lista vazia: o novo nó é o primeiro
    else
        tl->last->next = n;   // senão, encadeia depois do último
    tl->last = n;
    tl->size++;
}

int tl_size(TeamList *tl) {
    return tl->size;
}

Team *tl_get(TeamList *tl, int i) {
    if (i < 0 || i >= tl->size)
        return NULL;

    // Não há como pular para a posição i: caminha pelos next
    Node *n = tl->first;
    while (i > 0) {
        n = n->next;
        i--;
    }
    return n->team;
}

void tl_free(TeamList *tl) {
    if (tl == NULL)
        return;

    Node *n = tl->first;
    while (n != NULL) {
        Node *next = n->next;  // guarda o próximo antes de liberar o atual
        team_free(n->team);
        free(n);
        n = next;
    }
    free(tl);
}
