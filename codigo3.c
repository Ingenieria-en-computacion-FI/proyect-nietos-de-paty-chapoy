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
} Fantasma;


// Variables globales del juego
int playerX, playerY;
int foodX, foodY;
int score;
int lives;
int nivel;
int numFantasmas;
bool gameOver;


int map[HEIGHT][WDTH];
Fantasma fantasmas[MAX_FANTASMAS];


// Arreglo de colores disponibles para los fantasmas
char* coloresFantasmas[MAX_FANTASMAS] = {
    ANSI_ROJO, ANSI_CIAN, ANSI_VERDE, ANSI_MAGENTA, ANSI_AZUL, ANSI_BLANCO
};


// Función para capturar un carácter inmediatamente sin esperar el Enter
char capturarTecla() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
    old.c_lflag &= ~ICANON; // Desactiva el modo canónico
    old.c_lflag &= ~ECHO;   // Desactiva el eco
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ~ICANON");
    if (read(0, &buf, 1) < 0) perror("read()");
    old.c_lflag |= ICANON;  // Devuelve la terminal a su estado original
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON"); // <-- Línea 59 corregida aquí
    return buf;
}


void generarLaberinto() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WDTH; j++) {
            if ((i == 0 && j == 0) || (i == HEIGHT-1 && j == WDTH-1)) {
                map[i][j] = 0;
            } else {
                map[i][j] = (rand() % 100 < 12) ? 1 : 0; // Un poco menos de muros para permitir el paso
            }
        }
    }
}


// Función para spawnear un fantasma individual en una posición segura aleatoria
void spawnearFantasma(int index) {
    do {
        fantasmas[index].x = rand() % WDTH;
        fantasmas[index].y = rand() % HEIGHT;
        fantasmas[index].color = coloresFantasmas[index];
    } while (map[fantasmas[index].y][fantasmas[index].x] == 1 ||
            (abs(fantasmas[index].x - playerX) < 3 && abs(fantasmas[index].y - playerY) < 3));
            // Evita que aparezcan encima o extremadamente cerca de Pac-Man al inicio
}


void setup() {
    srand(time(NULL));
    playerX = 0;
    playerY = 0;
    score = 0;
    lives = 3;
    nivel = 1;
    numFantasmas = 2; // Empezamos con 2 fantasmas en el nivel 1
    gameOver = false;
   
    generarLaberinto();
   
    // Posicionar comida
    do {
        foodX = rand() % WDTH;
        foodY = rand() % HEIGHT;
    } while (map[foodY][foodX] == 1 || (foodX == playerX && foodY == playerY));


    // Inicializar los primeros fantasmas
    for (int i = 0; i < numFantasmas; i++) {
        spawnearFantasma(i);
    }
}


void draw() {
    system("clear");
   
    // Dibujar borde superior (Blanco)
    printf(ANSI_BLANCO);
    for (int j = 0; j < WDTH + 2; j++) printf("#");
    printf("\n" ANSI_RESET);


    // Renderizado del mapa
    for (int i = 0; i < HEIGHT; i++) {
        printf(ANSI_BLANCO "#" ANSI_RESET);
        for (int j = 0; j < WDTH; j++) {
           
            // Verificar si hay un fantasma en esta posición
            int fantasmaAqui = -1;
            for (int k = 0; k < numFantasmas; k++) {
                if (fantasmas[k].y == i && fantasmas[k].x == j) {
                    fantasmaAqui = k;
                    break;
                }
            }


            if (i == playerY && j == playerX) {
                printf(ANSI_AMARILLO "P" ANSI_RESET); // Pac-Man Amarillo
            } else if (fantasmaAqui != -1) {
                printf("%sG" ANSI_RESET, fantasmas[fantasmaAqui].color); // Fantasma de su respectivo color
            } else if (i == foodY && j == foodX) {
                printf(ANSI_AMARILLO "." ANSI_RESET); // Comida
            } else if (map[i][j] == 1) {
                printf(ANSI_BLANCO "#" ANSI_RESET);   // Muros Blancos
            } else {
                printf(" ");
            }
        }
        printf(ANSI_BLANCO "#\n" ANSI_RESET);
    }


    // Dibujar borde inferior (Blanco)
    printf(ANSI_BLANCO);
    for (int j = 0; j < WDTH + 2; j++) printf("#");
    printf("\n" ANSI_RESET);


    // Interfaz de información
    printf("Nivel: %d | Score: %d | Vidas: ", nivel, score);
    for(int v = 0; v < lives; v++) printf(ANSI_ROJO "<3 " ANSI_RESET);
    printf("\nFantasmas activos: %d/%d\n", numFantasmas, MAX_FANTASMAS);
    printf("Controles: W/A/S/D | P (Pausa) | X (Salir)\n");
}


void moverFantasmasIA() {
    for (int i = 0; i < numFantasmas; i++) {
        int nextX = fantasmas[i].x;
        int nextY = fantasmas[i].y;


        // IA de persecución: el fantasma decide acortar la distancia con el jugador
        if (fantasmas[i].x < playerX) nextX++;
        else if (fantasmas[i].x > playerX) nextX--;


        if (fantasmas[i].y < playerY) nextY++;
        else if (fantasmas[i].y > playerY) nextY--;


        // Validar que el movimiento inteligente no choque con muros ni bordes
        if (nextX >= 0 && nextX < WDTH && nextY >= 0 && nextY < HEIGHT && map[nextY][nextX] != 1) {
            fantasmas[i].x = nextX;
            fantasmas[i].y = nextY;
        } else {
            // Si su camino directo está bloqueado por un muro, intenta un movimiento aleatorio para no quedarse trabado
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
        // Si aún le quedan vidas, regresa a Pac-Man al inicio de manera segura
        playerX = 0;
        playerY = 0;
        // Re-ubica a los fantasmas lejos para darle oportunidad de reaccionar
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


    // CONDICIÓN DE MUERTE 1: Tocar bordes externos o muros internos
    if (nextX < 0 || nextX >= WDTH || nextY < 0 || nextY >= HEIGHT || map[nextY][nextX] == 1) {
        resetMuerte();
        return;
    }


    // Si el paso es seguro, se mueve Pac-Man
    playerX = nextX;
    playerY = nextY;


    // Los fantasmas avanzan cazando al jugador
    moverFantasmasIA();


    // CONDICIÓN DE MUERTE 2: Si un fantasma te alcanza tras el turno
    for (int i = 0; i < numFantasmas; i++) {
        if (playerX == fantasmas[i].x && playerY == fantasmas[i].y) {
            resetMuerte();
            return;
        }
    }


    // Si Pac-Man se come la comida
    if (playerX == foodX && playerY == foodY) {
        score += 10;
        nivel++; // Avanza de nivel


        // Cada nivel añade un fantasma nuevo, hasta un tope de 6
        if (numFantasmas < MAX_FANTASMAS) {
            numFantasmas++;
        }
       
        // El tablero cambia por completo
        generarLaberinto();


        // Reposicionar de manera segura la comida y todos los fantasmas activos en el nuevo laberinto
        do {
            foodX = rand() % WDTH;
            foodY = rand() % HEIGHT;
        } while (map[foodY][foodX] == 1 || (foodX == playerX && foodY == playerY));


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
    printf("  GAME OVER - ¡Te has quedado sin vidas! \n");
    printf("  Nivel alcanzado: %d\n", nivel);
    printf("  Tu puntuación final fue: %d\n", score);
    printf("===============================\n" ANSI_RESET);
   
    return 0;
}
