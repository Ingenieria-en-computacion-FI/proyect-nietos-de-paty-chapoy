#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>


#define WDTH 20
#define HEIGHT 10
#define MAX_FANTASMAS 6


// Códigos de color ANSI
#define ANSI_AMARILLO  "\x1b[33m"
#define ANSI_BLANCO    "\x1b[37m"
#define ANSI_ROJO      "\x1b[31m"
#define ANSI_CIAN      "\x1b[36m"
#define ANSI_VERDE     "\x1b[32m"
#define ANSI_MAGENTA   "\x1b[35m"
#define ANSI_AZUL      "\x1b[34m"
#define ANSI_RESET     "\x1b[0m"


// Estructura para manejar múltiples fantasmas
typedef struct {
    int x;
    int y;
    char* color;
    bool vivo; // Para saber si Pac-Man ya se lo comió en modo supergema
} Fantasma;


// Variables globales del juego
int playerX, playerY;
int foodX, foodY;
int gemX, gemY;
bool gemActiva;
int score;
int lives;
int nivel;
int numFantasmas;
bool gameOver;


// Variables para el poder de la gema
bool modoSuperGema;
time_t tiempoFinPoder;


int map[HEIGHT][WDTH];
Fantasma fantasmas[MAX_FANTASMAS];


char* coloresFantasmas[MAX_FANTASMAS] = {
    ANSI_ROJO, ANSI_CIAN, ANSI_VERDE, ANSI_MAGENTA, ANSI_AZUL, ANSI_BLANCO
};


char capturarTecla() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;  
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ~ICANON");
    if (read(0, &buf, 1) < 0) perror("read()");
    old.c_lflag |= ICANON;  
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    return buf;
}


void generarLaberinto() {
    // La dificultad aumenta: a mayor nivel, más probabilidad de muros (Tope de 25%)
    int probabilidadMuro = 10 + (nivel * 2);
    if (probabilidadMuro > 25) probabilidadMuro = 25;


    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WDTH; j++) {
            if ((i == 0 && j == 0) || (i == HEIGHT-1 && j == WDTH-1)) {
                map[i][j] = 0;
            } else {
                map[i][j] = (rand() % 100 < probabilidadMuro) ? 1 : 0;
            }
        }
    }
}


void spawnearFantasma(int index) {
    do {
        fantasmas[index].x = rand() % WDTH;
        fantasmas[index].y = rand() % HEIGHT;
        fantasmas[index].color = coloresFantasmas[index];
        fantasmas[index].vivo = true;
    } while (map[fantasmas[index].y][fantasmas[index].x] == 1 ||
            (abs(fantasmas[index].x - playerX) < 3 && abs(fantasmas[index].y - playerY) < 3));
}


void spawnearGema() {
    // 40% de probabilidad de que aparezca una gema al cambiar de nivel o reestructurar
    if (rand() % 100 < 40) {
        do {
            gemX = rand() % WDTH;
            gemY = rand() % HEIGHT;
        } while (map[gemY][gemX] == 1 || (gemX == playerX && gemY == playerY) || (gemX == foodX && gemY == foodY));
        gemActiva = true;
    } else {
        gemActiva = false; // En este laberinto no tocó gema
    }
}


void setup() {
    srand(time(NULL));
    playerX = 0;
    playerY = 0;
    score = 0;
    lives = 3;
    nivel = 1;
    numFantasmas = 2;
    gameOver = false;
    modoSuperGema = false;
   
    generarLaberinto();
   
    do {
        foodX = rand() % WDTH;
        foodY = rand() % HEIGHT;
    } while (map[foodY][foodX] == 1 || (foodX == playerX && foodY == playerY));


    spawnearGema();


    for (int i = 0; i < numFantasmas; i++) {
        spawnearFantasma(i);
    }
}


void draw() {
    system("clear");
   
    // Actualizar el estado del poder basado en el tiempo
    if (modoSuperGema && time(NULL) >= tiempoFinPoder) {
        modoSuperGema = false;
    }


    printf(ANSI_BLANCO);
    for (int j = 0; j < WDTH + 2; j++) printf("#");
    printf("\n" ANSI_RESET);


    for (int i = 0; i < HEIGHT; i++) {
        printf(ANSI_BLANCO "#" ANSI_RESET);
        for (int j = 0; j < WDTH; j++) {
           
            int fantasmaAqui = -1;
            for (int k = 0; k < numFantasmas; k++) {
                if (fantasmas[k].y == i && fantasmas[k].x == j && fantasmas[k].vivo) {
                    fantasmaAqui = k;
                    break;
                }
            }


            if (i == playerY && j == playerX) {
                printf(ANSI_AMARILLO "P" ANSI_RESET);
            } else if (fantasmaAqui != -1) {
                if (modoSuperGema) {
                    printf(ANSI_AZUL "G" ANSI_RESET); // Si hay poder, se vuelven azules (vulnerables)
                } else {
                    printf("%sG" ANSI_RESET, fantasmas[fantasmaAqui].color);
                }
            } else if (i == foodY && j == foodX) {
                printf(ANSI_AMARILLO "." ANSI_RESET);
            } else if (gemActiva && i == gemY && j == gemX) {
                printf(ANSI_MAGENTA "*" ANSI_RESET); // Gema Morada
            } else if (map[i][j] == 1) {
                printf(ANSI_BLANCO "#" ANSI_RESET);  
            } else {
                printf(" ");
            }
        }
        printf(ANSI_BLANCO "#\n" ANSI_RESET);
    }


    printf(ANSI_BLANCO);
    for (int j = 0; j < WDTH + 2; j++) printf("#");
    printf("\n" ANSI_RESET);


    // Interfaz de información
    printf("Nivel: %d | Score: %d | Vidas: ", nivel, score);
    for(int v = 0; v < lives; v++) printf(ANSI_ROJO "<3 " ANSI_RESET);
   
    if (modoSuperGema) {
        printf(ANSI_AZUL "\n¡MODO SUPERGEMA ACTIVO! (%lds restantes)" ANSI_RESET, tiempoFinPoder - time(NULL));
    } else {
        printf("\nModo: Normal");
    }
   
    printf("\nFantasmas activos: %d/%d\n", numFantasmas, MAX_FANTASMAS);
    printf("Controles: W/A/S/D | P (Pausa) | X (Salir)\n");
}


void moverFantasmasIA() {
    for (int i = 0; i < numFantasmas; i++) {
        if (!fantasmas[i].vivo) continue; // Los fantasmas comidos no se mueven


        int nextX = fantasmas[i].x;
        int nextY = fantasmas[i].y;


        if (modoSuperGema) {
            // IA de Huida: Al revés, se alejan de Pac-Man
            if (fantasmas[i].x < playerX) nextX--;
            else if (fantasmas[i].x > playerX) nextX++;


            if (fantasmas[i].y < playerY) nextY--;
            else if (fantasmas[i].y > playerY) nextY++;
        } else {
            // IA de Persecución normal
            if (fantasmas[i].x < playerX) nextX++;
            else if (fantasmas[i].x > playerX) nextX--;


            if (fantasmas[i].y < playerY) nextY++;
            else if (fantasmas[i].y > playerY) nextY--;
        }


        // Validar movimiento
        if (nextX >= 0 && nextX < WDTH && nextY >= 0 && nextY < HEIGHT && map[nextY][nextX] != 1) {
            fantasmas[i].x = nextX;
            fantasmas[i].y = nextY;
        } else {
            // Movimiento aleatorio de respaldo si se quedan atorados
            int dirX = (rand() % 3) - 1;
            int dirY = (rand() % 3) - 1;
            if (fantasmas[i].x + dirX >= 0 && fantasmas[i].x + dirX < WDTH &&
                fantasmas[i].y + dirY >= 0 && fantasmas[i].y + dirY < HEIGHT &&
                map[fantasmas[i].y + dirY][fantasmas[i].x + dirX] != 1) {
                fantasmas[i].x += dirX;
                fantasmas[i].y += dirY;
            }
        }
    }
}


void resetMuerte() {
    lives--;
    if (lives <= 0) {
        gameOver = true;
    } else {
        playerX = 0;
        playerY = 0;
        modoSuperGema = false; // Pierde el poder si muere
        for (int i = 0; i < numFantasmas; i++) {
            spawnearFantasma(i);
        }
        printf(ANSI_ROJO "\n¡Auch! Perdiste una vida. Presiona cualquier tecla para continuar..." ANSI_RESET);
        capturarTecla();
    }
}


void inputFunc() {
    char input = capturarTecla();


    if (input == 'x' || input == 'X') {
        gameOver = true;
        printf("\n¡Juego terminado por el usuario!\n");
        return;
    }


    if (input == 'p' || input == 'P') {
        printf("\n--- JUEGO EN PAUSA --- \nPresiona cualquier tecla para continuar... ");
        capturarTecla();
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


    // Colisión con paredes exteriores o interiores
    if (nextX < 0 || nextX >= WDTH || nextY < 0 || nextY >= HEIGHT || map[nextY][nextX] == 1) {
        resetMuerte();
        return;
    }


    playerX = nextX;
    playerY = nextY;


    // Verificar colisión con fantasmas ANTES de que ellos se muevan
    for (int i = 0; i < numFantasmas; i++) {
        if (fantasmas[i].vivo && playerX == fantasmas[i].x && playerY == fantasmas[i].y) {
            if (modoSuperGema) {
                fantasmas[i].vivo = false; // ¡Te lo comes!
                score += 50;               // Más puntos por comer fantasmas
            } else {
                resetMuerte();
                return;
            }
        }
    }


    moverFantasmasIA();


    // Verificar colisión con fantasmas DESPUÉS de que ellos se muevan
    for (int i = 0; i < numFantasmas; i++) {
        if (fantasmas[i].vivo && playerX == fantasmas[i].x && playerY == fantasmas[i].y) {
            if (modoSuperGema) {
                fantasmas[i].vivo = false;
                score += 50;
            } else {
                resetMuerte();
                return;
            }
        }
    }


    // Si Pac-Man agarra la Gema
    if (gemActiva && playerX == gemX && playerY == gemY) {
        gemActiva = false;
        modoSuperGema = true;
        tiempoFinPoder = time(NULL) + 5; // Activo por los próximos 5 segundos
        score += 20;
    }


    // Si Pac-Man come la comida (Pasa de nivel)
    if (playerX == foodX && playerY == foodY) {
        score += 10;
        nivel++;


        if (numFantasmas < MAX_FANTASMAS) {
            numFantasmas++;
        }
       
        generarLaberinto();
        spawnearGema();


        do {
            foodX = rand() % WDTH;
            foodY = rand() % HEIGHT;
        } while (map[foodY][foodX] == 1 || (foodX == playerX && foodY == playerY) || (gemActiva && foodX == gemX && foodY == gemY));


        for (int i = 0; i < numFantasmas; i++) {
            spawnearFantasma(i);
        }
    }
}


int main() {
    setup();
    while (!gameOver) {
        draw();
        inputFunc();
    }
   
    printf(ANSI_ROJO "\n===============================\n");
    printf("  GAME OVER - Fin de la partida\n");
    printf("  Nivel alcanzado: %d\n");
    printf("  Tu puntuación final fue: %d\n", score);
    printf("===============================\n" ANSI_RESET);
   
    return 0;
}
