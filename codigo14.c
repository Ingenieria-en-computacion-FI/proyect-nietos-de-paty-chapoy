#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


#define FILAS 22
#define COLUMNAS 28
#define TAM_CASILLA 16
#define WINDOW_WIDTH (COLUMNAS * TAM_CASILLA)
#define WINDOW_HEIGHT (FILAS * TAM_CASILLA)
#define MAX_FANTASMAS 6


// Matriz configurada:
// 1 = Muro, 0 = Pellet normal, 3 = Pellet de Poder, 4 = Diamante Rosa, 2 = Vacío
int mapa[FILAS][COLUMNAS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 3, 0, 2, 0, 0, 1, 0, 2, 0, 2, 0, 1, 0, 2, 1, 0, 2, 0, 2, 0, 1, 0, 2, 0, 2, 3, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 2, 0, 2, 0, 2, 0, 2, 4, 0, 2, 0, 2, 0, 2, 0, 2, 4, 0, 2, 0, 2, 0, 2, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 2, 0, 0, 2, 1, 0, 2, 0, 2, 0, 1, 1, 1, 1, 0, 0, 2, 0, 2, 1, 0, 2, 0, 2, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
    {2, 0, 2, 0, 2, 0, 2, 1, 1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 1, 1, 2, 0, 2, 0, 2, 0, 1},
    {1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1},
    {1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 1, 0, 2, 0, 2, 0, 2, 1, 0, 2, 0, 2, 0, 2, 0, 2, 0, 1},
    {1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1},
    {2, 0, 2, 0, 2, 0, 2, 1, 1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 1, 1, 0, 2, 0, 2, 0, 2, 1},
    {1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1},
    {1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 0, 2, 0, 2, 0, 2, 1},
    {1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1},
    {1, 0, 2, 0, 1, 0, 2, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 2, 1, 0, 2, 0, 1},
    {1, 1, 1, 0, 1, 0, 0, 1, 1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 1, 1, 2, 0, 1, 0, 1, 1, 1},
    {1, 2, 0, 2, 0, 2, 0, 2, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 2, 0, 2, 0, 2, 0, 2, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 3, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 1, 1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 3, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};


typedef struct {
    int x, y;
    int dx, dy;
    int nextDx, nextDy;
    int currentFrame;
} Player;


typedef struct {
    int x, y;
    int dx, dy;
    int currentFrame;
    int idColor;
    bool vivo;
} Ghost;


Player pacman;
Ghost fantasmas[MAX_FANTASMAS];


int nivel = 1;
int numFantasmasActivos = 1;
int score = 0;
int lives = 3;
int pelletsRestantes = 0;


bool juegoEnPausa = false;
bool esperandoReinicio = false;
bool gameOver = false;
bool modoLlaveAzul = false;
unsigned int tiempoFinPoder = 0;


unsigned int lastAnimTime = 0;
unsigned int lastLogicTime = 0;


bool esMuro(int xPixels, int yPixels) {
    int esquinasX[4] = {xPixels, xPixels + TAM_CASILLA - 1, xPixels, xPixels + TAM_CASILLA - 1};
    int esquinasY[4] = {yPixels, yPixels, yPixels + TAM_CASILLA - 1, yPixels + TAM_CASILLA - 1};


    for (int i = 0; i < 4; i++) {
        int col = esquinasX[i] / TAM_CASILLA;
        int fil = esquinasY[i] / TAM_CASILLA;
        if (col < 0 || col >= COLUMNAS || fil < 0 || fil >= FILAS)
            return true;
        if (mapa[fil][col] == 1)
            return true;
    }
    return false;
}


void resetPosiciones() {
    pacman.x = 13 * TAM_CASILLA;
    pacman.y = 10 * TAM_CASILLA;
    pacman.dx = 0;
    pacman.dy = 0;
    pacman.nextDx = 0;
    pacman.nextDy = 0;


    numFantasmasActivos = nivel;
    if (numFantasmasActivos > MAX_FANTASMAS)
        numFantasmasActivos = MAX_FANTASMAS;


    for (int i = 0; i < numFantasmasActivos; i++) {
        fantasmas[i].x = (11 + i) * TAM_CASILLA;
        fantasmas[i].y = 4 * TAM_CASILLA;
        fantasmas[i].dx = (rand() % 2 == 0) ? 1 : -1;
        fantasmas[i].dy = 0;
        fantasmas[i].idColor = i;
        fantasmas[i].vivo = true;
    }


    pelletsRestantes = 0;
    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            if (mapa[f][c] == 0 || mapa[f][c] == 3 || mapa[f][c] == 4) {
                pelletsRestantes++;
            }
        }
    }
    esperandoReinicio = true;
}


int main(int argc, char *argv[]) {
    SDL_SetMainReady();
    srand((unsigned int)time(NULL));
    SDL_Init(SDL_INIT_VIDEO);


    SDL_Window *window = SDL_CreateWindow("Pac-Girl Ultimate Arcade", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


    resetPosiciones();
    esperandoReinicio = false;


    bool running = true;
    SDL_Event event;


    while (running && !gameOver) {
        unsigned int currentTime = SDL_GetTicks();


        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_x: running = false; break;
                case SDLK_p: juegoEnPausa = !juegoEnPausa; break;
                case SDLK_SPACE: if (esperandoReinicio) esperandoReinicio = false; break;
                case SDLK_w: pacman.nextDx = 0;  pacman.nextDy = -1; break;
                case SDLK_s: pacman.nextDx = 0;  pacman.nextDy = 1;  break;
                case SDLK_a: pacman.nextDx = -1; pacman.nextDy = 0;  break;
                case SDLK_d: pacman.nextDx = 1;  pacman.nextDy = 0;  break;
                }
            }
        }


        if (juegoEnPausa) {
            SDL_Delay(16);
            continue;
        }


        // Temporizador para el poder de los fantasmas azules (10 segundos)
        if (modoLlaveAzul && currentTime > tiempoFinPoder) {
            modoLlaveAzul = false;
        }


        if (!esperandoReinicio && (currentTime > lastLogicTime + 16)) {
           
            if (pacman.x % TAM_CASILLA == 0 && pacman.y % TAM_CASILLA == 0) {
                if (!esMuro(pacman.x + pacman.nextDx * 2, pacman.y + pacman.nextDy * 2)) {
                    pacman.dx = pacman.nextDx;
                    pacman.dy = pacman.nextDy;
                }
            }


            // [TÚNEL] Teletransportación
            if (pacman.x <= 0 && pacman.dx == -1) {
                pacman.x = (COLUMNAS - 1) * TAM_CASILLA - 2;
            }
            else if (pacman.x >= (COLUMNAS - 1) * TAM_CASILLA && pacman.dx == 1) {
                pacman.x = 2;
            }
            else if (esMuro(pacman.x + pacman.dx * 2, pacman.y + pacman.dy * 2)) {
                pacman.dx = 0;
                pacman.dy = 0;
            }
            else {
                pacman.x += pacman.dx * 2;
                pacman.y += pacman.dy * 2;
            }


            int pacmanCol = (pacman.x + TAM_CASILLA / 2) / TAM_CASILLA;
            int pacmanFil = (pacman.y + TAM_CASILLA / 2) / TAM_CASILLA;


            if (pacmanCol >= 0 && pacmanCol < COLUMNAS && pacmanFil >= 0 && pacmanFil < FILAS) {
                int tipoCasilla = mapa[pacmanFil][pacmanCol];


                if (tipoCasilla == 0 || tipoCasilla == 3 || tipoCasilla == 4) {
                    mapa[pacmanFil][pacmanCol] = 2; // Limpiar la casilla
                    pelletsRestantes--;


                    if (tipoCasilla == 0) {
                        score += 10; // Pellet normal
                    }
                    else if (tipoCasilla == 3) {
                        score += 20; // Pellet de Poder
                        modoLlaveAzul = true;
                        tiempoFinPoder = currentTime + 10000; // 10 segundos de inmunidad
                    }
                    else if (tipoCasilla == 4) {
                        score += 100;
                    }


                    // CONTROL DE CAMBIO DE NIVEL AUTOMÁTICO
                    if (pelletsRestantes <= 0) {
                        nivel++;
                        if (nivel > 6) nivel = 1;


                        // Restaurar los caminos a su estado original para el siguiente nivel
                        for (int f = 0; f < FILAS; f++) {
                            for (int c = 0; c < COLUMNAS; c++) {
                                if (mapa[f][c] == 2) {
                                    // Regenerar según la posición original del mapa
                                    if ((f == 1 && c == 1) || (f == 1 && c == 26) || (f == 20 && c == 1) || (f == 20 && c == 26)) {
                                        mapa[f][c] = 3; // Mantener los poderes en sus esquinas
                                    } else if ((f == 4 && c == 9) || (f == 4 && c == 18) || (f == 18 && c == 9) || (f == 18 && c == 18)) {
                                        mapa[f][c] = 4; // Mantener diamantes
                                    } else {
                                        mapa[f][c] = 0; // El resto pellets ordinarios
                                    }
                                }
                            }
                        }
                        resetPosiciones();
                        esperandoReinicio = false;
                    }
                }
            }


            // Lógica de los Fantasmas Activos
            for (int i = 0; i < numFantasmasActivos; i++) {
                if (!fantasmas[i].vivo)
                    continue;


                if (fantasmas[i].x % TAM_CASILLA == 0 && fantasmas[i].y % TAM_CASILLA == 0) {
                    int dirX[4] = {1, -1, 0, 0};
                    int dirY[4] = {0, 0, 1, -1};
                    int opcionesValidas[4];
                    int contOpciones = 0;


                    for (int d = 0; d < 4; d++) {
                        if (dirX[d] == -fantasmas[i].dx && dirY[d] == -fantasmas[i].dy)
                            continue;
                        if (!esMuro(fantasmas[i].x + dirX[d] * TAM_CASILLA, fantasmas[i].y + dirY[d] * TAM_CASILLA)) {
                            opcionesValidas[contOpciones] = d;
                            contOpciones++;
                        }
                    }


                    if (contOpciones > 0) {
                        int seleccionada = opcionesValidas[rand() % contOpciones];
                        fantasmas[i].dx = dirX[seleccionada];
                        fantasmas[i].dy = dirY[seleccionada];
                    } else {
                        fantasmas[i].dx = -fantasmas[i].dx;
                        fantasmas[i].dy = -fantasmas[i].dy;
                    }
                }


                // [TÚNEL] Para fantasmas
                if (fantasmas[i].x <= 0 && fantasmas[i].dx == -1) {
                    fantasmas[i].x = (COLUMNAS - 1) * TAM_CASILLA - 2;
                }
                else if (fantasmas[i].x >= (COLUMNAS - 1) * TAM_CASILLA && fantasmas[i].dx == 1) {
                    fantasmas[i].x = 2;
                }
                else {
                    fantasmas[i].x += fantasmas[i].dx * 2;
                    fantasmas[i].y += fantasmas[i].dy * 2;
                }


                // COLISIONES JUGADOR VS FANTASMA
                if (abs(pacman.x - fantasmas[i].x) < 12 && abs(pacman.y - fantasmas[i].y) < 12) {
                    if (modoLlaveAzul) {
                        // ¡Nos comemos al fantasma!
                        score += 50;
                        fantasmas[i].x = 13 * TAM_CASILLA; // Regresa temporalmente a casa
                        fantasmas[i].y = 4 * TAM_CASILLA;
                    } else {
                        // Pérdida de vida
                        lives--;
                        if (lives <= 0) {
                            gameOver = true;
                        } else {
                            resetPosiciones();
                        }
                    }
                }
            }
            lastLogicTime = currentTime;
        }


        if (currentTime > lastAnimTime + 150) {
            pacman.currentFrame = (pacman.currentFrame + 1) % 2;
            for (int i = 0; i < numFantasmasActivos; i++) {
                fantasmas[i].currentFrame = (fantasmas[i].currentFrame + 1) % 2;
            }
            lastAnimTime = currentTime;
        }


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        // 1. Dibujar Muros (Cambia de color según el nivel)
        SDL_Rect rCasilla = {0, 0, TAM_CASILLA, TAM_CASILLA};
        for (int f = 0; f < FILAS; f++) {
            for (int c = 0; c < COLUMNAS; c++) {
                if (mapa[f][c] == 1) {
                    rCasilla.x = c * TAM_CASILLA;
                    rCasilla.y = f * TAM_CASILLA;
                   
                    if (nivel % 3 == 1) SDL_SetRenderDrawColor(renderer, 0, 90, 255, 255);    // Azul
                    else if (nivel % 3 == 2) SDL_SetRenderDrawColor(renderer, 230, 40, 40, 255); // Rojo
                    else SDL_SetRenderDrawColor(renderer, 40, 180, 40, 255);                    // Verde
                   
                    SDL_RenderFillRect(renderer, &rCasilla);
                }
            }
        }


        // 2. Dibujar Comestibles activos en el mapa
        for (int f = 0; f < FILAS; f++) {
            for (int c = 0; c < COLUMNAS; c++) {
                if (mapa[f][c] == 0) {
                    // Pellet ordinario
                    SDL_SetRenderDrawColor(renderer, 240, 190, 20, 255);
                    SDL_Rect monedita = { (c * TAM_CASILLA) + 6, (f * TAM_CASILLA) + 2, 4, 12 };
                    SDL_RenderFillRect(renderer, &monedita);
                }
                else if (mapa[f][c] == 3) {
                    // Súper Pellet de Poder
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_Rect poder = { (c * TAM_CASILLA) + 4, (f * TAM_CASILLA) + 4, 8, 8 };
                    SDL_RenderFillRect(renderer, &poder);
                }
                else if (mapa[f][c] == 4) {
                    // Especial
                    SDL_SetRenderDrawColor(renderer, 255, 20, 147, 255);
                    SDL_Rect diamantito = { (c * TAM_CASILLA) + 4, (f * TAM_CASILLA) + 2, 8, 12 };
                    SDL_RenderFillRect(renderer, &diamantito);
                }
            }
        }


        // 3. Dibujar los Fantasmas
        for (int i = 0; i < numFantasmasActivos; i++) {
            SDL_Rect destGhost = {fantasmas[i].x, fantasmas[i].y, TAM_CASILLA, TAM_CASILLA};
           
            if (modoLlaveAzul) {
                SDL_SetRenderDrawColor(renderer, 30, 144, 255, 255);
            } else {
                if (fantasmas[i].idColor == 0) SDL_SetRenderDrawColor(renderer, 255, 60, 60, 255);
                else if (fantasmas[i].idColor == 1) SDL_SetRenderDrawColor(renderer, 255, 150, 200, 255);
                else if (fantasmas[i].idColor == 2) SDL_SetRenderDrawColor(renderer, 255, 130, 0, 255);
                else SDL_SetRenderDrawColor(renderer, 0, 255, 200, 255);
            }
            SDL_RenderFillRect(renderer, &destGhost);
        }


        // 4. Dibujar Jugador
        SDL_Rect destPacman = {pacman.x, pacman.y, TAM_CASILLA, TAM_CASILLA};
        SDL_SetRenderDrawColor(renderer, 255, 235, 0, 255);
        SDL_RenderFillRect(renderer, &destPacman);


        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }


    printf("\n===================================\n");
    printf("         PARTIDA TERMINADA\n");
    printf("===================================\n");
    printf("Puntaje Final: %d\n", score);
    printf("Nivel Alcanzado: %d\n", nivel);
    printf("===================================\n");


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
