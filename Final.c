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


// Matriz (1=Muro, 0=Pellet normal, 3=Pellet de Poder, 4=Items, 2=Vacío)
int mapa[FILAS][COLUMNAS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 3, 0, 2, 0, 0, 1, 0, 2, 0, 2, 0, 1, 0, 2, 1, 0, 2, 0, 2, 0, 1, 0, 2, 0, 2, 3, 1},
    {1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 2, 0, 1, 0, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 2, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 0, 2, 3, 2, 0, 2, 2, 2, 2, 0, 2, 0, 2, 0, 2, 0, 2, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 2, 2, 0, 0, 2, 1, 2, 2, 0, 2, 2, 1, 1, 1, 1, 2, 0, 2, 0, 2, 1, 2, 2, 0, 2, 2, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
    {0, 2, 2, 0, 2, 2, 2, 1, 1, 2, 0, 2, 0, 2, 0, 2, 2, 2, 2, 1, 1, 0, 2, 2, 0, 2, 2, 0},
    {1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 2, 0, 1, 1, 1, 1, 1},
    {0, 2, 0, 2, 0, 2, 2, 2, 0, 2, 1, 0, 2, 0, 2, 0, 2, 1, 2, 2, 0, 0, 2, 2, 0, 2, 0, 0},
    {1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 0, 1, 1, 1, 1, 1},
    {0, 0, 2, 0, 2, 2, 2, 1, 1, 2, 0, 2, 0, 2, 0, 2, 0, 2, 2, 1, 1, 0, 2, 0, 2, 0, 2, 0},
    {1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 0, 1, 1, 1, 1, 1},
    {1, 2, 0, 2, 0, 2, 2, 2, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 0, 0, 2, 0, 2, 0, 2, 1},
    {1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 2, 0, 0, 1, 1, 2, 0, 1, 1, 1, 0, 1},
    {1, 0, 2, 0, 1, 2, 2, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 2, 1, 0, 2, 0, 1},
    {1, 1, 1, 0, 1, 0, 0, 1, 1, 2, 0, 2, 0, 2, 4, 2, 0, 2, 0, 1, 1, 2, 0, 1, 0, 1, 1, 1},
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
    { 401, 83, 16, 16 }, // Fantasma Gris
    { 418, 83, 16, 16 }  // Fantasma Peach
};


SDL_Rect spritesItems[9] = {
    { 618, 489, 16, 16 }, // 0 = Fresa
    { 401, 489, 16, 16 }, // 1 = Melón
    { 835, 489, 16, 16 }, // 2 = Manzana
    { 69, 675, 16, 16 },  // 3 = Fruta mala
    { 852, 489, 16, 16 }, // 4 = Galleta Bonus
    { 503, 540, 16, 16 }, // 5 = Píldora de poder
    { 120, 168, 16, 16 }, // 6 = Explosión mala
    { 435, 354, 16, 16 }, // 7 = Letrero de "200" (Fantasmas)
    { 869, 540, 16, 16 }  // 8 = Letrero de "800" (Explosión)
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


bool frutaVisible = false;
unsigned int tiempoProximaFruta = 0;
unsigned int tiempoDesaparecerFruta = 0;
int frutaFila = 10;  
int frutaColumna = 13;


// Control de la Pantalla de Inicio, Game Over e Índices dinámicos
bool pantallaInicio = true;
bool pantallaGameOver = false; // PANTALLA GRÁFICA
int indiceFrutaActiva = 0;
int indiceMaloActivo = 3;  


// Variables globales para el letrero flotante de puntuaciones
bool mostrarPuntosFlotantes = false;
unsigned int tiempoFinPuntos = 0;
int puntosFlotantesX = 0;
int puntosFlotantesY = 0;
int indicePuntosFlotantesA_Dibujar = 7;


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


void dibujarLetraGrande(SDL_Renderer *renderer, char letra, int x, int y, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    if (letra == 'P') {
        SDL_Rect seg[] = {{x, y, 20, 6}, {x, y, 6, 32}, {x+14, y, 6, 20}, {x, y+14, 20, 6}};
        for(int i=0; i<4; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'A') {
        SDL_Rect seg[] = {{x, y, 20, 6}, {x, y, 6, 32}, {x+14, y, 6, 32}, {x, y+14, 20, 6}};
        for(int i=0; i<4; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'C') {
        SDL_Rect seg[] = {{x, y, 20, 6}, {x, y, 6, 32}, {x, y+26, 20, 6}};
        for(int i=0; i<3; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'G') {
        SDL_Rect seg[] = {{x, y, 20, 6}, {x, y, 6, 32}, {x, y+26, 20, 6}, {x+14, y+16, 6, 16}, {x+8, y+16, 8, 6}};
        for(int i=0; i<5; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'I') {
        SDL_Rect seg[] = {{x, y, 18, 6}, {x+6, y, 6, 32}, {x, y+26, 18, 6}};
        for(int i=0; i<3; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'R') {
        SDL_Rect seg[] = {
            {x, y, 20, 6},      
            {x, y, 6, 32},      
            {x+14, y, 6, 18},    
            {x, y+14, 20, 6},    
            {x+12, y+20, 8, 12}  
        };
        for(int i=0; i<5; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'L') {
        SDL_Rect seg[] = {{x, y, 6, 32}, {x, y+26, 20, 6}};
        for(int i=0; i<2; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'S') {
        SDL_Rect seg[] = {{x, y, 20, 6}, {x, y, 6, 16}, {x, y+13, 20, 6}, {x+14, y+16, 6, 16}, {x, y+26, 20, 6}};
        for(int i=0; i<5; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'E') {
        SDL_Rect seg[] = {{x, y, 20, 6}, {x, y, 6, 32}, {x, y+13, 16, 6}, {x, y+26, 20, 6}};
        for(int i=0; i<4; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'M') {
        SDL_Rect seg[] = {{x, y, 6, 32}, {x+14, y, 6, 32}, {x+4, y+4, 4, 8}, {x+8, y+4, 4, 8}};
        for(int i=0; i<4; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'O') {
        SDL_Rect seg[] = {{x, y, 20, 6}, {x, y, 6, 32}, {x+14, y, 6, 32}, {x, y+26, 20, 6}};
        for(int i=0; i<4; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'V') {
        SDL_Rect seg[] = {{x, y, 6, 24}, {x+14, y, 6, 24}, {x+4, y+24, 4, 8}, {x+10, y+24, 4, 8}};
        for(int i=0; i<4; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
}


void dibujarLetraChica(SDL_Renderer *renderer, char letra, int x, int y, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    if (letra == 'P') {
        SDL_Rect seg[] = {{x, y, 10, 3}, {x, y, 3, 16}, {x+7, y, 3, 10}, {x, y+8, 10, 3}};
        for(int i=0; i<4; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'A') {
        SDL_Rect seg[] = {{x, y, 10, 3}, {x, y, 3, 16}, {x+7, y, 3, 16}, {x, y+8, 10, 3}};
        for(int i=0; i<4; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'C') {
        SDL_Rect seg[] = {{x, y, 10, 3}, {x, y, 3, 16}, {x, y+14, 10, 3}};
        for(int i=0; i<3; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'E') {
        SDL_Rect seg[] = {{x, y, 10, 3}, {x, y, 3, 16}, {x, y+7, 8, 3}, {x, y+14, 10, 3}};
        for(int i=0; i<4; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'M') {
        SDL_Rect seg[] = {{x, y, 3, 16}, {x+7, y, 3, 16}, {x+2, y+2, 2, 4}, {x+4, y+2, 2, 4}};
        for(int i=0; i<4; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'Z') {
        SDL_Rect seg[] = {{x, y, 10, 3}, {x, y+14, 10, 3}, {x+6, y+2, 3, 4}, {x+4, y+6, 3, 4}, {x+2, y+10, 3, 4}};
        for(int i=0; i<5; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'R') {
        SDL_Rect seg[] = {{x, y, 10, 3}, {x, y, 3, 16}, {x+7, y, 3, 9}, {x, y+8, 10, 3}, {x+6, y+10, 4, 6}};
        for(int i=0; i<5; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'S') {
        SDL_Rect seg[] = {{x, y, 10, 3}, {x, y, 3, 10}, {x, y+8, 10, 3}, {x+7, y+8, 3, 8}, {x, y+14, 10, 3}};
        for(int i=0; i<5; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'U') {
        SDL_Rect seg[] = {{x, y, 3, 16}, {x+7, y, 3, 16}, {x, y+14, 10, 3}};
        for(int i=0; i<3; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'O') {
        SDL_Rect seg[] = {{x, y, 10, 3}, {x, y, 3, 16}, {x+7, y, 3, 16}, {x, y+14, 10, 3}};
        for(int i=0; i<4; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'L') {
        SDL_Rect seg[] = {{x, y, 3, 16}, {x, y+14, 10, 3}};
        for(int i=0; i<2; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'I') {
        SDL_Rect seg[] = {{x, y, 9, 3}, {x+3, y, 3, 16}, {x, y+14, 9, 3}};
        for(int i=0; i<3; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'X') {
        SDL_Rect seg[] = {{x, y, 3, 5}, {x+7, y, 3, 5}, {x+3, y+5, 4, 6}, {x, y+11, 3, 5}, {x+7, y+11, 3, 5}};
        for(int i=0; i<5; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'N') {
        SDL_Rect seg[] = {{x, y, 3, 16}, {x+7, y, 3, 16}, {x+3, y+3, 4, 4}};
        for(int i=0; i<3; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'F') {
        SDL_Rect seg[] = {{x, y, 10, 3}, {x, y, 3, 16}, {x, y+7, 8, 3}};
        for(int i=0; i<3; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'V') {
        SDL_Rect seg[] = {{x, y, 3, 12}, {x+7, y, 3, 12}, {x+3, y+12, 4, 4}};
        for(int i=0; i<3; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'T') {
        SDL_Rect seg[] = {{x, y, 10, 3}, {x+4, y, 3, 16}};
        for(int i=0; i<2; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'J') {
        SDL_Rect seg[] = {{x+7, y, 3, 14}, {x, y+12, 3, 4}, {x, y+14, 10, 3}};
        for(int i=0; i<3; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
    else if (letra == 'G') {
        SDL_Rect seg[] = {{x, y, 10, 3}, {x, y, 3, 16}, {x, y+14, 10, 3}, {x+7, y+8, 3, 8}, {x+4, y+8, 4, 3}};
        for(int i=0; i<5; i++) SDL_RenderFillRect(renderer, &seg[i]);
    }
}


void dibujarTextoChico(SDL_Renderer *renderer, const char *texto, int x, int y, int r, int g, int b) {
    int i = 0;
    while (texto[i] != '\0') {
        if (texto[i] != ' ') {
            dibujarLetraChica(renderer, texto[i], x + (i * 14), y, r, g, b);
        }
        i++;
    }
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


// FUNCIÓN PRINCIPAL MAIN
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
                case SDLK_SPACE:
                    if (pantallaInicio) {
                        pantallaInicio = false;
                    }
                    if (esperandoReinicio) esperandoReinicio = false;
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


        if (mostrarPuntosFlotantes && currentTime > tiempoFinPuntos) {
            mostrarPuntosFlotantes = false;
        }


        if (modoLlaveAzul && currentTime > tiempoFinPoder) {
            modoLlaveAzul = false;
        }


        if (!pantallaInicio && !pantallaGameOver && !esperandoReinicio && (currentTime > lastLogicTime + 16)) {
           
            // Spawn aleatorio
            if (!frutaVisible && currentTime > tiempoProximaFruta) {
                if (rand() % 2 == 0) {
                    mapa[frutaFila][frutaColumna] = 4;
                    int poolFrutas[4] = {0, 1, 2, 4};
                    indiceFrutaActiva = poolFrutas[rand() % 4];
                } else {
                    mapa[frutaFila][frutaColumna] = 5;
                    indiceMaloActivo = (rand() % 2 == 0) ? 3 : 6;
                }
                frutaVisible = true;
                pelletsRestantes++;
                tiempoDesaparecerFruta = currentTime + 8000 + (rand() % 4000);
            }
            else if (frutaVisible && currentTime > tiempoDesaparecerFruta) {
                if (mapa[frutaFila][frutaColumna] == 4 || mapa[frutaFila][frutaColumna] == 5) {
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


            // Teletransportación
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


                if (tipoCasilla == 0 || tipoCasilla == 3 || tipoCasilla == 4 || tipoCasilla == 5) {
                    mapa[pacmanFil][pacmanCol] = 2;
                    pelletsRestantes--;


                    if (tipoCasilla == 0) score += 10;
                    else if (tipoCasilla == 3) {
                        score += 20;
                        modoLlaveAzul = true;
                        tiempoFinPoder = currentTime + 10000;
                    }
                    else if (tipoCasilla == 4) {
                        if (indiceFrutaActiva == 4) score += 500;
                        else if (indiceFrutaActiva == 2) score += 200;
                        else score += 100;
                       
                        frutaVisible = false;
                        tiempoProximaFruta = currentTime + 15000 + (rand() % 10000);
                    }
                    else if (tipoCasilla == 5) {
                        if (indiceMaloActivo == 3) {
                            score -= 50;          
                            modoLlaveAzul = false;
                            numFantasmasActivos += 2;
                            if (numFantasmasActivos > MAX_FANTASMAS) numFantasmasActivos = MAX_FANTASMAS;
                           
                            for (int i = 0; i < numFantasmasActivos; i++) {
                                fantasmas[i].x = (11 + (i % 3)) * TAM_CASILLA;
                                fantasmas[i].y = 4 * TAM_CASILLA;
                                fantasmas[i].vivo = true;
                            }
                        } else {
                            score -= 800;
                            if (score < 0) score = 0;


                            puntosFlotantesX = pacmanCol * TAM_CASILLA;
                            puntosFlotantesY = pacmanFil * TAM_CASILLA;
                            indicePuntosFlotantesA_Dibujar = 8;
                            mostrarPuntosFlotantes = true;
                            tiempoFinPuntos = currentTime + 1000;
                        }


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
                        score += 200;
                       
                        fantasmas[i].x = 13 * TAM_CASILLA;
                        fantasmas[i].y = 4 * TAM_CASILLA;


                        puntosFlotantesX = pacman.x;
                        puntosFlotantesY = pacman.y;
                        indicePuntosFlotantesA_Dibujar = 7;
                        mostrarPuntosFlotantes = true;
                        tiempoFinPuntos = currentTime + 1000;
                    } else {
                        lives--;
                        if (lives <= 0) {
                            pantallaGameOver = true; // === ACTIVA LA PANTALLA GRÁFICA EN LUGAR DE CERRAR ===
                        }
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


        // PANTALLA DE INICIO ARCADE
        if (pantallaInicio) {
            dibujarLetraGrande(renderer, 'P', 110,  70, 255, 0, 0);    
            dibujarLetraGrande(renderer, 'A', 134,  70, 255, 128, 0);  
            dibujarLetraGrande(renderer, 'C', 158,  70, 255, 255, 0);  
           
            dibujarLetraGrande(renderer, 'G', 200, 70, 0, 255, 255);
            dibujarLetraGrande(renderer, 'I', 224, 70, 0, 255, 255);
            dibujarLetraGrande(renderer, 'R', 248, 70, 0, 255, 255);
            dibujarLetraGrande(renderer, 'L', 272, 70, 0, 255, 255);
            dibujarLetraGrande(renderer, 'S', 296, 70, 0, 255, 255);


            dibujarTextoChico(renderer, "ESPACIO  EMPEZAR", 112, 170, 0, 255, 0);
            dibujarTextoChico(renderer, "P  PAUSA", 168, 210, 255, 255, 255);    
            dibujarTextoChico(renderer, "X  SALIR", 168, 250, 255, 255, 255);    
           
            SDL_Rect decoracionPacman = { 290, 250, 16, 16 };
            if (texturaHojaSprites) {
                SDL_RenderCopy(renderer, texturaHojaSprites, &framesPacmanAnim[0], &decoracionPacman);
            }


            SDL_RenderPresent(renderer);
            SDL_Delay(16);
            continue;
        }


        // PANTALLA DE GAME OVER
        if (pantallaGameOver) {
            dibujarLetraGrande(renderer, 'G', 120, 60, 255, 0, 0);
            dibujarLetraGrande(renderer, 'A', 144, 60, 255, 0, 0);
            dibujarLetraGrande(renderer, 'M', 168, 60, 255, 0, 0);
            dibujarLetraGrande(renderer, 'E', 192, 60, 255, 0, 0);


            dibujarLetraGrande(renderer, 'O', 230, 60, 255, 0, 0);
            dibujarLetraGrande(renderer, 'V', 254, 60, 255, 0, 0);
            dibujarLetraGrande(renderer, 'E', 278, 60, 255, 0, 0);
            dibujarLetraGrande(renderer, 'R', 302, 60, 255, 0, 0);


            dibujarTextoChico(renderer, "PUNTAJE  FINAL", 100, 140, 255, 255, 255);
            dibujarValor(renderer, score, 310, 140);


            dibujarTextoChico(renderer, "NIVEL", 150, 180, 255, 255, 255);
            dibujarNumero(renderer, nivel, 250, 180);


            dibujarTextoChico(renderer, "ESPACIO  REINTENTAR", 90, 240, 0, 255, 0);
            dibujarTextoChico(renderer, "X  SALIR", 160, 280, 255, 255, 0);


            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) running = false;
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_x) {
                        running = false;
                    }
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        // Reseteo total del estado de juego
                        score = 0;
                        nivel = 1;
                        lives = 3;
                        for (int f = 0; f < FILAS; f++) {
                            for (int c = 0; c < COLUMNAS; c++) {
                                mapa[f][c] = mapaOriginal[f][c];
                            }
                        }
                        resetPosiciones();
                        pantallaGameOver = false;
                        esperandoReinicio = false;
                    }
                }
            }


            SDL_RenderPresent(renderer);
            SDL_Delay(16);
            continue;
        }


        // Dibujar Muros
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


        // Dibujar Comestibles
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
                        SDL_RenderCopy(renderer, texturaHojaSprites, &spritesItems[indiceFrutaActiva], &destItem);
                    }
                }
                else if (mapa[f][c] == 5) {
                    if (texturaHojaSprites) {
                        SDL_RenderCopy(renderer, texturaHojaSprites, &spritesItems[indiceMaloActivo], &destItem);
                    }
                }
            }
        }


        // Dibujar Fantasmas
        for (int i = 0; i < numFantasmasActivos; i++) {
            SDL_Rect destGhost = {fantasmas[i].x, fantasmas[i].y, TAM_CASILLA, TAM_CASILLA};
           
            if (texturaHojaSprites) {
                if (modoLlaveAzul) {
                    SDL_RenderCopy(renderer, texturaHojaSprites, &frameFantasmaAzul[fantasmas[i].currentFrame], &destGhost);
                } else {
                    double anguloG = 0.0;
                    SDL_RendererFlip flipG = SDL_FLIP_NONE;


                    if (fantasmas[i].dx == -1) {
                        flipG = SDL_FLIP_HORIZONTAL;
                    }
                    else if (fantasmas[i].dx == 1) {
                        anguloG = 0.0;
                    }
               
                    SDL_RenderCopyEx(renderer, texturaHojaSprites, &cajasFantasmasSrc[fantasmas[i].idColor],
                                     &destGhost, anguloG, NULL, flipG);
                }
            }
        }


        // Dibujar Jugador
        SDL_Rect destPacman = {pacman.x, pacman.y, TAM_CASILLA, TAM_CASILLA};
        if (texturaHojaSprites) {
            double angulo = 0.0;
            SDL_RendererFlip flip = SDL_FLIP_NONE;


            if (pacman.dx == -1) {
                flip = SDL_FLIP_HORIZONTAL;
            }
            else if (pacman.dx == 1) {
                angulo = 0.0;
            }
            else if (pacman.dy == -1) {
                angulo = 270.0;
            }
            else if (pacman.dy == 1) {
                angulo = 90.0;
            }


            SDL_RenderCopyEx(renderer, texturaHojaSprites, &framesPacmanAnim[pacman.currentFrame],
                             &destPacman, angulo, NULL, flip);
        }


        // Dibujar puntos flotantes (200 o 800)
        if (mostrarPuntosFlotantes && texturaHojaSprites) {
            SDL_Rect destPuntos = { puntosFlotantesX, puntosFlotantesY - 4, TAM_CASILLA, TAM_CASILLA };
            SDL_RenderCopy(renderer, texturaHojaSprites, &spritesItems[indicePuntosFlotantesA_Dibujar], &destPuntos);
        }


        // Dibujar Marcador Compacto
        int posYMarcador = 356;


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
