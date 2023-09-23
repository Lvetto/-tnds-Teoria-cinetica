#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Particle {
    public:

    Particle(float posx, float posy, float velx=0, float vely = 0);

    bool update(int box[4], float dt=1);    // updates particle position, return true if collision with wall happened

    void draw(SDL_Renderer* renderer);      // draws particle on screen

    float pos[2];   // particle position
    float vel[2];   // particle velocity
};
