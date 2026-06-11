#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#define WINDOW_WIDTH 448
#define WINDOW_HEIGHT 576
#define MAX_FANTASMAS 6


// Estructura para Pac-Man
typedef struct {
    int x;
    int y;
    int speed;
    int currentFrame;
    int direction; // 0: Derecha, 1: Izquierda, 2: Arriba, 3: Abajo
    bool moving;
} Player;


// Estructura para los Fantasmas
typedef struct {
    int x;
    int y;
    int speed;
    int currentFrame;
    int direction;
    SDL_Rect spriteSheetBox; // Región base de su cuadrícula de color
    bool vivo;
} Ghost;


// Instancias globales
Player pacman;
Ghost fantasmas[MAX_FANTASMAS];


int nivel = 1;
int numFantasmasActivos = 1;
int score = 0;
int lives = 3;
bool esperandoReinicio = false;
bool gameOver = false;


unsigned int lastFrameTime = 0;
unsigned int lastGhostMoveTime = 0;


// =============================================================================
// RECUADROS DE RECORTE (SDL_Rect) - IMAGENES
// =============================================================================


// Los 4 laberintos del archivo anterior (Mantenemos la lógica de la hoja de tableros)
SDL_Rect mapasSrc[4] = {
    { 4,   4,   224, 288 }, // Nivel 1: Rosa
    { 232, 4,   224, 288 }, // Nivel 2: Verde agua
    { 460, 4,   224, 288 }, // Nivel 3: Naranja
    { 688, 4,   224, 288 }  // Nivel 4: Azul
};


// Coordenadas base en la nueva imagen para las cajas contenedoras de los 6 fantasmas
// Siguiendo el orden solicitado: Fila superior (izq a der) y luego fila inferior
SDL_Rect cajasFantasmasSrc[MAX_FANTASMAS] = {
    { 0,   0,   224, 288 }, // 1. Rojo (Fila 1, Col 1)
    { 228, 0,   224, 288 }, // 2. Rosa Claro (Fila 1, Col 2)
    { 456, 0,   224, 288 }, // 3. Cian (Fila 1, Col 3)
    { 684, 0,   224, 288 }, // 4. Naranja (Fila 1, Col 4)
    { 912, 0,   224, 288 }, // 5. Verde (Fila 1, Col 5)
    { 0,   292, 224, 288 }  // 6. Rojo-Naranja Alternativo (Fila 2, Col 1)
};


// Marcos relativos de animación de las "patitas" de los fantasmas mirando al frente/lados
// Dentro de cada caja, los fantasmas con animación están en la parte inferior izquierda de su bloque
SDL_Rect framesFantasmaAnim[2] = {
    { 3,  121, 14, 14 }, // Frame 1: Patas abiertas
    { 19, 121, 14, 14 }  // Frame 2: Patas cerradas
};


// Animaciones de Pac-Man (Desde su hoja de texturas correspondiente)
SDL_Rect walkHorizontal[] = { { 1, 55, 18, 37 }, { 22, 56, 17, 36 }, { 43, 55, 18, 37 }, { 65, 56, 18, 36 } };
SDL_Rect walkUp[]         = { { 1, 99, 19, 36 }, { 24, 99, 19, 36 }, { 47, 98, 21, 37 } };
SDL_Rect walkDown[]       = { { 0, 12, 18, 37 }, { 22, 11, 19, 38 }, { 45, 11, 18, 38 } };
SDL_Rect idle[]           = { { 68, 13, 22, 36 }, { 94, 11, 22, 36 } };


// Configurar niveles y cantidad de enemigos correspondientes
void configurarNivel() {
    numFantasmasActivos = nivel;
    if (numFantasmasActivos > MAX_FANTASMAS) numFantasmasActivos = MAX_FANTASMAS;


    // Inicializar los fantasmas asignando su caja de color correspondiente
    for (int i = 0; i < numFantasmasActivos; i++) {
        fantasmas[i].x = 60 + (i * 60); // Desplegar espaciados para evitar que se encimen al inicio
        fantasmas[i].y = 100 + (i * 30);
        fantasmas[i].speed = 2;
        fantasmas[i].currentFrame = 0;
        fantasmas[i].direction = rand() % 4;
        fantasmas[i].spriteSheetBox = cajasFantasmasSrc[i];
        fantasmas[i].vivo = true;
    }


    // Reposicionar a Pac-Man
    pacman.x = 212;
    pacman.y = 410;
    pacman.moving = false;
    esperandoReinicio = true;
}


int main(int argc, char **argv) {
    srand(time(NULL));


    if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { SDL_Quit(); return 1; }


    SDL_Window *window = SDL_CreateWindow("Ms. Pac-Man & Fantasmas Animados", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


    // Carga de las dos imágenes necesarias
    SDL_Texture *texTableros = IMG_LoadTexture(renderer, "tablerodepacman.png");
    SDL_Texture *texFantasmas = IMG_LoadTexture(renderer, "fantasmasdepacman.png");


    if (!texTableros || !texFantasmas) {
        printf("Error: Verifica que 'tablerodepacman.png' y 'fantasmasdepacman.png' estén en la misma carpeta.\n");
        return 1;
    }


    pacman.speed = 4;
    pacman.currentFrame = 0;
    pacman.direction = 0;
   
    configurarNivel();
    esperandoReinicio = false; // Listo para iniciar directo


    bool running = true;
    SDL_Event event;
    SDL_RendererFlip flipPacman = SDL_FLIP_NONE;


    while (running && !gameOver) {
        unsigned int currentTime = SDL_GetTicks();


        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_x) running = false;
                if (event.key.keysym.sym == SDLK_SPACE && esperandoReinicio) esperandoReinicio = false;
                if (event.key.keysym.sym == SDLK_n) { // Truco 'N': Forzar cambio de nivel para verificar
                    nivel++;
                    if (nivel > 6) nivel = 1;
                    configurarNivel();
                    esperandoReinicio = false;
                }
            }
        }


        const Uint8 *keyboard = SDL_GetKeyboardState(NULL);
        pacman.moving = false;


        // 1. Control de movimiento de Pac-Man
        if (!esperandoReinicio) {
            if (keyboard[SDL_SCANCODE_RIGHT] || keyboard[SDL_SCANCODE_D]) {
                pacman.x += pacman.speed; pacman.moving = true; pacman.direction = 0; flipPacman = SDL_FLIP_HORIZONTAL;
            }
            if (keyboard[SDL_SCANCODE_LEFT] || keyboard[SDL_SCANCODE_A]) {
                pacman.x -= pacman.speed; pacman.moving = true; pacman.direction = 0; flipPacman = SDL_FLIP_NONE;
            }
            if (keyboard[SDL_SCANCODE_UP] || keyboard[SDL_SCANCODE_W]) {
                pacman.y -= pacman.speed; pacman.moving = true; pacman.direction = 1;
            }
            if (keyboard[SDL_SCANCODE_DOWN] || keyboard[SDL_SCANCODE_S]) {
                pacman.y += pacman.speed; pacman.moving = true; pacman.direction = 2;
            }


            // Límites de pantalla para Pac-Man
            if (pacman.x < 15) pacman.x = 15;
            if (pacman.x > WINDOW_WIDTH - 35) pacman.x = WINDOW_WIDTH - 35;
            if (pacman.y < 15) pacman.y = 15;
            if (pacman.y > WINDOW_HEIGHT - 50) pacman.y = WINDOW_HEIGHT - 50;
        }


        // 2. IA Inteligente de Movimiento de los Fantasmas (Cada 200 ms toman un paso o cambian dirección)
        if (!esperandoReinicio && (currentTime > lastGhostMoveTime + 200)) {
            for (int i = 0; i < numFantasmasActivos; i++) {
                // Pequeña probabilidad de cambiar de rumbo al azar para simular búsqueda
                if (rand() % 100 < 20) {
                    fantasmas[i].direction = rand() % 4;
                }


                switch (fantasmas[i].direction) {
                    case 0: fantasmas[i].x += fantasmas[i].speed * 4; break; // Derecha
                    case 1: fantasmas[i].x -= fantasmas[i].speed * 4; break; // Izquierda
                    case 2: fantasmas[i].y -= fantasmas[i].speed * 4; break; // Arriba
                    case 3: fantasmas[i].y += fantasmas[i].speed * 4; break; // Abajo
                }


                // Límites de rebote en los bordes del tablero para los fantasmas
                if (fantasmas[i].x < 15) { fantasmas[i].x = 15; fantasmas[i].direction = 0; }
                if (fantasmas[i].x > WINDOW_WIDTH - 35) { fantasmas[i].x = WINDOW_WIDTH - 35; fantasmas[i].direction = 1; }
                if (fantasmas[i].y < 15) { fantasmas[i].y = 15; fantasmas[i].direction = 3; }
                if (fantasmas[i].y > WINDOW_HEIGHT - 50) { fantasmas[i].y = WINDOW_HEIGHT - 50; fantasmas[i].direction = 2; }


                // Cambiar el frame de animación de las patitas del fantasma
                fantasmas[i].currentFrame = (fantasmas[i].currentFrame + 1) % 2;
            }
            lastGhostMoveTime = currentTime;
        }


        // 3. Manejo de tiempos para la animación de Pac-Man
        if (currentTime > lastFrameTime + (pacman.moving ? 150 : 500)) {
            pacman.currentFrame++;
            lastFrameTime = currentTime;
        }


        // =============================================================================
        // PROCESO DE RENDERIZADO (DIBUJO)
        // =============================================================================
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        // A. Dibujar el mapa del nivel (Cíclico de 0 a 3 de la imagen de laberintos)
        int mapaActualIdx = (nivel - 1) % 4;
        SDL_Rect destMapa = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderCopy(renderer, texTableros, &mapasSrc[mapaActualIdx], &destMapa);


        // B. Dibujar los Fantasmas Activos con movimiento de patitas en tiempo real
        for (int i = 0; i < numFantasmasActivos; i++) {
            // Calcular las coordenadas exactas combinando el origen de su bloque de color + el frame de patitas
            SDL_Rect srcGhost = {
                fantasmas[i].spriteSheetBox.x + framesFantasmaAnim[fantasmas[i].currentFrame].x,
                fantasmas[i].spriteSheetBox.y + framesFantasmaAnim[fantasmas[i].currentFrame].y,
                framesFantasmaAnim[fantasmas[i].currentFrame].w,
                framesFantasmaAnim[fantasmas[i].currentFrame].h
            };


            // Tamaño escalado para que se vean bien dentro de los pasillos
            SDL_Rect destGhost = { fantasmas[i].x, fantasmas[i].y, 28, 28 };
           
            // Dibujar fantasma usando la textura de fantasmas
            SDL_RenderCopy(renderer, texFantasmas, &srcGhost, &destGhost);
        }


        // C. Dibujar a Pac-Man con sus animaciones
        SDL_Rect srcPacman;
        if (pacman.moving) {
            switch (pacman.direction) {
                case 0: srcPacman = walkHorizontal[pacman.currentFrame % 4]; break;
                case 1: srcPacman = walkUp[pacman.currentFrame % 3]; break;
                case 2: srcPacman = walkDown[pacman.currentFrame % 3]; break;
            }
        } else {
            srcPacman = idle[pacman.currentFrame % 2];
        }


        SDL_Rect destPacman = { pacman.x, pacman.y, 24, 32 };
        // Usamos la textura de tableros porque ahí es donde venía tu Pac-Man originalmente
        SDL_RenderCopyEx(renderer, texTableros, &srcPacman, &destPacman, 0, NULL, flipPacman);


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
