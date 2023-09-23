#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Particle {
    public:

    Particle(float posx, float posy, float velx=0, float vely = 0);

    void update(int box[4], float dt=1);

    void draw(SDL_Renderer* renderer);

    float pos[2];
    float vel[2];
};
