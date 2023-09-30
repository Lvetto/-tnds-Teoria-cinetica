#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Particle {
    public:

    Particle(float posx, float posy, float velx=0, float vely = 0);

    float update(float box[4], float *box_vel, float wall_mass);    // updates particle position, return change in momentum

    void draw(SDL_Renderer* renderer);      // draw particle on screen

    float pos[2];   // particle position
    float vel[2];   // particle velocity
};

// used to compute velocities after collision with the walls
float* solve_collision(float v1, float v2, float m1=1, float m2=1);
