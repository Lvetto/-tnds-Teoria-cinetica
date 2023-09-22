#include "graphics.h"
#include "particle.h"

Particle::Particle(int posx, int posy, float velx, float vely)  {
        this->pos[0] = posx;;
        this->pos[1] = posy;
        this->vel[0] = velx;
        this->vel[1] = vely;
        //part_vec.push_back(this);
}

void Particle::update(float dt) {
        if (pos[0]>=800) {
            vel[0]*=-1;
            pos[0]-=10;
        }
        if (pos[0]<=0) {
            vel[0]*=-1;
            pos[0]+=10;
        }

        pos[0] = static_cast<int>(pos[0] + vel[0] * dt);
        
        if (pos[1]>=500) {
            vel[1]*=-1;
            pos[1]-=10;
        }
        if (pos[1]<=0) {
            vel[1]*=-1;
            pos[1]+=10;
        }
        pos[1] = static_cast<int>(pos[1] + vel[1] * dt);
}

void Particle::draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 210, 0, 0, 255);   // rosso
    drawCircle(renderer, pos[0], pos[1], 10);
}

