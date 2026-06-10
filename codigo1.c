#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


#define WDTH 20
#define HEIGHT 10


// Posición del jugador (Pac-Man)
int playerX, playerY;
// Posición de la comida
int foodX, foodY;
int score;


// Laberinto estático: 0 = Vacío, 1 = Muro interno
// Dimensiones: HEIGHT (filas) x WDTH (columnas)
int map[HEIGHT][WDTH] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,0,1,1,1,1,0,1,1,0,1,1,1,1,0,1,1,0},
    {0,1,1,0,1,1,1,1,0,1,1,0,1,1,1,1,0,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,0,1,1,0,1,1,1,1,1,1,0,1,1,0,1,1,0},
    {0,1,1,0,1,1,0,1,1,1,1,1,1,0,1,1,0,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,0,1,1,1,1,0,1,1,0,1,1,1,1,0,1,1,0},
    {0,1,1,0,1,1,1,1,0,1,1,0,1,1,1,1,0,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};


void setup() {
    srand(time(NULL)); // Inicializa la semilla para números aleatorios reales
   
    // Posición inicial de Pac-Man (en una zona sin muros)
    playerX = 0;
    playerY = 0;
    score = 0;
   
    // Generar comida inicial en una posición libre
    do {
        foodX = rand() % WDTH;
        foodY = rand() % HEIGHT;
    } while (map[foodY][foodX] == 1 || (foodX == playerX && foodY == playerY));
}


void draw() {
    // Limpia la pantalla (puedes usar "cls" en Windows si "clear" no funciona)
    system("clear");
   
    // Dibujar borde superior externo
    for (int j = 0; j < WDTH + 2; j++) printf("#");
    printf("\n");


    // Recorrer filas (i = Y) y columnas (j = X)
    for (int i = 0; i < HEIGHT; i++) {
        printf("#"); // Borde izquierdo externo
       
        for (int j = 0; j < WDTH; j++) {
            if (i == playerY && j == playerX) {
                printf("P"); // Pac-Man
            } else if (i == foodY && j == foodX) {
                printf("."); // Comida (Pac-dot)
            } else if (map[i][j] == 1) {
                printf("#"); // Muros del laberinto
            } else {
                printf(" "); // Espacio vacío
            }
        }
       
        printf("#\n"); // Borde derecho externo y salto de línea
    }


    // Dibujar borde inferior externo
    for (int j = 0; j < WDTH + 2; j++) printf("#");
    printf("\n");


    printf("Score: %d\n", score);
    printf("Controles: W (Arriba) | S (Abajo) | A (Izquierda) | D (Derecha)\n");
}


void inputFunc() {
    char input;
    // Captura el movimiento del usuario (requiere presionar Enter)
    if (scanf(" %c", &input) == 1) {
        int nextX = playerX;
        int nextY = playerY;


        // Calcular siguiente movimiento teórico
        switch (input) {
            case 'w': case 'W': nextY--; break;
            case 's': case 'S': nextY++; break;
            case 'a': case 'A': nextX--; break;
            case 'd': case 'D': nextX++; break;
            default: return; // Si es otra tecla, no hace nada
        }


        // Verificar colisión con bordes externos del mapa
        if (nextX >= 0 && nextX < WDTH && nextY >= 0 && nextY < HEIGHT) {
            // Verificar si el siguiente paso no es un muro interno
            if (map[nextY][nextX] != 1) {
                playerX = nextX;
                playerY = nextY;
            }
        }


        // Verificar si Pac-Man se come la comida
        if (playerX == foodX && playerY == foodY) {
            score += 10;
            // Respawnear la comida en una posición válida
            do {
                foodX = rand() % WDTH;
                foodY = rand() % HEIGHT;
            } while (map[foodY][foodX] == 1 || (foodX == playerX && foodY == playerY));
        }
    }
}


int main() {
    setup();
    while (true) {
        draw();
        inputFunc();
    }
    return 0;
}


