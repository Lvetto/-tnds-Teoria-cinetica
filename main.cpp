#include <iostream>     // only actually used to display errors, should be debug only?
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
vector<float> avg_vec;      // used to average pressure over several cycles to reduce irregularities in graph. Not ideal!

// define colors used in the code
SDL_Color black = {0, 0, 0, 255};
SDL_Color red = {200, 0, 0, 255};
SDL_Color green = {0, 150, 0, 255};
SDL_Color blue = {0, 0, 200, 255};
SDL_Color gray1 = {170, 170, 170, 255};
SDL_Color gray2 = {190, 190, 190, 255};

// constants used to control the framerate
const int targetFPS = 60;
const int frameDelay = 1000 / targetFPS;

bool quit = false;  // used to represent program state. false == running, true == should close on next cycle

void draw(win_data wdata);    // stuff to be drawn on screen and updated every cycle

float box[] = {100, 200, 400, 500};     // vertices of the box containing the particles. {x_0, y_0, x_1, y_1}
float box_vel[] = {0, 0, 0, 0};     // side walls velocity

float ext_force = 0;    // force to be applied to the walls
float wall_mass = 10000;    // mass of the walls. Used to solve collisions. Low values work well when applying external forces, higher values are needed if we don't want the walls to move

// functions used by the buttons
void volume_plus();
void volume_minus();
void temp_plus();
void temp_minus();
void particle_plus();
void particle_minus();
void ext_press_plus();
void ext_press_minus();
void wall_mass_plus();
void wall_mass_minus();

void DrawGraph(int p0[2], int h, vector<float> v, win_data wdata, char title[20], SDL_Color color=blue);    // function used to draw graphs. Not in graphics_functions to avoid making (more of) a mess with includes

void HandleEvents();    // function used in the mainloop to check events

int main() {
    // initialize SDL and create window data
    win_data wdata;
    init(&wdata, 700, 1500);

    srand(time(NULL));  // set seed for rand(). Used to create new particles

    // create buttons and assign functions
    button_vect.push_back(Button(750, 150, 50, 50, "+"));   // volume+
    button_vect.push_back(Button(820, 150, 50, 50, "-"));   // volume-

    button_vect[0].onClick_func = &volume_plus;
    button_vect[1].onClick_func = &volume_minus;

    button_vect.push_back(Button(750, 220, 50, 50, "+"));   // temperature+
    button_vect.push_back(Button(820, 220, 50, 50, "-"));   // temperature-

    button_vect[2].onClick_func = &temp_plus;
    button_vect[3].onClick_func = &temp_minus;

    button_vect.push_back(Button(750, 290, 50, 50, "+"));   // particle+
    button_vect.push_back(Button(820, 290, 50, 50, "-"));   // particle-

    button_vect[4].onClick_func = &particle_plus;
    button_vect[5].onClick_func = &particle_minus;

    button_vect.push_back(Button(750, 430, 50, 50, "+"));   // external pressure+
    button_vect.push_back(Button(820, 430, 50, 50, "-"));   // external pressure-

    button_vect[6].onClick_func = &ext_press_plus;
    button_vect[7].onClick_func = &ext_press_minus;

    button_vect.push_back(Button(750, 500, 50, 50, "+"));   // wall mass+
    button_vect.push_back(Button(820, 500, 50, 50, "-"));   // wall mass-

    button_vect[8].onClick_func = &wall_mass_plus;
    button_vect[9].onClick_func = &wall_mass_minus;


    // main loop
    while (!quit) {
        Uint32 frameStart = SDL_GetTicks();     // get time at the start of a frame

        HandleEvents();     // process events

        // clear window
        SDL_SetRenderDrawColor(wdata.renderer, gray1.r, gray1.g, gray1.b, 255);
        SDL_RenderClear(wdata.renderer);

        draw(wdata);   // draw and update stuff

        SDL_RenderPresent(wdata.renderer);  // update window content

        cout << SDL_GetError();     // print last error. SDL does not do so by default. Should be debug only?

        // compute delay to limit framerate to 60 fps, if needed
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
    float avg_vel;
    if (part_vec.size() > 0)    // if there is at least one particle, initial velocity should be chosen taking in consideration average velocity of the others
        avg_vel = pow(average(part_vec, &speed_mod2), 0.5);
    else
        avg_vel = 5;    // default max component value for first particle

    // create new particle. Initial velocity should have random direction and random mod chosen from an appropriate interval. Component value can also be negative
    float v1 = (rand() / static_cast<float>(RAND_MAX)) * 2 * avg_vel - avg_vel;
    float v2 = (rand() / static_cast<float>(RAND_MAX)) * 2 * avg_vel - avg_vel;

    part_vec.push_back(Particle(250, 350, v1, v2));     // add particle to vector so that it can be updated
}

void particle_minus() {
    if (part_vec.size() > 0)
        part_vec.pop_back();
}

// change external force value
void ext_press_plus() {
    if (ext_force == 0)     // allows the value to start from zero
        ext_force += 1;
    else
        ext_force *= 1.1;
}

void ext_press_minus() {
    if (ext_force <= 0.5)   // allows to set the value to exactly zero
        ext_force = 0;
    else
        ext_force /= 1.1;
}

// change wall mass
void wall_mass_plus() {
    if (wall_mass <= 900000)    // value needs to be limited so it can be shown on screen without covering other parts of the interface. Higher values also create problems with wall interactions
        wall_mass *= 1.1;
}

void wall_mass_minus() {
    wall_mass /= 1.1;
}


void draw(win_data wdata) {
    // draw the box
    float width = box[2] - box[0];
    float height = box[3] - box[1];

    SDL_Rect rect1 = {static_cast<int>(box[0]), static_cast<int>(box[1]), static_cast<int>(width), static_cast<int>(height)};
    SDL_Rect rect2 = {static_cast<int>(box[0]+5), static_cast<int>(box[1]+5), static_cast<int>(width - 10), static_cast<int>(height - 10)};

    SDL_SetRenderDrawColor(wdata.renderer, blue.r, blue.g, blue.b, 255);
    SDL_RenderFillRect(wdata.renderer, &rect1);
    SDL_SetRenderDrawColor(wdata.renderer, black.r, black.g, black.b, 255);
    SDL_RenderFillRect(wdata.renderer, &rect2);

    // compute and apply acceleration to keep the box centered. We want top left corner to move toward the point that centers the box around (250, 350).
    // acceleration is proportional to the distance from target point. Constant should be small enough to allow some movement, but big enough to prevent the box from going off screen
    float center_x = (box[2]-box[0]) / 2;
    float center_y = (box[3]-box[1]) / 2;

    float correction_x = -0.05 * (box[0] - (250 - width / 2));
    float correction_y = -0.05 * (box[1] - (350 - height / 2));

    box_vel[0] += correction_x; box_vel[2] += correction_x;     // apply to vertical walls
    box_vel[1] += correction_y; box_vel[3] += correction_y;     // apply to horizontal walls

    // update wall positions
    if (box[2]-box[0] > 10) {   // only update if box is big enough. Otherwise with a big enough external force box size could become negative
        box[0] += box_vel[0];
        box[2] += box_vel[2];

        box_vel[0] += ext_force/wall_mass;
        box_vel[2] -= ext_force/wall_mass;

    }   else {  // if box is too small, make it a bit bigger. This prevents the box from getting stuck at the minimum size
        box[0] -= 1;
        box[2] += 1;
    }
    
    if (box[3]-box[1]>10) {
        box[1] += box_vel[1];
        box[3] += box_vel[3];

        box_vel[1] += ext_force/wall_mass;
        box_vel[3] -= ext_force/wall_mass;
    }   else {
        box[1] -= 1;
        box[3] += 1;
    }

    // compute status variables
    int volume = (box[2] - box[0]) * (box[3] - box[1]);     // volume = area of the box
    float temp = average(part_vec, &speed_mod2, 0, -1);     // temperature is proportional to average squared velocity. Constants are omitted
    float dq = 0;   // pressure is computed trough the definition as Force/Surface. Force is approximated as delta_q/delta_t (change in momentum over time)
    for (auto &particle: part_vec) {
            dq += particle.update(box, box_vel, wall_mass);     // to compute pressure particles need to be updated
            particle.draw(wdata.renderer);      // particles are drawn in the same loop to avoid having to loop over the vector twice
    }
    float pressure = 100 * dq / (2 * (box[2] - box[0]) + 2 * (box[3] - box[1]));

    // add values to their respective vectors. These values are then used to draw graphs
    temp_vec.push_back(temp);
    press_vec.push_back(pressure);
    vol_vec.push_back(volume);

    // limit vectors size to around 10 seconds of data. If vectors are longer first element is removed
    // vector sizes should always be the same, this could probably be replaced with only one if-statement
    if (temp_vec.size() > 500)
        temp_vec.erase(temp_vec.begin());

    if (press_vec.size() > 500)
        press_vec.erase(press_vec.begin());

    if (vol_vec.size() > 500)
        vol_vec.erase(vol_vec.begin());

    // create char arrays to represent text to be displayed on screen
    char v_string[30] = "Volume: ";
    char t_string[30] = "Temperatura: ";
    char p_string[30] = "Particelle: ";
    char f_string[30] = "Forza esterna: ";
    char m_string[30] = "Massa parete: ";
    char pr_string[30] = "Pressione: ";

    // append values to the end of the arrays
    sprintf(v_string + strlen(v_string), "%d", volume);
    sprintf(t_string + strlen(t_string), "%.2f", temp);
    sprintf(p_string + strlen(p_string), "%d", static_cast<int>(part_vec.size()));      // vector size is apparently not an int
    sprintf(f_string + strlen(f_string), "%.0f", 100*ext_force);
    sprintf(m_string + strlen(m_string), "%.0f", wall_mass);
    pressure = average(press_vec);  // pressure is averaged to reduce oscillations
    sprintf(pr_string + strlen(pr_string), "%.2f", pressure);

    // draw text on window
    drawText(wdata.renderer, v_string, 550, 160, black);
    drawText(wdata.renderer, t_string, 550, 230, black);
    drawText(wdata.renderer, p_string, 550, 300, black);
    drawText(wdata.renderer, pr_string, 550, 370, black);
    drawText(wdata.renderer, f_string, 550, 440, black);
    drawText(wdata.renderer, m_string, 550, 510, black);

    // draw buttons
    for (auto &button: button_vect)
            button.draw(wdata.renderer);
    
    // pressure values change wildly across frames. This creates a lot of problems when drawing graphs
    // to mitigate this, another vector is used to keep track of average pressure values over last 50 frames
    if (max(press_vec) != 0) {      // pressure is non-negative. If max pressure over last 500 frames is 0, all values are 0 and pressure is not well defined
        if (press_vec.size() > 50)  // averaging over less frames gives less stable results and can create problems when scaling graphs
            avg_vec.push_back(average(press_vec, press_vec.size() - 120));  // take average over 2 seconds. Average function takes care of shorter vectors

        // limit amount of values stored
        if (avg_vec.size() > 500)
            avg_vec.erase(avg_vec.begin());
    }

    // graph titles
    char title1[] = "Pressione";
    char title2[] = "Volume";
    char title3[] = "Temperatura";

    // graph positions
    int p0_press[] = {950, 200};
    int p0_vol[] = {950, 430};
    int p0_temp[] = {950, 660};    

    // draw graphs
    DrawGraph(p0_press, 150, avg_vec, wdata, title1, red);  // pressure
    DrawGraph(p0_vol, 150, vol_vec, wdata, title2, green);  // volume
    DrawGraph(p0_temp, 150, temp_vec, wdata, title3, blue); // temperature
}

// draw a graph. Height is h+50. Width is 560
void DrawGraph(int p0[2], int h, vector<float> v, win_data wdata, char title[20], SDL_Color color) {

    // draw rectangle around graph that is a slightly different color from the 
    SDL_SetRenderDrawColor(wdata.renderer, gray2.r, gray2.g, gray2.b, 255);
    SDL_Rect r = {p0[0]-30, p0[1]-30-h, 560, h+50};
    SDL_RenderFillRect(wdata.renderer, &r);

    SDL_SetRenderDrawColor(wdata.renderer, black.r, black.g, black.b, 255);

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
            drawCircle(wdata.renderer, p0[0] + i, y, 3, 2);     // a circle with ang_steps=2 looks good to represent data points and is very quick to draw
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
