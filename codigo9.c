#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#define FILAS 22
#define COLUMNAS 28
#define TAM_CASILLA 16
#define WINDOW_WIDTH (COLUMNAS * TAM_CASILLA) // 448 px
#define WINDOW_HEIGHT (FILAS * TAM_CASILLA)   // 352 px
#define MAX_FANTASMAS 6


// Matriz del Laberinto clásica (1 = Pared, 0 = Pasillo)
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
    {1,1,1,1,1,1,0,1,1,0,1,1,1,0,0,1,1,1,0,1,1,0,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1},
    {0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0},
    {1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1},
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
    int x, y;          
    int dx, dy;        
    int nextDx, nextDy;
    int currentFrame;
} Player;


typedef struct {
    int x, y;
    int dx, dy;
    int currentFrame;
    SDL_Rect spriteSheetBox;
    bool vivo;
} Ghost;


typedef struct {
    int x, y;
    int tipo; // 0:Fresa, 1:Cereza, 2:Durazno, 3:Melon, 4:Flor, 5:Campana, 6:Llave Azul
    int puntos;
    bool activa;
    unsigned int tiempoSpawn;
} Item;


Player pacman;
Ghost fantasmas[MAX_FANTASMAS];
Item itemActual;


int nivel = 1;
int numFantasmasActivos = 1;
int score = 0;
int lives = 3;
bool juegoEnPausa = false;
bool esperandoReinicio = false;
bool gameOver = false;


// Variables de poder de la llave azul
bool modoLlaveAzul = false;
unsigned int tiempoFinPoder = 0;


unsigned int lastAnimTime = 0;
unsigned int lastLogicTime = 0;


// Orígenes de corte de laberintos de 'tablerodepacman.png'
SDL_Rect mapasSrc[4] = {
    { 4,   4,   224, 288 }, // Nivel 1: Rosa
    { 232, 4,   224, 288 }, // Nivel 2: Verde agua
    { 460, 4,   224, 288 }, // Nivel 3: Naranja
    { 688, 4,   224, 288 }  // Nivel 4: Azul
};


// Cajas base para capturar las paletas de fantasmas en 'image_85babf.png'
SDL_Rect cajasFantasmasSrc[MAX_FANTASMAS] = {
    { 140, 115, 150, 150 }, // Fila 1 - Fantasma Gris/Lila
    { 140, 132, 150, 150 }, // Fila 2 - Fantasma Naranja
    { 140, 150, 150, 150 }, // Fila 3 - Fantasma Azul Cielo
    { 0,   168, 150, 150 }, // Fila 4 - Fantasma Rojo
    { 140, 115, 150, 150 }, // Reutilizados para nivel 5 y 6
    { 140, 132, 150, 150 }
};


// Coordenadas relativas de las frutas de la fila inferior en 'image_85babf.png'
SDL_Rect spritesItems[7] = {
    { 343, 173, 16, 16 }, // 0: Fresa (5 pts)
    { 382, 173, 16, 16 }, // 1: Cereza (3 pts)
    { 421, 173, 16, 16 }, // 2: Durazno (8 pts)
    { 460, 173, 16, 16 }, // 3: Melon (12 pts)
    { 499, 173, 16, 16 }, // 4: Flor (15 pts)
    { 538, 173, 16, 16 }, // 5: Campana (20 pts)
    { 577, 173, 16, 16 }  // 6: Llave Azul (Poder Especial)
};


int puntosItems[7] = { 5, 3, 8, 12, 15, 20, 0 };


// Animaciones básicas interiores
SDL_Rect framesFantasmaAnim[2] = { { 30, 0, 14, 14 }, { 44, 0, 14, 14 } };
SDL_Rect frameFantasmaAzul[2] = { { 172, 19, 14, 14 }, { 186, 19, 14, 14 } }; // Fantasma asustado
SDL_Rect framesPacmanAnim[2] = { { 3, 1, 14, 14 }, { 19, 1, 14, 14 } }; // Pacman de la fila superior


bool esMuro(int xPixels, int yPixels) {
    int col = xPixels / TAM_CASILLA;
    int fil = yPixels / TAM_CASILLA;
    if (col < 0 || col >= COLUMNAS || fil < 0 || fil >= FILAS) return true;
    return (mapa[fil][col] == 1);
}


void spawnearItem() {
    int col, fil;
    do {
        col = rand() % COLUMNAS;
        fil = rand() % FILAS;
    } while (mapa[fil][col] != 0); // Buscar espacio vacío


    itemActual.x = col * TAM_CASILLA;
    itemActual.y = fil * TAM_CASILLA;
    itemActual.tipo = rand() % 7; // Elige al azar entre frutas o la llave
    itemActual.puntos = puntosItems[itemActual.tipo];
    itemActual.activa = true;
    itemActual.tiempoSpawn = SDL_GetTicks();
}


void resetPosiciones() {
    pacman.x = 13 * TAM_CASILLA;
    pacman.y = 10 * TAM_CASILLA;
    pacman.dx = 0; pacman.dy = 0;
    pacman.nextDx = 0; pacman.nextDy = 0;


    numFantasmasActivos = nivel;
    if (numFantasmasActivos > MAX_FANTASMAS) numFantasmasActivos = MAX_FANTASMAS;


    for (int i = 0; i < numFantasmasActivos; i++) {
        fantasmas[i].x = (11 + i) * TAM_CASILLA;
        fantasmas[i].y = 4 * TAM_CASILLA;
        fantasmas[i].dx = (rand() % 2 == 0) ? 1 : -1;
        fantasmas[i].dy = 0;
        fantasmas[i].spriteSheetBox = cajasFantasmasSrc[i];
        fantasmas[i].vivo = true;
    }
    spawnearItem();
    esperandoReinicio = true;
}


int main(int argc, char **argv) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);


    SDL_Window *window = SDL_CreateWindow("Pac-Man Arcade Edition", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


    SDL_Texture *texTableros = IMG_LoadTexture(renderer, "tablerodepacman.png");
    SDL_Texture *texSprites = IMG_LoadTexture(renderer, "image_85babf.png");


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
                    case SDLK_x: running = false; break; // X para Salir
                    case SDLK_p: juegoEnPausa = !juegoEnPausa; break; // P para Pausar
                    case SDLK_SPACE: if (esperandoReinicio) esperandoReinicio = false; break;
                    case SDLK_n:
                        nivel++; if (nivel > 6) nivel = 1;
                        resetPosiciones(); esperandoReinicio = false;
                        break;
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


        // --- VERIFICAR REGLA DE DESAPARECER FRUTA (30 Segundos) ---
        if (itemActual.activa && (currentTime - itemActual.tiempoSpawn > 30000)) {
            itemActual.activa = false;
            spawnearItem(); // Genera automáticamente una nueva fruta en otra posición
        }


        // --- APAGAR PODER DE LA LLAVE AZUL (10 Segundos) ---
        if (modoLlaveAzul && currentTime > tiempoFinPoder) {
            modoLlaveAzul = false;
        }


        // --- FÍSICAS Y MOVIMIENTO ---
        if (!esperandoReinicio && (currentTime > lastLogicTime + 16)) {
           
            // Lógica Pac-Man: Giro condicionado por comandos W, A, S, D
            if (pacman.x % TAM_CASILLA == 0 && pacman.y % TAM_CASILLA == 0) {
                pacman.dx = pacman.nextDx;
                pacman.dy = pacman.nextDy;
            }


            // REGLA NUEVA: Si el siguiente pixel es un muro, Pac-Man choca y PIERDE una vida de inmediato
            if (esMuro(pacman.x + pacman.dx, pacman.y + pacman.dy) && (pacman.dx != 0 || pacman.dy != 0)) {
                lives--;
                if (lives <= 0) gameOver = true;
                else resetPosiciones();
                lastLogicTime = currentTime;
                continue;
            }


            // Si el camino está despejado, avanza con normalidad
            pacman.x += pacman.dx * 2;
            pacman.y += pacman.dy * 2;


            // --- RECOLECCIÓN DE FRUTAS / LLAVE AZUL ---
            if (itemActual.activa && abs(pacman.x - itemActual.x) < 10 && abs(pacman.y - itemActual.y) < 10) {
                itemActual.activa = false;
                if (itemActual.tipo == 6) { // Si es la Llave Azul
                    modoLlaveAzul = true;
                    tiempoFinPoder = currentTime + 10000; // 10 segundos de poder absoluto
                } else {
                    score += itemActual.puntos;
                }
                spawnearItem();
            }


            // Lógica IA de los Fantasmas
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


                fantasmas[i].x += fantasmas[i].dx * 2;
                fantasmas[i].y += fantasmas[i].dy * 2;


                // --- DETECCIÓN DE COLISIONES BAJO REGLAS DE PODER ---
                if (abs(pacman.x - fantasmas[i].x) < 12 && abs(pacman.y - fantasmas[i].y) < 12) {
                    if (modoLlaveAzul) {
                        // Cazamos al fantasma: Suma 50 puntos y lo manda a regenerarse al centro
                        score += 50;
                        fantasmas[i].x = 13 * TAM_CASILLA;
                        fantasmas[i].y = 4 * TAM_CASILLA;
                    } else {
                        // El fantasma nos mata
                        lives--;
                        if (lives <= 0) gameOver = true;
                        else resetPosiciones();
                    }
                }
            }
            lastLogicTime = currentTime;
        }


        // Animación de fotogramas
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


        // B. Dibujar la Fruta o Llave Azul activa en el mapa
        if (itemActual.activa) {
            SDL_Rect destItem = { itemActual.x, itemActual.y, TAM_CASILLA, TAM_CASILLA };
            SDL_RenderCopy(renderer, texSprites, &spritesItems[itemActual.tipo], &destItem);
        }


        // C. Dibujar Fantasmas
        for (int i = 0; i < numFantasmasActivos; i++) {
            SDL_Rect srcGhost;
            if (modoLlaveAzul) {
                // Si está activa la llave, cargamos la textura del fantasma azul asustado de la fila de abajo
                srcGhost.x = frameFantasmaAzul[fantasmas[i].currentFrame].x;
                srcGhost.y = frameFantasmaAzul[fantasmas[i].currentFrame].y;
                srcGhost.w = frameFantasmaAzul[fantasmas[i].currentFrame].w;
                srcGhost.h = frameFantasmaAzul[fantasmas[i].currentFrame].h;
            } else {
                // Si es modo normal, toma sus colores respectivos de la paleta
                srcGhost.x = fantasmas[i].spriteSheetBox.x + framesFantasmaAnim[fantasmas[i].currentFrame].x;
                srcGhost.y = fantasmas[i].spriteSheetBox.y + framesFantasmaAnim[fantasmas[i].currentFrame].y;
                srcGhost.w = framesFantasmaAnim[fantasmas[i].currentFrame].w;
                srcGhost.h = framesFantasmaAnim[fantasmas[i].currentFrame].h;
            }
            SDL_Rect destGhost = { fantasmas[i].x, fantasmas[i].y, TAM_CASILLA, TAM_CASILLA };
            SDL_RenderCopy(renderer, texSprites, &srcGhost, &destGhost);
        }


        // D. Dibujar Pac-Man Amarillo Clásico Animado
        SDL_Rect srcPacman = framesPacmanAnim[pacman.currentFrame];
        SDL_Rect destPacman = { pacman.x, pacman.y, TAM_CASILLA, TAM_CASILLA };
       
        double angulo = 0;
        if (pacman.dx == 1)  angulo = 180;
        if (pacman.dy == -1) angulo = 90;  
        if (pacman.dy == 1)  angulo = 270;


        SDL_RenderCopyEx(renderer, texSprites, &srcPacman, &destPacman, angulo, NULL, SDL_FLIP_NONE);


        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }


    printf("Puntuacion Final: %d\n", score);


    SDL_DestroyTexture(texTableros);
    SDL_DestroyTexture(texSprites);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
