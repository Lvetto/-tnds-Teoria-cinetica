#include <iostream>
#include <cmath>
#include "graphics.h"
//#include "particle.h"     already included in functions.h
#include "functions.h"
#include <vector>   // already included in functions.h
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
std::vector<float> temp_vec;

// define colors used in the code
SDL_Color black = {0, 0, 0, 255};
SDL_Color red = {200, 0, 0, 255};
SDL_Color green = {0, 150, 0, 255};
SDL_Color blue = {0, 0, 200, 255};

// constants used to control the framerate
const int targetFPS = 60;
const int frameDelay = 1000 / targetFPS;

// used to represent program state. false == running, true == should close
bool quit = false;

// stuff to be drawn on screen every cycle
void draw(win_data wdata, float box[4]);

// vertices of the box containing the particles. {x_0, y_0, x_1, y_1}
float box[] = {100, 200, 400, 500};
float box_vel[] = {0, 0, 0, 0};
bool internal_collisions = true;

// functions used by the buttons
void volume_plus();
void volume_minus();
void temp_plus();
void temp_minus();
void particle_plus();
void particle_minus();

// function used to draw graphs. Not in graphics_functions to avoid making (more of) a mess with includes
void DrawGraph(int p0[2], int h, vector<float> v, win_data wdata, char title[20], SDL_Color color=blue);

// function used in the mainloop to check events
void HandleEvents();

int main() {
    // initialize SDL and create window data
    win_data wdata;
    init(&wdata, 700, 1500);

    // set seed for rand()
    srand(time(NULL));

    // create buttons and assign functions
    button_vect.push_back(Button(750, 200, 50, 50, "+"));   // volume+
    button_vect.push_back(Button(820, 200, 50, 50, "-"));   // volume-

    button_vect[0].onClick_func = &volume_plus;
    button_vect[1].onClick_func = &volume_minus;

    button_vect.push_back(Button(750, 270, 50, 50, "+"));   // temperature+
    button_vect.push_back(Button(820, 270, 50, 50, "-"));   // temperature-

    button_vect[2].onClick_func = &temp_plus;
    button_vect[3].onClick_func = &temp_minus;

    button_vect.push_back(Button(750, 340, 50, 50, "+"));   // particle+
    button_vect.push_back(Button(820, 340, 50, 50, "-"));   // particle-

    button_vect[4].onClick_func = &particle_plus;
    button_vect[5].onClick_func = &particle_minus;

    // main loop
    while (!quit) {
        Uint32 frameStart = SDL_GetTicks();     // get time at the start of a frame

        HandleEvents();

        // clear window
        SDL_SetRenderDrawColor(wdata.renderer, 170, 170, 170, 255); // grigio
        SDL_RenderClear(wdata.renderer);

        // draw stuff
        draw(wdata, box);

        // update window content
        SDL_RenderPresent(wdata.renderer);

        // print last error. SDL does not do so by default
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
    // if there is at least one particle, initial velocity should be chosen taking in consideration average velocity of the others
    float avg_vel;
    if (part_vec.size() > 0)
        avg_vel = pow(average(part_vec, &speed_mod2), 0.5);
    else
        avg_vel = 5;

    // create new particle. Initial velocity should have random direction and random modulus chosen from an appropriate interval
    float v1 = (rand() / static_cast<float>(RAND_MAX)) * 2 * avg_vel - avg_vel;
    float v2 = (rand() / static_cast<float>(RAND_MAX)) * 2 * avg_vel - avg_vel;
    part_vec.push_back(Particle(250, 350, v1, v2));
}

void particle_minus() {
    if (part_vec.size() > 0)
        part_vec.pop_back();
}

vector<float> avg_vec;      // used to average pressure over several cycles to reduce irregularities in graph. Not the best solution!

void draw(win_data wdata, float box[4]) {

    // draw the box
    float center_x = (box[2]-box[0]) / 2;
    float center_y = (box[3]-box[1]) / 2;
    float width = box[2] - box[0];
    float height = box[3] - box[1];
    float correction_x = -0.2 * (box[0] - (250 - width / 2));
    float correction_y = -0.2 * (box[1] - (350 - height / 2));
    box_vel[0] += correction_x; box_vel[2] += correction_x;
    box_vel[1] += correction_y; box_vel[3] += correction_y;
    SDL_Rect rect1 = {static_cast<int>(box[0]), static_cast<int>(box[1]), static_cast<int>(width), static_cast<int>(height)};
    SDL_Rect rect2 = {static_cast<int>(box[0]+5), static_cast<int>(box[1]+5), static_cast<int>(width - 10), static_cast<int>(height - 10)};
    SDL_SetRenderDrawColor(wdata.renderer, 0, 0, 200, 255); // blue
    SDL_RenderFillRect(wdata.renderer, &rect1);
    SDL_SetRenderDrawColor(wdata.renderer, 0, 0, 0, 255); // black
    SDL_RenderFillRect(wdata.renderer, &rect2);

    // compute and display volume
    int volume = (box[2] - box[0]) * (box[3] - box[1]);
    char v_string[30] = "Volume: ";
    sprintf(v_string + strlen(v_string), "%d", volume);
    drawText(wdata.renderer, v_string, 550, 210, black);

    // compute and display temperature
    int temp = average(part_vec, &speed_mod2, 0, -1);
    char t_string[30] = "Temperatura: ";
    sprintf(t_string + strlen(t_string), "%d", temp);
    drawText(wdata.renderer, t_string, 550, 280, black);

    temp_vec.push_back(temp);
    if (temp_vec.size() > 500)
        temp_vec.erase(temp_vec.begin());

    // compute and display part count
    char p_string[30] = "Particelle: ";
    sprintf(p_string + strlen(p_string), "%d", static_cast<int>(part_vec.size()));
    drawText(wdata.renderer, p_string, 550, 350, black);

    // draw buttons
    for (auto &button: button_vect) {
            button.draw(wdata.renderer);
        }
    
    // update particle position and compute pressure
    float dq = 0;
    for (auto &particle: part_vec) {
            if (particle.update(box, 1, box_vel))
                dq += 2 * pow(pow(particle.vel[0], 2) + pow(particle.vel[1], 2), 2);    // pressure is (proportional to) the total change in momentum
            particle.draw(wdata.renderer);
        }
    float pressure = dq / (2 * (box[2] - box[0]) + 2 * (box[3] - box[1]));

    /* if (internal_collisions) {
        vector<Particle*> pairs = generateUniquePairs(part_vec);
        for (auto &pair: pairs) {
            if (distance(pair[0], pair[1]) < 10) {
                float alpha = atan2(pair[0].pos[1] - pair[1].pos[1], pair[0].pos[0] - pair[1].pos[0]);

                float v_par1 = pair[0].vel[1] * sin(alpha) + pair[0].vel[0] * cos(alpha);
                float v_par2 = pair[1].vel[1] * sin(alpha) + pair[1].vel[0] * cos(alpha);

                float v_perp1 = pair[0].vel[1] * cos(alpha) + pair[0].vel[0] * sin(alpha);
                float v_perp2 = pair[1].vel[1] * cos(alpha) + pair[1].vel[0] * sin(alpha);

                float* new_vels = solve_collision(v_par1, v_par2);

                float mod1 = pow(pow(v_perp1, 2) + pow(new_vels[0], 2), 0.5);
                pair[0].vel[0] = mod1 * cos(alpha);
                pair[0].vel[1] = mod1 * sin(alpha);
            }
        }
    } */

    //printf("%f,%f,%f,%f\t", box_vel[0], box_vel[1], box_vel[2], box_vel[3]);
    //printf("%f,%f,%f,%f\n", box[0], box[1], box[2], box[3]);

    box[0] += box_vel[0];
    box[1] += box_vel[1];
    box[2] += box_vel[2];
    box[3] += box_vel[3];

    float F = 0.001;
    box_vel[0] += F;
    box_vel[1] += F;
    box_vel[2] -= F;
    box_vel[3] -= F;

    /* box_vel[0] += -0.05 * box_vel[0];
    box_vel[1] += -0.05 * box_vel[1];
    box_vel[2] += -0.05 * box_vel[2];
    box_vel[3] += -0.05 * box_vel[3]; */



    // add pressure to vector, limit vector size to 300 (5 seconds)
    press_vec.push_back(pressure);
    if (press_vec.size() > 500)
        press_vec.erase(press_vec.begin());
    
    // write pressure to screen. The value displayed is the average over the last 5 seconds
    pressure = average(press_vec);
    char pr_string[30] = "Pressione: ";
    sprintf(pr_string + strlen(pr_string), "%.2f", pressure);
    drawText(wdata.renderer, pr_string, 550, 420, black);

    vol_vec.push_back(volume);
    if (vol_vec.size() > 500)
        vol_vec.erase(vol_vec.begin());
    
    // prevent division by zero
    if (max(press_vec) != 0) {
        // pressure values need to be averaged. If vector is shorter than 50, average can not be accurately computed. This reduces oscillations in values in the first frames that would mess up scaling for a few seconds
        if (press_vec.size() > 50)
            avg_vec.push_back(average(press_vec, press_vec.size() - 120));

        // limit amount of values stored
        if (avg_vec.size() > 500)
            avg_vec.erase(avg_vec.begin());
    }

    // draw graphs
    int p0_press[] = {950, 200};
    char title1[] = "Pressione";
    DrawGraph(p0_press, 150, avg_vec, wdata, title1, red);

    int p0_vol[] = {950, 430};
    char title2[] = "Volume";
    DrawGraph(p0_vol, 150, vol_vec, wdata, title2, green);

    int p0_temp[] = {950, 660};
    char title3[] = "Temperatura";
    DrawGraph(p0_temp, 150, temp_vec, wdata, title3, blue);
}

// draw a graph. Height is h+50. Width is 560
void DrawGraph(int p0[2], int h, vector<float> v, win_data wdata, char title[20], SDL_Color color) {

    // draw rectangle around graph
    SDL_Rect r = {p0[0]-30, p0[1]-30-h, 560, h+50};
    SDL_SetRenderDrawColor(wdata.renderer, 190, 190, 190, 255); // grigino
    SDL_RenderFillRect(wdata.renderer, &r);

    SDL_SetRenderDrawColor(wdata.renderer, 0, 0, 0, 255); // nero

    // axis
    SDL_RenderDrawLine(wdata.renderer, p0[0], p0[1] + 15, p0[0], p0[1] - h);
    SDL_RenderDrawLine(wdata.renderer, p0[0] - 20, p0[1], p0[0] + 500, p0[1]);

    // arrow on x axis
    SDL_RenderDrawLine(wdata.renderer, p0[0], p0[1] - h, p0[0] + 10, p0[1] - h + 10);
    SDL_RenderDrawLine(wdata.renderer, p0[0], p0[1] - h, p0[0] - 10, p0[1] - h + 10);

    // arrow on y axis
    SDL_RenderDrawLine(wdata.renderer, p0[0] + 500, p0[1], p0[0] + 490, p0[1] - 10);
    SDL_RenderDrawLine(wdata.renderer, p0[0] + 500, p0[1], p0[0] + 490, p0[1] + 10);

    // write title
    drawText(wdata.renderer, title, p0[0] - 20, p0[1] - h - 25, black);

    // draw points
    SDL_SetRenderDrawColor(wdata.renderer, color.r, color.g, color.b, color.a);
    for (int i=0; i<v.size(); i++) {
            int y = static_cast<int>(p0[1] - h * (v[i] / max(v)));
            drawCircle(wdata.renderer, p0[0] + i, y, 3, 2);
        }

}

void HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {     // iterate trough events and store them in "event"

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
                for (auto &button: button_vect) button.checkSelect(mouseX, mouseY);
            }
    }
}
