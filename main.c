#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600

typedef struct {
    float x, y;
} Ponto;

typedef struct {
    int v1, v2;
} Aresta;

typedef struct {
    int n_pontos;
    int n_arestas;
    Ponto *pontos;
    Aresta *arestas;
} Objeto2D;

void desenhaAresta(SDL_Renderer *renderer, float x1, float y1, float x2, float y2) {
    int xa, xb, ya, yb;
    xa = WIDTH * ((x1 + 1) / 2);
    xb = WIDTH * ((x2 + 1) / 2);
    ya = HEIGHT * ((-y1 + 1) / 2);
    yb = HEIGHT * ((-y2 + 1) / 2);
    SDL_RenderDrawLine(renderer, xa, ya, xb, yb);
}

Ponto rotacionarPonto(Ponto p, float angulo) {
    Ponto r;
    r.x = p.x * cos(angulo) - p.y * sin(angulo);
    r.y = p.x * sin(angulo) + p.y * cos(angulo);
    return r;
}

void desenhaObjeto(SDL_Renderer *renderer, Objeto2D *obj, float escalaX, float escalaY, float deslocX, float deslocY, float angulo) {
    for (int i = 0; i < obj->n_arestas; i++) {
        Ponto p1 = obj->pontos[obj->arestas[i].v1];
        Ponto p2 = obj->pontos[obj->arestas[i].v2];

        // Aplicar rotação
        p1 = rotacionarPonto(p1, angulo);
        p2 = rotacionarPonto(p2, angulo);

        // Aplicar escala e translação
        float x1 = deslocX + p1.x * escalaX;
        float y1 = deslocY + p1.y * escalaY;
        float x2 = deslocX + p2.x * escalaX;
        float y2 = deslocY + p2.y * escalaY;

        desenhaAresta(renderer, x1, y1, x2, y2);
    }
}

Objeto2D* carregarObjeto(const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "r");
    if (!fp) {
        printf("Erro ao abrir arquivo %s\n", nomeArquivo);
        return NULL;
    }

    Objeto2D *obj = malloc(sizeof(Objeto2D));

    fscanf(fp, "%d", &obj->n_pontos);
    obj->pontos = malloc(sizeof(Ponto) * obj->n_pontos);
    for (int i = 0; i < obj->n_pontos; i++) {
        fscanf(fp, "%f %f", &obj->pontos[i].x, &obj->pontos[i].y);
    }

    fscanf(fp, "%d", &obj->n_arestas);
    obj->arestas = malloc(sizeof(Aresta) * obj->n_arestas);
    for (int i = 0; i < obj->n_arestas; i++) {
        fscanf(fp, "%d %d", &obj->arestas[i].v1, &obj->arestas[i].v2);
    }

    fclose(fp);
    return obj;
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Transformações 2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Objeto2D *objeto = carregarObjeto("casa.txt");
    if (!objeto) return 1;

    float escalaX = 0.1f, escalaY = 0.1f;
    float deslocX = -0.9f, deslocY = -0.4f;
    float angulo = 0.0f;

    SDL_Event event;
    int rodando = 1;

    while (rodando) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) rodando = 0;

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w: deslocY += 0.01f; break;
                    case SDLK_s: deslocY -= 0.01f; break;
                    case SDLK_a: deslocX -= 0.01f; break;
                    case SDLK_d: deslocX += 0.01f; break;
                    case SDLK_q: angulo += 0.05f; break; // rotaciona sentido horário
                    case SDLK_e: angulo -= 0.05f; break; // rotaciona sentido anti-horário
                }
            }

            if (event.type == SDL_MOUSEWHEEL) {
                if (event.wheel.y > 0) {
                    escalaX += 0.01f;
                    escalaY += 0.01f;
                } else if (event.wheel.y < 0) {
                    escalaX -= 0.01f;
                    escalaY -= 0.01f;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        desenhaObjeto(renderer, objeto, escalaX, escalaY, deslocX, deslocY, angulo);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free(objeto->pontos);
    free(objeto->arestas);
    free(objeto);

    return 0;
}
