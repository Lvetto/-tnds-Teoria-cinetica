#include <iostream>
//#include <SDL2/SDL.h>     // already included
#include <cmath>
#include "graphics.h"
#include "particle.h"
#include <vector>

using namespace std;

std::vector<Button> button_vect;
std::vector<Particle> part_vec;

// stuff to be drawn on screen every cycle
void draw(win_data wdata) {
    for (auto &button: button_vect) {
            button.draw(wdata.renderer);
        }
    
    for (auto &particle: part_vec) {
            particle.update(1);
            particle.draw(wdata.renderer);
        }
}

int main() {
    win_data wdata;
    init(&wdata, 500, 800);

    // main loop. Could get quite long, maybe move to a separate function?
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                quit = true;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;
                    for (auto &button: button_vect) button.checkClick(mouseX, mouseY);
                }
            }

            if (event.type == SDL_MOUSEMOTION) {
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;
                //SDL_Log("Posizione cursore: (%d, %d)", mouseX, mouseY);
                for (auto &button: button_vect) button.checkSelect(mouseX, mouseY);
                   
            }
        }

        // clear window
        SDL_SetRenderDrawColor(wdata.renderer, 0, 0, 0, 255); // nero
        SDL_RenderClear(wdata.renderer);

        // draw stuff
        draw(wdata);

        // update window content
        SDL_RenderPresent(wdata.renderer);
        cout << SDL_GetError();     // Debug!!!

        // add some delay. Should be replaced with some proper time tracking to ensure uniform time cycles
        SDL_Delay(10);
    }

    // de-initialize SDL
    SDL_DestroyRenderer(wdata.renderer);
    SDL_DestroyWindow(wdata.window);
    SDL_Quit();

    return 0;
}
