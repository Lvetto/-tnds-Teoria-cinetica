#include <iostream>
#include <cmath>
#include "graphics.h"
#include "particle.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>

using namespace std;

// vectors used to keep track of buttons and particles
std::vector<Button> button_vect;
std::vector<Particle> part_vec;

// vectors used to keep track of past values of variables
std::vector<float> press_vec;
std::vector<float> vol_vec;

// define colors used in the code
SDL_Color black = {0, 0, 0, 255};

// constants used to control the framerate
const int targetFPS = 60;
const int frameDelay = 1000 / targetFPS;

// stuff to be drawn on screen every cycle
void draw(win_data wdata, int box[4]);

// vertices of the box containing the particles. {x_0, y_0, x_1, y_1}
int box[] = {100, 100, 400, 400};

// functions used by the buttons
void volume_plus();
void volume_minus();
void temp_plus();
void temp_minus();
void particle_plus();
void particle_minus();

float max(vector<float> v) {
    float out = v[0];
    for (auto &i: v)
        if (i > out)
            out = i;
    return out;
}

template <typename type>
float average(vector<type> v, int start=0, int end=-1, float (*func)(type)=nullptr) {
    if (end < start)
        end = v.size() - 1;
    float out = 0;
    for (int i=start; i <= end; i++) {
        if (func)
            out += func(v[i]) / static_cast<float>(end - start);
        else
            out += static_cast<float>(v[i]) / static_cast<float>(end - start);
    }
    return out;    
}

int main() {
    // initialize SDL and create window data
    win_data wdata;
    init(&wdata, 700, 1500);
    srand(time(NULL));  // set "random" seed for rand()

    // create buttons and assign functions
    button_vect.push_back(Button(750, 100, 50, 50, "+"));   // volume+
    button_vect.push_back(Button(820, 100, 50, 50, "-"));   // volume-

    button_vect[0].onClick_func = &volume_plus;
    button_vect[1].onClick_func = &volume_minus;

    button_vect.push_back(Button(750, 170, 50, 50, "+"));   // temperature+
    button_vect.push_back(Button(820, 170, 50, 50, "-"));   // temperature-

    button_vect[2].onClick_func = &temp_plus;
    button_vect[3].onClick_func = &temp_minus;

    button_vect.push_back(Button(750, 240, 50, 50, "+"));   // particle+
    button_vect.push_back(Button(820, 240, 50, 50, "-"));   // particle-

    button_vect[4].onClick_func = &particle_plus;
    button_vect[5].onClick_func = &particle_minus;

    // main loop. Could get quite long, maybe move to a separate function?
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        Uint32 frameStart = SDL_GetTicks();     // start timer used to limit the framerate
        while (SDL_PollEvent(&event)) {

            // check if window is closed
            if (event.type == SDL_QUIT) {
                quit = true;
            }

            // check if buttons are clicked
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;
                    for (auto &button: button_vect) button.checkClick(mouseX, mouseY);
                }
            }

            // check if buttons are selected
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

        // limit framerate to 60 fps
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay)
            SDL_Delay(frameDelay - frameTime);
    }

    // de-initialize SDL
    SDL_DestroyRenderer(wdata.renderer);
    SDL_DestroyWindow(wdata.window);
    SDL_Quit();

    return 0;
}

// increase or decrease box volume by moving vertices
void volume_plus() {
    box[0] -= 5;    box[1] -= 5;
    box[2] += 5;    box[3] += 5;
}

void volume_minus() {
    box[0] += 5;    box[1] += 5;
    box[2] -= 5;    box[3] -= 5;
}

// increase or decrease temperature by changing particle velocity
void temp_plus() {
    for (auto &particle: part_vec) {
            particle.vel[0] *= 1.1;
            particle.vel[1] *= 1.1;
        } 
}

void temp_minus() {
    for (auto &particle: part_vec) {
            particle.vel[0] /= 1.1;
            particle.vel[1] /= 1.1;
        } 
}

// create or destroy new particles by editing part_vec
void particle_plus() {
    float v1 = (rand() / static_cast<float>(RAND_MAX)) * 10 - 5;
    float v2 = (rand() / static_cast<float>(RAND_MAX)) * 10 - 5;
    part_vec.push_back(Particle(250, 250, v1, v2));
}

void particle_minus() {
    if (part_vec.size() > 0)
        part_vec.pop_back();
}

vector<float> avg_vec;

void draw(win_data wdata, int box[4]) {

    // draw the box
    SDL_Rect rect1 = {box[0], box[1], box[2] - box[0], box[3]-box[1]};
    SDL_Rect rect2 = {box[0]+5, box[1]+5, box[2] - box[0] - 10, box[3] - box[1] - 10};
    SDL_SetRenderDrawColor(wdata.renderer, 0, 0, 200, 255); // blue
    SDL_RenderFillRect(wdata.renderer, &rect1);
    SDL_SetRenderDrawColor(wdata.renderer, 0, 0, 0, 255); // black
    SDL_RenderFillRect(wdata.renderer, &rect2);

    // compute and display volume
    int volume = (box[2] - box[0]) * (box[3] - box[1]);
    char v_string[30] = "Volume: ";
    sprintf(v_string + strlen(v_string), "%d", volume);
    drawText(wdata.renderer, v_string, 550, 110, black);

    // compute and display temperature
    int temp = 0;
    for (auto &particle: part_vec) 
        temp += ((particle.vel[0]*particle.vel[0]) + (particle.vel[1]*particle.vel[1]))/part_vec.size();    // temperature is (proportional to) the average squared speed
    char t_string[30] = "Temperatura: ";
    sprintf(t_string + strlen(t_string), "%d", temp);
    drawText(wdata.renderer, t_string, 550, 180, black);

    // compute and display part count
    char p_string[30] = "Particelle: ";
    sprintf(p_string + strlen(p_string), "%d", static_cast<int>(part_vec.size()));
    drawText(wdata.renderer, p_string, 550, 250, black);

    // draw buttons
    for (auto &button: button_vect) {
            button.draw(wdata.renderer);
        }
    
    // update particle position and compute pressure
    float dq = 0;
    for (auto &particle: part_vec) {
            if (particle.update(box))
                dq += 2 * pow(pow(particle.vel[0], 2) + pow(particle.vel[1], 2), 2);    // pressure is (proportional to) the total change in momentum
            particle.draw(wdata.renderer);
        }
    float pressure = dq / (2 * (box[2] - box[0]) + 2 * (box[3] - box[1]));
    
    // add pressure to vector, limit vector size to 300 (5 seconds)
    press_vec.push_back(pressure);
    if (press_vec.size() > 500)
        press_vec.erase(press_vec.begin());
    
    // write pressure to screen. The value displayed is the average over the last 5 seconds
    pressure = average(press_vec);
    char pr_string[30] = "Pressione: ";
    sprintf(pr_string + strlen(pr_string), "%.2f", pressure);
    drawText(wdata.renderer, pr_string, 550, 320, black);

    vol_vec.push_back(volume);
    if (vol_vec.size() > 300)
        vol_vec.erase(vol_vec.begin());
    

    if (max(press_vec) != 0) {
        for (int i=50; i<press_vec.size(); i++) {
            float avg = average(press_vec, i-50, i);
            int y1 = static_cast<int>(200 - 400 * (avg / max(press_vec)));
            //int y2 = static_cast<int>(600 - 200 * (avg / (2*average(press_vec))));
            SDL_SetRenderDrawColor(wdata.renderer, 0, 200, 0, 255); // verde
            //SDL_RenderDrawPoint(wdata.renderer, x, y);
            drawCircle(wdata.renderer, 950 + i, y1, 3, 2);
            //SDL_SetRenderDrawColor(wdata.renderer, 0, 0, 200, 255); // blu
            //drawCircle(wdata.renderer, 1000 + i, y2, 3, 2);
        }

        if (press_vec.size() > 50)
            avg_vec.push_back(average(press_vec, press_vec.size() - 120));
        if (avg_vec.size() > 500)
            avg_vec.erase(avg_vec.begin());
        for (int i=0; i<avg_vec.size(); i++) {
            int y = static_cast<int>(600 - 200 * (avg_vec[i] / max(avg_vec)));
            SDL_SetRenderDrawColor(wdata.renderer, 0, 0, 200, 255); // blu
            drawCircle(wdata.renderer, 1000 + i, y, 3, 2);

        }
    }
}
