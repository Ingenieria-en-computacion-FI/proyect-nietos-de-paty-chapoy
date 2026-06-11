#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


#define WDTH 20
#define HEIGHT 10


// Códigos de color ANSI
#define ANSI_AMARILLO  "\x1b[33m"
#define ANSI_BLANCO    "\x1b[37m"
#define ANSI_ROJO      "\x1b[31m"
#define ANSI_CIAN      "\x1b[36m"
#define ANSI_RESET     "\x1b[0m"


// Variables del juego
int playerX, playerY;
int foodX, foodY;
int ghost1X, ghost1Y;
int ghost2X, ghost2Y;
int score;
bool gameOver;


// Matriz del laberinto
int map[HEIGHT][WDTH];


// Función para generar un laberinto aleatorio limpio
void generarLaberinto() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WDTH; j++) {
            // Un 15% de probabilidad de que una casilla sea muro interno
            // Evitamos poner muros en las esquinas iniciales para no encerrar a nadie
            if ((i == 0 && j == 0) || (i == HEIGHT-1 && j == WDTH-1)) {
                map[i][j] = 0;
            } else {
                map[i][j] = (rand() % 100 < 15) ? 1 : 0;
            }
        }
    }
}


void setup() {
    srand(time(NULL));
    playerX = 0;
    playerY = 0;
    score = 0;
    gameOver = false;
   
    generarLaberinto();
   
    // Posicionar comida
    do {
        foodX = rand() % WDTH;
        foodY = rand() % HEIGHT;
    } while (map[foodY][foodX] == 1 || (foodX == playerX && foodY == playerY));


    // Posicionar Fantasma 1 (Rojo) cerca del final del mapa
    do {
        ghost1X = rand() % WDTH;
        ghost1Y = rand() % HEIGHT;
    } while (map[ghost1Y][ghost1X] == 1 || (ghost1X == playerX && ghost1Y == playerY));


    // Posicionar Fantasma 2 (Cian)
    do {
        ghost2X = rand() % WDTH;
        ghost2Y = rand() % HEIGHT;
    } while (map[ghost2Y][ghost2X] == 1 || (ghost2X == playerX && ghost2Y == playerY) || (ghost2X == ghost1X && ghost2Y == ghost1Y));
}


void draw() {
    system("clear");
   
    // Dibujar borde superior (Blanco)
    printf(ANSI_BLANCO);
    for (int j = 0; j < WDTH + 2; j++) printf("#");
    printf("\n" ANSI_RESET);


    // Renderizado del mapa
    for (int i = 0; i < HEIGHT; i++) {
        printf(ANSI_BLANCO "#" ANSI_RESET); // Borde izquierdo
       
        for (int j = 0; j < WDTH; j++) {
            if (i == playerY && j == playerX) {
                printf(ANSI_AMARILLO "P" ANSI_RESET); // Pac-Man Amarillo
            } else if (i == ghost1Y && j == ghost1X) {
                printf(ANSI_ROJO "G" ANSI_RESET);     // Fantasma 1 Rojo
            } else if (i == ghost2Y && j == ghost2X) {
                printf(ANSI_CIAN "G" ANSI_RESET);     // Fantasma 2 Cian
            } else if (i == foodY && j == foodX) {
                printf(ANSI_AMARILLO "." ANSI_RESET); // Comida
            } else if (map[i][j] == 1) {
                printf(ANSI_BLANCO "#" ANSI_RESET);   // Muros Blancos
            } else {
                printf(" ");
            }
        }
       
        printf(ANSI_BLANCO "#\n" ANSI_RESET); // Borde derecho
    }


    // Dibujar borde inferior (Blanco)
    printf(ANSI_BLANCO);
    for (int j = 0; j < WDTH + 2; j++) printf("#");
    printf("\n" ANSI_RESET);


    printf("Score: %d\n", score);
    printf("Controles: W/A/S/D | P (Pausa) | X (Salir)\n");
}


void moverFantasmas() {
    // Dirección aleatoria para Fantasma 1 (-1, 0, o 1)
    int dirX1 = (rand() % 3) - 1;
    int dirY1 = (rand() % 3) - 1;
    int nextG1X = ghost1X + dirX1;
    int nextG1Y = ghost1Y + dirY1;


    // Si no choca con bordes ni muros, se mueve
    if (nextG1X >= 0 && nextG1X < WDTH && nextG1Y >= 0 && nextG1Y < HEIGHT) {
        if (map[nextG1Y][nextG1X] != 1) {
            ghost1X = nextG1X;
            ghost1Y = nextG1Y;
        }
    }


    // Dirección aleatoria para Fantasma 2
    int dirX2 = (rand() % 3) - 1;
    int dirY2 = (rand() % 3) - 1;
    int nextG2X = ghost2X + dirX2;
    int nextG2Y = ghost2Y + dirY2;


    if (nextG2X >= 0 && nextG2X < WDTH && nextG2Y >= 0 && nextG2Y < HEIGHT) {
        if (map[nextG2Y][nextG2X] != 1) {
            ghost2X = nextG2X;
            ghost2Y = nextG2Y;
        }
    }
}


void inputFunc() {
    char input;
    if (scanf(" %c", &input) == 1) {
        // Opción de Salir del juego
        if (input == 'x' || input == 'X') {
            gameOver = true;
            printf("\n¡Juego terminado por el usuario!\n");
            return;
        }


        // Opción de Pausa
        if (input == 'p' || input == 'P') {
            printf("\n--- JUEGO EN PAUSA --- \nPresiona cualquier letra y Enter para continuar: ");
            char pauseChar;
            scanf(" %c", &pauseChar);
            return;
        }


        int nextX = playerX;
        int nextY = playerY;


        switch (input) {
            case 'w': case 'W': nextY--; break;
            case 's': case 'S': nextY++; break;
            case 'a': case 'A': nextX--; break;
            case 'd': case 'D': nextX++; break;
            default: return;
        }


        // CONDICIÓN DE GAME OVER 1: Tocar bordes externos
        if (nextX < 0 || nextX >= WDTH || nextY < 0 || nextY >= HEIGHT) {
            gameOver = true;
            return;
        }


        // CONDICIÓN DE GAME OVER 2: Tocar muros internos
        if (map[nextY][nextX] == 1) {
            gameOver = true;
            return;
        }


        // Si pasó los filtros de Game Over, el movimiento es válido
        playerX = nextX;
        playerY = nextY;


        // Mover a los fantasmas después de que el jugador se mueva
        moverFantasmas();


        // CONDICIÓN DE GAME OVER 3: Chocar contra un fantasma
        if ((playerX == ghost1X && playerY == ghost1Y) || (playerX == ghost2X && playerY == ghost2Y)) {
            gameOver = true;
            return;
        }


        // Si Pac-Man come la comida
        if (playerX == foodX && playerY == foodY) {
            score += 10;
           
            // CAMBIO ALEATORIO DEL TABLERO
            generarLaberinto();


            // Respawnear comida, fantasma 1 y fantasma 2 en zonas seguras post-cambio
            do {
                foodX = rand() % WDTH;
                foodY = rand() % HEIGHT;
            } while (map[foodY][foodX] == 1 || (foodX == playerX && foodY == playerY));


            do {
                ghost1X = rand() % WDTH;
                ghost1Y = rand() % HEIGHT;
            } while (map[ghost1Y][ghost1X] == 1 || (ghost1X == playerX && ghost1Y == playerY));


            do {
                ghost2X = rand() % WDTH;
                ghost2Y = rand() % HEIGHT;
            } while (map[ghost2Y][ghost2X] == 1 || (ghost2X == playerX && ghost2Y == playerY));
        }
    }
}


int main() {
    setup();
    while (!gameOver) {
        draw();
        inputFunc();
    }
   
    // Pantalla de Game Over
    printf(ANSI_ROJO "\n===============================\n");
    printf("  GAME OVER - ¡Has chocado! \n");
    printf("  Tu puntuación final fue: %d\n", score);
    printf("===============================\n" ANSI_RESET);
   
    return 0;
}
