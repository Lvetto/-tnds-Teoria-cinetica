#include <iostream>
//#include <SDL2/SDL.h>     // already included
#include <cmath>
#include "graphics.h"
#include "particle.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>

using namespace std;

std::vector<Button> button_vect;
std::vector<Particle> part_vec;

SDL_Color black = {0, 0, 0, 255};

// stuff to be drawn on screen every cycle
void draw(win_data wdata, int box[4]) {

    // draw the box
    SDL_Rect rect1 = {box[0], box[1], box[2] - box[0], box[3]-box[1]};
    SDL_Rect rect2 = {box[0]+5, box[1]+5, box[2] - box[0] - 10, box[3] - box[1] - 10};
    SDL_SetRenderDrawColor(wdata.renderer, 0, 0, 200, 255); // blue
    SDL_RenderFillRect(wdata.renderer, &rect1);
    SDL_SetRenderDrawColor(wdata.renderer, 0, 0, 0, 255); // black
    SDL_RenderFillRect(wdata.renderer, &rect2);

    // write volume
    int volume = (box[2] - box[0]) * (box[3] - box[1]);
    char v_string[30] = "Volume: ";
    sprintf(v_string + strlen(v_string), "%d", volume);
    drawText(wdata.renderer, v_string, 550, 110, black);

    // write temperature
    int temp = 0;
    for (auto &particle: part_vec) 
        temp += ((particle.vel[0]*particle.vel[0]) + (particle.vel[1]*particle.vel[1]))/part_vec.size();
    char t_string[30] = "Temperatura: ";
    sprintf(t_string + strlen(t_string), "%d", temp);
    drawText(wdata.renderer, t_string, 550, 180, black);

    // write part count
    char p_string[30] = "Particelle: ";
    sprintf(p_string + strlen(p_string), "%d", static_cast<int>(part_vec.size()));
    drawText(wdata.renderer, p_string, 550, 250, black);

    for (auto &button: button_vect) {
            button.draw(wdata.renderer);
        }
    
    for (auto &particle: part_vec) {
            particle.update(box);
            particle.draw(wdata.renderer);
        }    
}

int box[] = {100, 100, 400, 400};

void volp() {
    box[0] -= 5;    box[1] -= 5;
    box[2] += 5;    box[3] += 5;
}

void volm() {
    box[0] += 5;    box[1] += 5;
    box[2] -= 5;    box[3] -= 5;
}

void tempp() {
    for (auto &particle: part_vec) {
            particle.vel[0] *= 1.1;
            particle.vel[1] *= 1.1;
        } 
}

void tempm() {
    for (auto &particle: part_vec) {
            particle.vel[0] /= 1.1;
            particle.vel[1] /= 1.1;
        } 
}

void partp() {
    float v1 = (rand() / static_cast<float>(RAND_MAX)) * 10 - 5;
    float v2 = (rand() / static_cast<float>(RAND_MAX)) * 10 - 5;
    part_vec.push_back(Particle(250, 250, v1, v2));
}

void partm() {
    if (part_vec.size() > 0)
        part_vec.pop_back();
}

int main() {
    win_data wdata;
    init(&wdata, 700, 1000);
    srand(time(NULL));

    button_vect.push_back(Button(750, 100, 50, 50, "+"));
    button_vect.push_back(Button(820, 100, 50, 50, "-"));

    button_vect[0].onClick_func = &volp;
    button_vect[1].onClick_func = &volm;

    button_vect.push_back(Button(750, 170, 50, 50, "+"));
    button_vect.push_back(Button(820, 170, 50, 50, "-"));

    button_vect[2].onClick_func = &tempp;
    button_vect[3].onClick_func = &tempm;

    button_vect.push_back(Button(750, 240, 50, 50, "+"));
    button_vect.push_back(Button(820, 240, 50, 50, "-"));

    button_vect[4].onClick_func = &partp;
    button_vect[5].onClick_func = &partm;

    /* part_vec.push_back(Particle(250, 250, 2, 1));
    part_vec.push_back(Particle(250, 250, 3, 2));
    part_vec.push_back(Particle(250, 250, -4, 3));
    part_vec.push_back(Particle(250, 250, 1, -4));
    part_vec.push_back(Particle(250, 250, -3, -2)); */


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
        SDL_SetRenderDrawColor(wdata.renderer, 170, 170, 170, 255); // grigio
        SDL_RenderClear(wdata.renderer);

        // draw stuff
        draw(wdata, box);

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
