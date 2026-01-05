
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ROWS 6
#define COLS 7

/* Prototypes */
void init_grid(int g[ROWS][COLS]);
void print_grid(int g[ROWS][COLS]);
int drop_piece(int g[ROWS][COLS], int col, int player); /* retourne la ligne où le pion a été placé, ou -1 si impossible */
bool is_valid_column(int g[ROWS][COLS], int col);
bool is_grid_full(int g[ROWS][COLS]);
bool check_win(int g[ROWS][COLS], int last_row, int last_col, int player);
int get_player_move(int player);
int menu_choice(void);

int main(void) {
    int grid[ROWS][COLS];
    int choice;

    while (1) {
        choice = menu_choice();
        if (choice == 2) {
            printf("Au revoir !\n");
            break;
        } else if (choice == 1) {
            init_grid(grid);
            int current_player = 1;
            bool game_over = false;

            print_grid(grid);

            while (!game_over) {
                printf("Joueur %d, à vous de jouer.\n", current_player);
                int col = get_player_move(current_player);

                /* tenter de déposer la pièce */
                int row = drop_piece(grid, col, current_player);
                if (row == -1) {
                    printf("Colonne %d pleine ou invalide, réessayez.\n", col);
                    continue; /* rejouer le même joueur */
                }

                print_grid(grid);

                if (check_win(grid, row, col, current_player)) {
                    printf("Félicitations ! Le joueur %d a gagné !\n", current_player);
                    game_over = true;
                } else if (is_grid_full(grid)) {
                    printf("Match nul : la grille est pleine.\n");
                    game_over = true;
                } else {
                    /* alterner les joueurs */
                    current_player = (current_player == 1) ? 2 : 1;
                }
            }
        } else {
            printf("Choix invalide. Veuillez choisir 1 ou 2.\n");
        }
    }

    return 0;
}

/* Initialise la grille avec des 0 (cases vides) */
void init_grid(int g[ROWS][COLS]) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            g[r][c] = 0;
        }
    }
}

/* Affiche la grille en ASCII, avec numéro des colonnes (0..6) */
void print_grid(int g[ROWS][COLS]) {
    printf("\n  ");
    for (int c = 0; c < COLS; c++) {
        printf(" %d ", c);
    }
    printf("\n +");
    for (int c = 0; c < COLS; c++) printf("---");
    printf("+\n");

    for (int r = 0; r < ROWS; r++) {
        printf(" |");
        for (int c = 0; c < COLS; c++) {
            if (g[r][c] == 0) printf(" . ");
            else if (g[r][c] == 1) printf(" X ");
            else if (g[r][c] == 2) printf(" O ");
            else printf(" ? ");
        }
        printf("|\n");
    }

    printf(" +");
    for (int c = 0; c < COLS; c++) printf("---");
    printf("+\n\n");
}

/* Vérifie si une colonne est valide (index correct et pas pleine) */
bool is_valid_column(int g[ROWS][COLS], int col) {
    if (col < 0 || col >= COLS) return false;
    return (g[0][col] == 0); /* si la case du dessus est libre, la colonne accepte un pion */
}

/* Dépose un pion du joueur dans la colonne col.
   Retourne la ligne (0..ROWS-1) où le pion est placé, ou -1 si impossible. */
int drop_piece(int g[ROWS][COLS], int col, int player) {
    if (!is_valid_column(g, col)) return -1;
    for (int r = ROWS - 1; r >= 0; r--) {
        if (g[r][col] == 0) {
            g[r][col] = player;
            return r;
        }
    }
    return -1; /* ne devrait pas arriver si is_valid_column true */
}

/* Vérifie si la grille est pleine (aucune case 0) */
bool is_grid_full(int g[ROWS][COLS]) {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (g[r][c] == 0) return false;
    return true;
}

/* Vérifie si le joueur a aligné 4 pions en utilisant la dernière position jouée
   Nous comptons dans chaque direction (horiz, vert, diag) les pions consécutifs */
bool check_win(int g[ROWS][COLS], int last_row, int last_col, int player) {
    if (last_row < 0 || last_col < 0) return false;

    const int directions[4][2] = {
        {0, 1},  /* horizontal */
        {1, 0},  /* vertical */
        {1, 1},  /* diagonale descendante (\) */
        {1, -1}  /* diagonale montante (/) */
    };

    for (int d = 0; d < 4; d++) {
        int dr = directions[d][0];
        int dc = directions[d][1];
        int count = 1; /* inclure la case dernière jouée */

        /* compter dans la direction positive */
        int r = last_row + dr;
        int c = last_col + dc;
        while (r >= 0 && r < ROWS && c >= 0 && c < COLS && g[r][c] == player) {
            count++;
            r += dr;
            c += dc;
        }

        /* compter dans la direction négative */
        r = last_row - dr;
        c = last_col - dc;
        while (r >= 0 && r < ROWS && c >= 0 && c < COLS && g[r][c] == player) {
            count++;
            r -= dr;
            c -= dc;
        }

        if (count >= 4) return true;
    }

    return false;
}

/* Lit et valide le choix de colonne du joueur (0..6).
   Réessaye en cas d'entrée invalide (non-numérique ou hors plage). */
int get_player_move(int player) {
    char buf[128];
    long val;
    char *endptr;

    while (1) {
        printf("Entrez le numéro de colonne (0-%d) : ", COLS - 1);
        if (!fgets(buf, sizeof(buf), stdin)) {
            /* EOF ou erreur d'entrée -> quitter proprement */
            printf("\nErreur d'entrée. Fin du programme.\n");
            exit(EXIT_FAILURE);
        }

        /* enlever le '\n' à la fin si présent */
        size_t len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';

        /* conversion en nombre entier */
        val = strtol(buf, &endptr, 10);
        if (endptr == buf || *endptr != '\0') {
            printf("Entrée invalide : veuillez entrer un entier.\n");
            continue;
        }
        if (val < 0 || val >= COLS) {
            printf("Colonne hors plage : choisissez entre 0 et %d.\n", COLS - 1);
            continue;
        }
        return (int)val;
    }
}

/* Affiche le menu principal et retourne le choix :
   1 = Lancer une partie, 2 = Quitter */
int menu_choice(void) {
    char buf[128];
    while (1) {
        printf("=== Puissance 4 (console) ===\n");
        printf("1) Lancer une partie\n");
        printf("2) Quitter\n");
        printf("Choix : ");

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("\nErreur d'entrée. Fin du programme.\n");
            exit(EXIT_FAILURE);
        }

        if (buf[0] == '1' && (buf[1] == '\n' || buf[1] == '\0')) return 1;
        if (buf[0] == '2' && (buf[1] == '\n' || buf[1] == '\0')) return 2;

        printf("Choix invalide. Veuillez taper 1 ou 2.\n\n");
    }
}