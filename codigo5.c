#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

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

typedef struct {
    int x;
    int y;
    char* color;
    bool vivo;
} Fantasma;

// Variables globales del juego
int playerX, playerY;
int gemX, gemY;
bool gemActiva;
int score;
int lives;
int nivel;
int numFantasmas;
int comidaRestante; 
bool gameOver;

// Estado especial para evitar que se trabe en las muertes
bool esperandoReinicio; 

// Variables para el poder de la gema
bool modoSuperGema;
time_t tiempoFinPoder; 

int map[HEIGHT][WDTH]; // 0 = Comida, 1 = Muro, 2 = Vacío
Fantasma fantasmas[MAX_FANTASMAS];

char* coloresFantasmas[MAX_FANTASMAS] = {
    ANSI_ROJO, ANSI_CIAN, ANSI_VERDE, ANSI_MAGENTA, ANSI_AZUL, ANSI_BLANCO
};

// Configura la terminal para lectura inmediata
void modoNoBloqueante(bool activar) {
    struct termios ttystate;
    tcgetattr(0, &ttystate);
    if (activar) {
        ttystate.c_lflag &= ~ICANON; 
        ttystate.c_lflag &= ~ECHO;   
        ttystate.c_cc[VMIN] = 0;     
        ttystate.c_cc[VTIME] = 0;
    } else {
        ttystate.c_lflag |= ICANON;
        ttystate.c_lflag |= ECHO;
    }
    tcsetattr(0, TCSANOW, &ttystate);
}

// Revisa si hay teclas en el búfer sin detener el programa
int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

void generarLaberinto() {
    int probabilidadMuro = 10 + (nivel * 2); 
    if (probabilidadMuro > 22) probabilidadMuro = 22;

    comidaRestante = 0;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WDTH; j++) {
            if ((i == 0 && j == 0) || (i == HEIGHT-1 && j == WDTH-1)) {
                map[i][j] = 2; 
            } else {
                if (rand() % 100 < probabilidadMuro) {
                    map[i][j] = 1; 
                } else {
                    map[i][j] = 0; 
                    comidaRestante++;
                }
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
            (abs(fantasmas[index].x - playerX) < 4 && abs(fantasmas[index].y - playerY) < 4));
}

void spawnearGema() {
    if (rand() % 100 < 70) { 
        do {
            gemX = rand() % WDTH;
            gemY = rand() % HEIGHT;
        } while (map[gemY][gemX] == 1 || (gemX == playerX && gemY == playerY));
        gemActiva = true;
    } else {
        gemActiva = false;
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
    esperandoReinicio = false;
    
    modoNoBloqueante(true);
    generarLaberinto();
    spawnearGema();

    for (int i = 0; i < numFantasmas; i++) {
        spawnearFantasma(i);
    }
}

void draw() {
    system("clear"); 
    
    if (modoSuperGema && time(NULL) >= tiempoFinPoder) {
        modoSuperGema = false;
    }

    // Borde superior
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

            if (i == playerY && j == playerX && !esperandoReinicio) {
                printf(ANSI_AMARILLO "P" ANSI_RESET); 
            } else if (fantasmaAqui != -1) {
                if (modoSuperGema) {
                    printf(ANSI_AZUL "G" ANSI_RESET); 
                } else {
                    printf("%sG" ANSI_RESET, fantasmas[fantasmaAqui].color); 
                }
            } else if (gemActiva && i == gemY && j == gemX) {
                printf(ANSI_MAGENTA "*" ANSI_RESET); 
            } else if (map[i][j] == 0) {
                printf(ANSI_AMARILLO "." ANSI_RESET); 
            } else if (map[i][j] == 1) {
                printf(ANSI_BLANCO "#" ANSI_RESET);   
            } else {
                printf(" "); 
            }
        }
        printf(ANSI_BLANCO "#\n" ANSI_RESET); 
    }

    // Borde inferior
    printf(ANSI_BLANCO);
    for (int j = 0; j < WDTH + 2; j++) printf("#");
    printf("\n" ANSI_RESET);

    printf("Nivel: %d | Score: %d | Vidas: ", nivel, score);
    for(int v = 0; v < lives; v++) printf(ANSI_ROJO "<3 " ANSI_RESET);
    
    // Cambios dinámicos en la interfaz según el estado del juego
    if (esperandoReinicio) {
        printf(ANSI_ROJO "\n¡AUCH! PERDISTE UNA VIDA. Presiona ESPACIO para continuar..." ANSI_RESET);
    } else if (modoSuperGema) {
        printf(ANSI_AZUL "\n¡MODO SUPERGEMA! (%lds)" ANSI_RESET, tiempoFinPoder - time(NULL));
    } else {
        printf("\nModo: Normal");
    }
    printf(" | Puntitos restantes: %d\n", comidaRestante);
    printf("Controles: W/A/S/D | P (Pausa) | X (Salir)\n");
}

void moverFantasmasIA() {
    // Si estamos esperando que el jugador presione espacio, los fantasmas se quedan quietos
    if (esperandoReinicio) return;

    for (int i = 0; i < numFantasmas; i++) {
        if (!fantasmas[i].vivo) continue;

        int nextX = fantasmas[i].x;
        int nextY = fantasmas[i].y;

        if (rand() % 100 < 15) {
            int dirX = (rand() % 3) - 1;
            int dirY = (rand() % 3) - 1;
            if (fantasmas[i].x + dirX >= 0 && fantasmas[i].x + dirX < WDTH && 
                fantasmas[i].y + dirY >= 0 && fantasmas[i].y + dirY < HEIGHT && 
                map[fantasmas[i].y + dirY][fantasmas[i].x + dirX] != 1) {
                fantasmas[i].x += dirX;
                fantasmas[i].y += dirY;
            }
            continue;
        }

        if (modoSuperGema) {
            if (fantasmas[i].x < playerX) nextX--;
            else if (fantasmas[i].x > playerX) nextX++;
            if (fantasmas[i].y < playerY) nextY--;
            else if (fantasmas[i].y > playerY) nextY++;
        } else {
            if (fantasmas[i].x < playerX) nextX++;
            else if (fantasmas[i].x > playerX) nextX--;
            if (fantasmas[i].y < playerY) nextY++;
            else if (fantasmas[i].y > playerY) nextY--;
        }

        if (nextX >= 0 && nextX < WDTH && nextY >= 0 && nextY < HEIGHT && map[nextY][nextX] != 1) {
            fantasmas[i].x = nextX;
            fantasmas[i].y = nextY;
        }
    }
}

void resetMuerte() {
    lives--;
    if (lives <= 0) {
        gameOver = true;
    } else {
        // En lugar de un bucle "while" agresivo, activamos la bandera de espera segura
        esperandoReinicio = true; 
        playerX = 0;
        playerY = 0;
        modoSuperGema = false; 
        
        // Reubicar fantasmas de inmediato lejos de la esquina inicial
        for (int i = 0; i < numFantasmas; i++) {
            spawnearFantasma(i);
        }
    }
}

void actualizarJuego() {
    // 1. Manejo del estado cuando perdiste una vida
    if (esperandoReinicio) {
        if (kbhit()) {
            char input = getchar();
            while(kbhit()) getchar(); // Limpia ráfagas de botones

            // Si presiona la barra espaciadora (o cualquier tecla), el juego se reanuda suavemente
            if (input == ' ' || input == 'w' || input == 's' || input == 'a' || input == 'd' || input == 'W' || input == 'S' || input == 'A' || input == 'D') {
                esperandoReinicio = false;
            }
        }
        return; // Detiene el resto de la actualización (fantasmas e inputs) hasta que salga de la pausa
    }

    // 2. Movimiento normal de la IA si el juego está activo
    moverFantasmasIA();

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

    // 3. Procesar movimientos de Pac-Man
    if (kbhit()) {
        char input = getchar();
        while(kbhit()) getchar(); 

        if (input == 'x' || input == 'X') {
            gameOver = true;
            return;
        }

        if (input == 'p' || input == 'P') {
            printf("\n--- JUEGO EN PAUSA --- \nPresiona otra tecla para continuar... ");
            while(!kbhit());
            getchar();
            while(kbhit()) getchar(); 
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

        if (nextX < 0 || nextX >= WDTH || nextY < 0 || nextY >= HEIGHT || map[nextY][nextX] == 1) {
            resetMuerte();
            return;
        }

        playerX = nextX;
        playerY = nextY;

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

        if (map[playerY][playerX] == 0) {
            map[playerY][playerX] = 2; 
            score += 5;
            comidaRestante--;
        }

        if (gemActiva && playerX == gemX && playerY == gemY) {
            gemActiva = false;
            modoSuperGema = true;
            tiempoFinPoder = time(NULL) + 5;
            score += 20;
        }
    }

    if (comidaRestante <= 0) {
        nivel++;
        score += 100; 
        if (numFantasmas < MAX_FANTASMAS) {
            numFantasmas++;
        }
        generarLaberinto();
        spawnearGema();
        playerX = 0;
        playerY = 0;
        for (int i = 0; i < numFantasmas; i++) {
            spawnearFantasma(i);
        }
    }
}

int main() {
    setup();
    while (!gameOver) {
        draw();
        actualizarJuego();
        usleep(140000); 
    }
    
    modoNoBloqueante(false); 
    printf(ANSI_ROJO "\n===============================\n");
    printf("  GAME OVER - Fin de la partida\n");
    printf("  Nivel alcanzado: %d\n", nivel);
    printf("  Tu puntuación final fue: %d\n", score);
    printf("===============================\n" ANSI_RESET);
    
    return 0;
}