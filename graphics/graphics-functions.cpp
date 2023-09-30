#include "graphics-functions.h"

// draw a circle by drawing lines across it
void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, int ang_steps) {
    for (int i=0; i<ang_steps; i++) {
            // compute current angle
            double theta = (M_PI / ang_steps) * i;

            // compute line end-points
            int x1 = static_cast<int>(centerX + (radius * cos(theta)));
            int x2 = static_cast<int>(centerX + (radius * cos(theta + M_PI)));

            int y1 = static_cast<int>(centerY + (radius * sin(theta)));
            int y2 = static_cast<int>(centerY + (radius * sin(theta + M_PI)));

            // draw line
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
}

void init(win_data *data, int height, int width) {
    // start SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Impossibile inizializzare SDL: %s", SDL_GetError());
        return;
    }

    // create window
    data->window = SDL_CreateWindow(
        "Simulazione",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!data->window) {
        SDL_Log("Impossibile creare la finestra: %s", SDL_GetError());
        return;
    }

    // create renderer
    data->renderer = SDL_CreateRenderer(data->window, -1, SDL_RENDERER_ACCELERATED);

    if (!data->renderer) {
        SDL_Log("Impossibile creare il renderer: %s", SDL_GetError());
        return;
    }

    // start SDL_TTF. Needed to render text
    if (TTF_Init() < 0) {
        SDL_Log("Impossibile inizializzare TTF: %s", SDL_GetError());
        return;
    }

}

void drawText(SDL_Renderer *renderer, const char *text, int dest_x, int dest_y, SDL_Color color, int size, const char *font_path) {
    // create font object
    TTF_Font *font = TTF_OpenFont(font_path, size);
    if (!font) {
        SDL_Log("Impossibile caricare il font: %s", SDL_GetError());
        return;
    }

    // create surface from text and texture from surface
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // compute destination rectangle and render to it
    int height = textSurface->h; int width = textSurface->w;
    SDL_Rect dest = {dest_x, dest_y, width, height};
    SDL_RenderCopy(renderer, textTexture, nullptr, &dest);

    // clean up
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}
