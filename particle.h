#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Particle {
    public:

    Particle(int posx, int posy, float velx=0, float vely = 0);

    void update(float dt);

    void draw(SDL_Renderer* renderer);

    int pos[2];
    float vel[2];
};