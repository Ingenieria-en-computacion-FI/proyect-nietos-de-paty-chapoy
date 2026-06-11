#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


// Dimensiones de la ventana basadas en la proporción del laberinto individual escalado
#define WINDOW_WIDTH 448
#define WINDOW_HEIGHT 576


// Estructura para el jugador (Pac-Man)
typedef struct {
    int x;
    int y;
    int speed;
    int currentFrame;
    int direction; // 0: Derecha, 1: Izquierda, 2: Arriba, 3: Abajo
    bool moving;
} Player;


// Instancia global del jugador
Player pacman;


int nivel = 1;
int score = 0;
int lives = 3;
bool esperandoReinicio = false;
bool gameOver = false;
unsigned int lastFrameTime = 0;


// =============================================================================
// RECUADROS DE RECORTE (SDL_Rect) PARA LA NUEVA IMAGEN
// =============================================================================


// Coordenadas de los 4 laberintos de la fila superior de la nueva imagen
SDL_Rect mapasSrc[4] = {
    { 4,   4,   224, 288 }, // Nivel 1: Rosa
    { 232, 4,   224, 288 }, // Nivel 2: Verde agua
    { 460, 4,   224, 288 }, // Nivel 3: Naranja
    { 688, 4,   224, 288 }  // Nivel 4: Azul
};


// Tus coordenadas originales de animación de Pac-Man
SDL_Rect walkHorizontal[] = {
    { 1, 55, 18, 37 }, { 22, 56, 17, 36 }, { 43, 55, 18, 37 }, { 65, 56, 18, 36 }, { 88, 56, 18, 36 }
};
SDL_Rect walkUp[] = {
    { 1, 99, 19, 36 }, { 24, 99, 19, 36 }, { 47, 98, 21, 37 }, { 72, 98, 21, 37 }
};
SDL_Rect walkDown[] = {
    { 0, 12, 18, 37 }, { 22, 11, 19, 38 }, { 45, 11, 18, 38 }
};
SDL_Rect idle[] = {
    { 68, 13, 22, 36 }, { 94, 11, 22, 36 }
};


void resetPosicion() {
    pacman.x = 212; // Posición central inicial en la pantalla
    pacman.y = 410;
    pacman.moving = false;
    esperandoReinicio = true;
}


int main(int argc, char **argv) {
    // 1. Inicialización de los subsistemas de SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error SDL_Init: %s\n", SDL_GetError());
        return 1;
    }


    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Error IMG_Init: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }


    // 2. Creación de Ventana y Renderizador
    SDL_Window *window = SDL_CreateWindow(
        "Ms. Pac-Man Dinamico por Niveles",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );


    if (window == NULL) {
        printf("Error Window: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }


    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Error Renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }


    // 3. Carga de la Textura (¡Asegúrate de nombrar tu archivo como "tablerodepacman.png"!)
    SDL_Texture *spritesheet = IMG_LoadTexture(renderer, "tablerodepacman.png");
    if (spritesheet == NULL) {
        printf("Error al cargar la imagen: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }


    // Configuración inicial del juego
    pacman.speed = 4;
    pacman.currentFrame = 0;
    pacman.direction = 0;
    resetPosicion();
    esperandoReinicio = false; // Iniciar listo para jugar


    bool running = true;
    SDL_Event event;
    SDL_RendererFlip flip = SDL_FLIP_NONE;


    // 4. Bucle Maestro del Juego
    while (running && !gameOver) {
       
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_x: // Salir con X
                        running = false;
                        break;
                    case SDLK_SPACE: // Salir de la pausa de muerte de forma segura
                        if (esperandoReinicio) {
                            esperandoReinicio = false;
                        }
                        break;
                    case SDLK_n: // CAMBIO MANUAL DE NIVEL (Para pruebas)
                        nivel++;
                        if (nivel > 4) nivel = 1;
                        resetPosicion();
                        esperandoReinicio = false; // Lo dejamos activo para ver el cambio rápido
                        break;
                }
            }
        }


        // Lectura continua de los botones de dirección (W, A, S, D o Flechas)
        const Uint8 *keyboard = SDL_GetKeyboardState(NULL);
        pacman.moving = false;


        if (!esperandoReinicio) {
            if (keyboard[SDL_SCANCODE_RIGHT] || keyboard[SDL_SCANCODE_D]) {
                pacman.x += pacman.speed;
                pacman.moving = true;
                pacman.direction = 0;
                flip = SDL_FLIP_HORIZONTAL;
            }
            if (keyboard[SDL_SCANCODE_LEFT] || keyboard[SDL_SCANCODE_A]) {
                pacman.x -= pacman.speed;
                pacman.moving = true;
                pacman.direction = 0;
                flip = SDL_FLIP_NONE;
            }
            if (keyboard[SDL_SCANCODE_UP] || keyboard[SDL_SCANCODE_W]) {
                pacman.y -= pacman.speed;
                pacman.moving = true;
                pacman.direction = 1;
            }
            if (keyboard[SDL_SCANCODE_DOWN] || keyboard[SDL_SCANCODE_S]) {
                pacman.y += pacman.speed;
                pacman.moving = true;
                pacman.direction = 2;
            }


            // Bordes lógicos para que Pac-Man no se salga volando del laberinto
            if (pacman.x < 15) pacman.x = 15;
            if (pacman.x > WINDOW_WIDTH - 35) pacman.x = WINDOW_WIDTH - 35;
            if (pacman.y < 15) pacman.y = 15;
            if (pacman.y > WINDOW_HEIGHT - 50) pacman.y = WINDOW_HEIGHT - 50;
        }


        // 5. Gestión del tiempo para las animaciones (Tus intervalos originales)
        unsigned int currentTime = SDL_GetTicks();
        if (!pacman.moving) {
            if (currentTime > lastFrameTime + 500) {
                pacman.currentFrame++;
                lastFrameTime = currentTime;
            }
        } else {
            if (currentTime > lastFrameTime + 150) {
                pacman.currentFrame++;
                lastFrameTime = currentTime;
            }
        }


        // Selección del frame correspondiente de tus arreglos rectangulares
        SDL_Rect srcPacman;
        if (pacman.moving) {
            switch (pacman.direction) {
                case 0: srcPacman = walkHorizontal[pacman.currentFrame % 5]; break;
                case 1: srcPacman = walkUp[pacman.currentFrame % 4]; break;
                case 2: srcPacman = walkDown[pacman.currentFrame % 3]; break;
            }
        } else {
            srcPacman = idle[pacman.currentFrame % 2];
        }


        // =============================================================================
        // RENDERIZADO GRÁFICO (DIBUJO)
        // =============================================================================
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        // Índice cíclico para renderizar del mapa 0 al 3 basándonos en el nivel actual
        int mapaActualIdx = (nivel - 1) % 4;
        SDL_Rect destMapa = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };


        // Dibujar el fondo del tablero correspondiente al nivel
        SDL_RenderCopy(renderer, spritesheet, &mapasSrc[mapaActualIdx], &destMapa);


        // Dibujar a Pac-Man encima del tablero con sus dimensiones y animaciones reales
        SDL_Rect destPacman = { pacman.x, pacman.y, 24, 32 };
        SDL_RenderCopyEx(renderer, spritesheet, &srcPacman, &destPacman, 0, NULL, flip);


        // Presentar el cuadro en el monitor
        SDL_RenderPresent(renderer);


        SDL_Delay(16); // ~60 FPS estables y fluidos sin sobrecargar la CPU
    }


    // 6. Liberación de memoria limpia
    SDL_DestroyTexture(spritesheet);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();


    return 0;
}
