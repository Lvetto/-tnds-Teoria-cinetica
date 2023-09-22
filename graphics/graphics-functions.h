#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct win_data {   // struct to represent window information. An instance of this should be passed to most functions that draw stuff on screen
    SDL_Window *window;     // pointer to window surface. Currently defined in init() but unused?
    SDL_Renderer *renderer;     // pointer to renderer object. Needed for all rendering functions
};

void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, int ang_steps=100);    // draw a circle

void init(win_data *data, int height, int width);   // initialize SDL, create window and renderer

// draw text on screen. Takes care of surface/texture creation and deletions and renders to window surface.
void drawText(SDL_Renderer *renderer, const char *text, int dest_x, int dest_y, SDL_Color color, int size=20, const char *font_path="Roboto-Regular.ttf");