#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#define FILAS 28
#define COLUMNAS 28
#define TAM_CASILLA 16 // Cada casilla de la matriz mide 16x16 píxeles
#define WINDOW_WIDTH (COLUMNAS * TAM_CASILLA) // 448 píxeles
#define WINDOW_HEIGHT (FILAS * TAM_CASILLA)   // 448 píxeles
#define MAX_FANTASMAS 6


// Matriz lógica del Laberinto clásico (1 = Pared, 0 = Pasillo/Camino libre)
int mapa[FILAS][COLUMNAS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1},
    {0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0},
    {1,1,1,1,1,1,0,1,1,0,1,1,1,2,2,1,1,1,0,1,1,0,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,0,1,1,0,1,2,2,2,2,2,2,1,0,1,1,0,1,1,1,1,1,1},
    {0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,0,0,0,0,0},
    {1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,0,0,0,1},
    {1,1,1,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


typedef struct {
    int x, y;          // Posición en píxeles de pantalla
    int dx, dy;        // Dirección actual (-1, 0, 1)
    int nextDx, nextNextDy; // Buffer de siguiente movimiento
    int currentFrame;
} Player;


typedef struct {
    int x, y;
    int dx, dy;
    int currentFrame;
    SDL_Rect spriteSheetBox;
} Ghost;


Player pacman;
Ghost fantasmas[MAX_FANTASMAS];


int nivel = 1;
int numFantasmasActivos = 1;
int lives = 3;
bool esperandoReinicio = false;
bool gameOver = false;
unsigned int lastAnimTime = 0;
unsigned int lastLogicTime = 0;


// Recortes de los tableros (Cambiantes según nivel)
SDL_Rect mapasSrc[4] = {
    { 4,   4,   224, 288 }, // Nivel 1: Rosa
    { 232, 4,   224, 288 }, // Nivel 2: Verde agua
    { 460, 4,   224, 288 }, // Nivel 3: Naranja
    { 688, 4,   224, 288 }  // Nivel 4: Azul
};


// Cajas base de los fantasmas en 'fantasmasdepacman.png'
SDL_Rect cajasFantasmasSrc[MAX_FANTASMAS] = {
    { 0,   0,   224, 288 }, // Rojo
    { 228, 0,   224, 288 }, // Rosa
    { 456, 0,   224, 288 }, // Cian
    { 684, 0,   224, 288 }, // Naranja
    { 912, 0,   224, 288 }, // Verde
    { 0,   292, 224, 288 }  // Rojo Alternativo
};


// Cuadros interiores de animación de patitas (14x14 px)
SDL_Rect framesFantasmaAnim[2] = { { 3, 121, 14, 14 }, { 19, 121, 14, 14 } };


// NUEVO RECORTE: Pac-Man Amarillo Clásico en la hoja de fantasmas (Fila 1, Caja del Fantasma Rojo)
// Mapeamos frames de la animación de Pac-Man comiendo (Boca abierta / Boca cerrada)
SDL_Rect framesPacmanAnim[2] = {
    { 3,  16, 14, 14 }, // Boca Abierta mirando a la izquierda
    { 19, 16, 14, 14 }  // Boca cerrada / Círculo
};


// Función para verificar si una coordenada de la matriz es un muro sólido
bool esMuro(int xPixels, int yPixels) {
    int col = xPixels / TAM_CASILLA;
    int fil = yPixels / TAM_CASILLA;
   
    if (col < 0 || col >= COLUMNAS || fil < 0 || fil >= FILAS) return true;
    return (mapa[fil][col] == 1);
}


void resetPosiciones() {
    // Colocar a Pac-Man en una zona libre central en píxeles exactos alineados a la rejilla
    pacman.x = 13 * TAM_CASILLA;
    pacman.y = 10 * TAM_CASILLA;
    pacman.dx = 0; pacman.dy = 0;
    pacman.nextDx = 0; pacman.nextNextDy = 0;


    numFantasmasActivos = nivel;
    if (numFantasmasActivos > MAX_FANTASMAS) numFantasmasActivos = MAX_FANTASMAS;


    // Colocar fantasmas en la zona del centro (la casa de los fantasmas o pasillos libres)
    for (int i = 0; i < numFantasmasActivos; i++) {
        fantasmas[i].x = (11 + i) * TAM_CASILLA;
        fantasmas[i].y = 13 * TAM_CASILLA;
        fantasmas[i].dx = (rand() % 2 == 0) ? 1 : -1;
        fantasmas[i].dy = 0;
        fantasmas[i].spriteSheetBox = cajasFantasmasSrc[i];
    }
    esperandoReinicio = true;
}


int main(int argc, char **argv) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);


    SDL_Window *window = SDL_CreateWindow("Ms. Pac-Man Matriz Perfecta", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


    SDL_Texture *texTableros = IMG_LoadTexture(renderer, "tablerodepacman.png");
    SDL_Texture *texFantasmas = IMG_LoadTexture(renderer, "fantasmasdepacman.png");


    resetPosiciones();
    esperandoReinicio = false;


    bool running = true;
    SDL_Event event;


    while (running && !gameOver) {
        unsigned int currentTime = SDL_GetTicks();


        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_x: running = false; break;
                    case SDLK_SPACE: if (esperandoReinicio) esperandoReinicio = false; break;
                    case SDLK_n: // Forzar nivel para verificar color de tablero y fantasmas
                        nivel++; if (nivel > 6) nivel = 1;
                        resetPosiciones(); esperandoReinicio = false;
                        break;
                    // Controles con W, A, S, D asignando el búfer de dirección intencional
                    case SDLK_w: pacman.nextDx = 0;  pacman.nextNextDy = -1; break;
                    case SDLK_s: pacman.nextDx = 0;  pacman.nextNextDy = 1;  break;
                    case SDLK_a: pacman.nextDx = -1; pacman.nextNextDy = 0;  break;
                    case SDLK_d: pacman.nextDx = 1;  pacman.nextNextDy = 0;  break;
                }
            }
        }


        // --- FÍSICAS Y LOGICA DE MOVIMIENTO DE PASILLOS (Cada 16ms) ---
        if (!esperandoReinicio && (currentTime > lastLogicTime + 16)) {
           
            // Lógica Pac-Man: Solo permitir giros si está perfectamente alineado a la rejilla de pasillos
            if (pacman.x % TAM_CASILLA == 0 && pacman.y % TAM_CASILLA == 0) {
                // Si la dirección deseada en el búfer está libre, cambiar de rumbo
                if (!esMuro(pacman.x + pacman.nextDx * TAM_CASILLA, pacman.y + pacman.nextNextDy * TAM_CASILLA)) {
                    pacman.dx = pacman.nextDx;
                    pacman.dy = pacman.nextNextDy;
                }
            }


            // Mover a Pac-Man si no hay un muro al frente
            if (!esMuro(pacman.x + pacman.dx, pacman.y + pacman.dy)) {
                pacman.x += pacman.dx * 2; // Avanza a velocidad constante de 2px por frame
                pacman.y += pacman.dy * 2;
            }


            // Lógica Fantasmas: Movimiento continuo por pasillos y decisiones en encrucijadas
            for (int i = 0; i < numFantasmasActivos; i++) {
                if (fantasmas[i].x % TAM_CASILLA == 0 && fantasmas[i].y % TAM_CASILLA == 0) {
                    // Buscar direcciones disponibles que no sean muros
                    int dirX[4] = {1, -1, 0, 0};
                    int dirY[4] = {0, 0, 1, -1};
                    int opcionesValidas[4];
                    int contOpciones = 0;


                    for (int d = 0; d < 4; d++) {
                        // Evitar que el fantasma se dé la vuelta completa de golpe inmediatamente excepto si es necesario
                        if (dirX[d] == -fantasmas[i].dx && dirY[d] == -fantasmas[i].dy) continue;


                        if (!esMuro(fantasmas[i].x + dirX[d] * TAM_CASILLA, fantasmas[i].y + dirY[d] * TAM_CASILLA)) {
                            opcionesValidas[contOpciones] = d;
                            contOpciones++;
                        }
                    }


                    // Si encontró una encrucijada o esquina libre, escoge una al azar
                    if (contOpciones > 0) {
                        int seleccionada = opcionesValidas[rand() % contOpciones];
                        fantasmas[i].dx = dirX[seleccionada];
                        fantasmas[i].dy = dirY[seleccionada];
                    } else {
                        // Si se encuentra atrapado, permite dar la vuelta total
                        fantasmas[i].dx = -fantasmas[i].dx;
                        fantasmas[i].dy = -fantasmas[i].dy;
                    }
                }


                // Mover fantasmas de forma fluida
                fantasmas[i].x += fantasmas[i].dx * 2;
                fantasmas[i].y += fantasmas[i].dy * 2;


                // --- DETECCIÓN DE COLISIÓN (Pac-Man vs Fantasma) ---
                // Si la distancia en píxeles es muy corta, se considera impacto
                if (abs(pacman.x - fantasmas[i].x) < 12 && abs(pacman.y - fantasmas[i].y) < 12) {
                    lives--;
                    if (lives <= 0) {
                        gameOver = true;
                    } else {
                        resetPosiciones();
                    }
                }
            }
            lastLogicTime = currentTime;
        }


        // --- TIEMPO DE ANIMACIÓN DE SPRITES (Cada 150ms) ---
        if (currentTime > lastAnimTime + 150) {
            pacman.currentFrame = (pacman.currentFrame + 1) % 2;
            for (int i = 0; i < numFantasmasActivos; i++) {
                fantasmas[i].currentFrame = (fantasmas[i].currentFrame + 1) % 2;
            }
            lastAnimTime = currentTime;
        }


        // =============================================================================
        // RENDERIZADO GRÁFICO (DIBUJO)
        // =============================================================================
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        // A. Dibujar fondo de tablero según nivel
        int mapaActualIdx = (nivel - 1) % 4;
        SDL_Rect destMapa = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderCopy(renderer, texTableros, &mapasSrc[mapaActualIdx], &destMapa);


        // B. Dibujar Fantasmas dentro del tablero
        for (int i = 0; i < numFantasmasActivos; i++) {
            SDL_Rect srcGhost = {
                fantasmas[i].spriteSheetBox.x + framesFantasmaAnim[fantasmas[i].currentFrame].x,
                fantasmas[i].spriteSheetBox.y + framesFantasmaAnim[fantasmas[i].currentFrame].y,
                framesFantasmaAnim[fantasmas[i].currentFrame].w,
                framesFantasmaAnim[fantasmas[i].currentFrame].h
            };
            SDL_Rect destGhost = { fantasmas[i].x, fantasmas[i].y, TAM_CASILLA, TAM_CASILLA };
            SDL_RenderCopy(renderer, texFantasmas, &srcGhost, &destGhost);
        }


        // C. Dibujar a Pac-Man Amarillo Clásico animado (Tomado de la hoja de fantasmas)
        SDL_Rect srcPacman = framesPacmanAnim[pacman.currentFrame];
        SDL_Rect destPacman = { pacman.x, pacman.y, TAM_CASILLA, TAM_CASILLA };
       
        // Rotación visual de Pacman según hacia donde camina
        double angulo = 0;
        if (pacman.dx == 1)  angulo = 180; // Voltear derecha
        if (pacman.dy == -1) angulo = 90;  // Voltear arriba
        if (pacman.dy == 1)  angulo = 270; // Voltear abajo


        SDL_RenderCopyEx(renderer, texFantasmas, &srcPacman, &destPacman, angulo, NULL, SDL_FLIP_NONE);


        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }


    SDL_DestroyTexture(texTableros);
    SDL_DestroyTexture(texFantasmas);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
