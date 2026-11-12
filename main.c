#include "team.h"
#include "teamlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Este arquivo não pode depender da implementação escolhida em config.mk:
// use apenas as funções de team.h e teamlist.h.

#define LINE_SIZE 256
#define NAME_WIDTH 15

// Lê o arquivo (uma linha por time, formato id,nome) e insere cada time na
// coleção, sem assumir quantidade fixa de times.
// Devolve 0 em caso de sucesso e -1 se o arquivo não puder ser aberto.
static int load_teams(const char *path, TeamList *tl) {
    FILE *f = fopen(path, "r");
    if (f == NULL)
        return -1;

    char line[LINE_SIZE];
    while (fgets(line, sizeof(line), f) != NULL) {
        // Remove \n e \r do final (aceita arquivos com quebra de linha do Windows)
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;  // ignora linhas em branco

        char *comma = strchr(line, ',');
        if (comma == NULL) {
            fprintf(stderr, "Linha ignorada (formato inválido): %s\n", line);
            continue;
        }
        *comma = '\0';  // separa "id" e "nome" na própria linha

        char *end;
        long id = strtol(line, &end, 10);
        if (end == line || *end != '\0') {
            fprintf(stderr, "Linha ignorada (id inválido): %s\n", line);
            continue;
        }

        Team *t = team_create((int)id, comma + 1);
        if (t == NULL) {
            fprintf(stderr, "Sem memória ao criar o time %ld\n", id);
            break;
        }
        tl_append(tl, t);
    }

    fclose(f);
    return 0;
}

// printf("%-15s") conta bytes, não letras, e os acentos em UTF-8 ocupam
// 2 bytes ou mais. Cada byte de continuação (10xxxxxx) é uma "coluna a menos"
// na tela, então o campo é alargado em um byte para cada um deles.
static int utf8_extra_bytes(const char *s) {
    int extra = 0;
    for (; *s != '\0'; s++) {
        if (((unsigned char)*s & 0xC0) == 0x80)
            extra++;
    }
    return extra;
}

static void print_table(TeamList *tl) {
    // Colunas (a partir da 0): ID=0, Time=4, V=20, E=24, D=28, GM=32, GS=36, S=40, PG=44
    printf("%-3s %-*s %-3s %-3s %-3s %-3s %-3s %-3s %s\n",
           "ID", NAME_WIDTH, "Time", "V", "E", "D", "GM", "GS", "S", "PG");

    for (int i = 0; i < tl_size(tl); i++) {
        Team *t = tl_get(tl, i);

        int wins = team_wins(t);
        int draws = team_draws(t);
        int losses = team_losses(t);
        int gm = team_goals_scored(t);
        int gs = team_goals_conceded(t);
        int sg = gm - gs;           // saldo de gols
        int pg = 3 * wins + draws;  // pontos ganhos

        const char *name = team_name(t);
        printf("%-3d %-*s %-3d %-3d %-3d %-3d %-3d %-3d %d\n",
               team_id(t), NAME_WIDTH + utf8_extra_bytes(name), name,
               wins, draws, losses, gm, gs, sg, pg);
    }
}

int main(void) {
    TeamList *tl = tl_create();
    if (tl == NULL) {
        fprintf(stderr, "Sem memória ao criar a coleção de times\n");
        return 1;
    }

    if (load_teams("teams.csv", tl) != 0) {
        fprintf(stderr, "Não foi possível abrir teams.csv\n");
        tl_free(tl);
        return 1;
    }

    print_table(tl);

    tl_free(tl);
    return 0;
}
