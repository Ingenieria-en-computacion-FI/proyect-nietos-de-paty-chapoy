#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


#define FILAS 22
#define COLUMNAS 28
#define TAM_CASILLA 16
#define WINDOW_WIDTH (COLUMNAS * TAM_CASILLA)
#define ALTO_MARCADOR 32
#define WINDOW_HEIGHT ((FILAS * TAM_CASILLA) + ALTO_MARCADOR)
#define MAX_FANTASMAS 6


// Matriz personalizada (0 = Pellet, 2 = Vacío, 3 = Poder, 4 = Fresa, 5 = Llave Mala, 6 = Explosión)
int mapa[FILAS][COLUMNAS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 3, 0, 2, 0, 0, 1, 0, 2, 0, 2, 0, 1, 0, 2, 1, 0, 2, 0, 2, 0, 1, 0, 2, 0, 2, 3, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 2, 0, 2, 0, 2, 0, 2, 2, 0, 2, 0, 2, 0, 2, 0, 2, 2, 0, 2, 0, 2, 0, 2, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 2, 0, 0, 2, 1, 0, 2, 0, 2, 0, 1, 1, 1, 1, 0, 0, 2, 0, 2, 1, 0, 2, 0, 2, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
    {0, 0, 2, 0, 2, 0, 2, 1, 1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 1, 1, 2, 0, 2, 0, 2, 0, 0},
    {1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1},
    {0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 1, 0, 2, 0, 2, 0, 2, 1, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0},
    {1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1},
    {0, 0, 2, 0, 2, 0, 2, 1, 1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 1, 1, 0, 2, 0, 2, 0, 2, 0},
    {1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1},
    {1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 0, 2, 0, 2, 0, 2, 1},
    {1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1},
    {1, 0, 2, 0, 1, 0, 2, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 2, 1, 0, 2, 0, 1},
    {1, 1, 1, 0, 1, 0, 0, 1, 1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 1, 1, 2, 0, 1, 0, 1, 1, 1},
    {1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0, 2, 0, 2, 0, 2, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 3, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 1, 1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 3, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};


int mapaOriginal[FILAS][COLUMNAS];


SDL_Texture* texturaHojaSprites = NULL;


SDL_Rect cajasFantasmasSrc[MAX_FANTASMAS] = {
    { 401, 83, 16, 16 }, // Fantasma Rojo
    { 418, 83, 16, 16 }, // Fantasma Rosa
    { 435, 83, 16, 16 }, // Fantasma Naranja
    { 452, 83, 16, 16 }, // Fantasma Cian
    { 401, 83, 16, 16 },
    { 418, 83, 16, 16 }  
};


SDL_Rect spritesItems[7] = {
    { 618, 489, 16, 16 }, // 0 = Fresa
    { 401, 489, 16, 16 },
    { 835, 489, 16, 16 },
    { 69, 675, 16, 16 },  // 3 = Llave (Ítem malo)
    { 852, 489, 16, 16 },
    { 503, 540, 16, 16 }, // 5 = Píldora de poder
    { 120, 168, 16, 16 }  // 6 = Explosión
};


SDL_Rect frameFantasmaAzul[2] = { { 401, 168, 16, 16 }, { 418, 168, 16, 16 } };
SDL_Rect framesPacmanAnim[2] = { { 503, 337, 16, 16 }, { 503, 354, 16, 16 } };


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


// Variables globales para las Frutas Dinámicas
bool frutaVisible = false;
unsigned int tiempoProximaFruta = 0;
unsigned int tiempoDesaparecerFruta = 0;
int frutaFila = 10;  
int frutaColumna = 13;


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
            if (mapa[f][c] == 0 || mapa[f][c] == 3 || mapa[f][c] == 4 || mapa[f][c] == 5) {
                pelletsRestantes++;
            }
        }
    }
   
    frutaVisible = false;
    tiempoProximaFruta = SDL_GetTicks() + 10000 + (rand() % 8000);
   
    esperandoReinicio = true;
}


void dibujarNumero(SDL_Renderer *renderer, int numero, int x, int y) {
    char digitos[10][7] = {
        {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1}, {1,1,1,1,0,0,1}, {0,1,1,0,0,1,1},
        {1,0,1,1,0,1,1}, {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0}, {1,1,1,1,1,1,1}, {1,1,1,1,0,1,1}
    };


    int n = numero % 10;
    SDL_Rect segmentos[7] = {
        {x, y, 10, 3}, {x+7,y, 3, 10}, {x+7,y+8, 3, 10}, {x, y+16, 10, 3},
        {x, y+8, 3, 10}, {x, y, 3, 10}, {x, y+8, 10, 3}
    };


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 7; i++) {
        if (digitos[n][i] == 1) {
            SDL_RenderFillRect(renderer, &segmentos[i]);
        }
    }
}


void dibujarValor(SDL_Renderer *renderer, int valor, int x, int y) {
    int temp = valor;
    int digitos[5] = {0};
    for (int i = 4; i >= 0; i--) {
        digitos[i] = temp % 10;
        temp /= 10;
    }
    for (int i = 0; i < 5; i++) {
        dibujarNumero(renderer, digitos[i], x + (i * 14), y);
    }
}


int main(int argc, char *argv[]) {
    SDL_SetMainReady();
    srand((unsigned int)time(NULL));
    SDL_Init(SDL_INIT_VIDEO);


    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            mapaOriginal[f][c] = mapa[f][c];
        }
    }


    int anchoVentana = 28 * 16;
    int altoVentana = (22 * 16) + 40;
   
    SDL_Window *window = SDL_CreateWindow("Pac-Girls Ultimate Arcade FP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, anchoVentana, altoVentana, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


    IMG_Init(IMG_INIT_PNG);


    texturaHojaSprites = IMG_LoadTexture(renderer, "image_85babf.png");
    if (!texturaHojaSprites) {
        printf("Error: ¡No encontré el archivo image_85babf.png!\n");
    }


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


        if (modoLlaveAzul && currentTime > tiempoFinPoder) {
            modoLlaveAzul = false;
        }


        if (!esperandoReinicio && (currentTime > lastLogicTime + 16)) {
           
            // LÓGICA DE SPAWN ALEATORIO: 3 OPCIONES
            if (!frutaVisible && currentTime > tiempoProximaFruta) {
                int eleccion = rand() % 3;
                if (eleccion == 0) {
                    mapa[frutaFila][frutaColumna] = 4; // Fresa Buena
                } else if (eleccion == 1) {
                    mapa[frutaFila][frutaColumna] = 5; // Ítem Malo 1 (Llave)
                } else {
                    mapa[frutaFila][frutaColumna] = 6; // Ítem Malo 2 (Explosión)
                }
                frutaVisible = true;
                pelletsRestantes++;
                tiempoDesaparecerFruta = currentTime + 8000 + (rand() % 4000);
            }
            else if (frutaVisible && currentTime > tiempoDesaparecerFruta) {
                if (mapa[frutaFila][frutaColumna] == 4 || mapa[frutaFila][frutaColumna] == 5 || mapa[frutaFila][frutaColumna] == 6) {
                    mapa[frutaFila][frutaColumna] = 2;
                    pelletsRestantes--;
                }
                frutaVisible = false;
                tiempoProximaFruta = currentTime + 12000 + (rand() % 10000);
            }


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


                if (tipoCasilla == 0 || tipoCasilla == 3 || tipoCasilla == 4 || tipoCasilla == 5 || tipoCasilla == 6) {
                    mapa[pacmanFil][pacmanCol] = 2;
                    pelletsRestantes--;


                    if (tipoCasilla == 0) score += 10;
                    else if (tipoCasilla == 3) {
                        score += 20;
                        modoLlaveAzul = true;
                        tiempoFinPoder = currentTime + 10000;
                    }
                    else if (tipoCasilla == 4) {
                        score += 100;
                        frVisible: frutaVisible = false;
                        tiempoProximaFruta = currentTime + 15000 + (rand() % 10000);
                    }
                    else if (tipoCasilla == 5) { // INDUCE MÁS FANTASMAS
                        score -= 50;          
                        modoLlaveAzul = false;
                       
                        numFantasmasActivos += 2;
                        if (numFantasmasActivos > MAX_FANTASMAS) {
                            numFantasmasActivos = MAX_FANTASMAS;
                        }
                       
                        for (int i = 0; i < numFantasmasActivos; i++) {
                            fantasmas[i].x = (11 + (i % 3)) * TAM_CASILLA;
                            fantasmas[i].y = 4 * TAM_CASILLA;
                            fantasmas[i].vivo = true;
                        }


                        frutaVisible = false;
                        tiempoProximaFruta = currentTime + 15000 + (rand() % 10000);
                    }
                    else if (tipoCasilla == 6) { // RESTA 200 PUNTOS
                        score -= 200;
                        if (score < 0) score = 0;
                       
                        frutaVisible = false;
                        tiempoProximaFruta = currentTime + 15000 + (rand() % 10000);
                    }


                    if (pelletsRestantes <= 0) {
                        nivel++;
                        if (nivel > 6) nivel = 1;


                        for (int f = 0; f < FILAS; f++) {
                            for (int c = 0; c < COLUMNAS; c++) {
                                mapa[f][c] = mapaOriginal[f][c];
                            }
                        }
                        resetPosiciones();
                        esperandoReinicio = false;
                    }
                }
            }


            // Lógica Fantasmas
            for (int i = 0; i < numFantasmasActivos; i++) {
                if (!fantasmas[i].vivo) continue;


                if (fantasmas[i].x % TAM_CASILLA == 0 && fantasmas[i].y % TAM_CASILLA == 0) {
                    int dirX[4] = {1, -1, 0, 0};
                    int dirY[4] = {0, 0, 1, -1};
                    int opcionesValidas[4];
                    int contOpciones = 0;


                    for (int d = 0; d < 4; d++) {
                        if (dirX[d] == -fantasmas[i].dx && dirY[d] == -fantasmas[i].dy) continue;
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


                if (abs(pacman.x - fantasmas[i].x) < 12 && abs(pacman.y - fantasmas[i].y) < 12) {
                    if (modoLlaveAzul) {
                        score += 50;
                        fantasmas[i].x = 13 * TAM_CASILLA;
                        fantasmas[i].y = 4 * TAM_CASILLA;
                    } else {
                        lives--;
                        if (lives <= 0) gameOver = true;
                        else resetPosiciones();
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


        // 1. Dibujar Muros
        SDL_Rect rCasilla = {0, 0, TAM_CASILLA, TAM_CASILLA};
        for (int f = 0; f < FILAS; f++) {
            for (int c = 0; c < COLUMNAS; c++) {
                if (mapa[f][c] == 1) {
                    rCasilla.x = c * TAM_CASILLA;
                    rCasilla.y = f * TAM_CASILLA;
                   
                    if (nivel % 3 == 1) SDL_SetRenderDrawColor(renderer, 0, 90, 255, 255);
                    else if (nivel % 3 == 2) SDL_SetRenderDrawColor(renderer, 230, 40, 40, 255);
                    else SDL_SetRenderDrawColor(renderer, 40, 180, 40, 255);
                   
                    SDL_RenderFillRect(renderer, &rCasilla);
                }
            }
        }


        // 2. Dibujar Comestibles
        for (int f = 0; f < FILAS; f++) {
            for (int c = 0; c < COLUMNAS; c++) {
                SDL_Rect destItem = { c * TAM_CASILLA, f * TAM_CASILLA, TAM_CASILLA, TAM_CASILLA };
               
                if (mapa[f][c] == 0) {
                    SDL_SetRenderDrawColor(renderer, 240, 190, 20, 255);
                    SDL_Rect monedita = { (c * TAM_CASILLA) + 6, (f * TAM_CASILLA) + 2, 4, 12 };
                    SDL_RenderFillRect(renderer, &monedita);
                }
                else if (mapa[f][c] == 3) {
                    if (texturaHojaSprites) {
                        SDL_RenderCopy(renderer, texturaHojaSprites, &spritesItems[5], &destItem);
                    }
                }
                else if (mapa[f][c] == 4) {
                    if (texturaHojaSprites) {
                        SDL_RenderCopy(renderer, texturaHojaSprites, &spritesItems[0], &destItem);
                    }
                }
                else if (mapa[f][c] == 5) {
                    if (texturaHojaSprites) {
                        SDL_RenderCopy(renderer, texturaHojaSprites, &spritesItems[3], &destItem);
                    }
                }
                else if (mapa[f][c] == 6) {
                    if (texturaHojaSprites) {
                        SDL_RenderCopy(renderer, texturaHojaSprites, &spritesItems[6], &destItem);
                    }
                }
            }
        }


        // 3. Dibujar Fantasmas
        for (int i = 0; i < numFantasmasActivos; i++) {
            SDL_Rect destGhost = {fantasmas[i].x, fantasmas[i].y, TAM_CASILLA, TAM_CASILLA};
           
            if (texturaHojaSprites) {
                if (modoLlaveAzul) {
                    SDL_RenderCopy(renderer, texturaHojaSprites, &frameFantasmaAzul[fantasmas[i].currentFrame], &destGhost);
                } else {
                    SDL_RenderCopy(renderer, texturaHojaSprites, &cajasFantasmasSrc[fantasmas[i].idColor], &destGhost);
                }
            }
        }


        // 4. Dibujar Jugador
        SDL_Rect destPacman = {pacman.x, pacman.y, TAM_CASILLA, TAM_CASILLA};
        if (texturaHojaSprites) {
            SDL_RenderCopy(renderer, texturaHojaSprites, &framesPacmanAnim[pacman.currentFrame], &destPacman);
        }


        // 5. Dibujar Marcador
        int posYMarcador = 356;


        // Vidas dibujadas con el sprite
        for (int v = 0; v < lives; v++) {
            SDL_Rect destVida = { 16 + (v * 20), posYMarcador, 16, 16 };
            if (texturaHojaSprites) {
                SDL_RenderCopy(renderer, texturaHojaSprites, &framesPacmanAnim[0], &destVida);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 235, 0, 255);
                SDL_Rect cuadraditoVida = { 16 + (v * 20), posYMarcador + 2, 12, 12 };
                SDL_RenderFillRect(renderer, &cuadraditoVida);
            }
        }


        dibujarValor(renderer, score, 180, posYMarcador);


        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        int posXLevel = 380;
        SDL_Rect letraL_v = { posXLevel, posYMarcador, 3, 16 }; SDL_RenderFillRect(renderer, &letraL_v);
        SDL_Rect letraL_h = { posXLevel, posYMarcador + 14, 8, 3 }; SDL_RenderFillRect(renderer, &letraL_h);


        dibujarNumero(renderer, nivel, posXLevel + 16, posYMarcador);
       
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }


    if (texturaHojaSprites) {
        SDL_DestroyTexture(texturaHojaSprites);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
