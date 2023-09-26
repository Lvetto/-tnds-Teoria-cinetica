#include "graphics.h"
#include "particle.h"

Particle::Particle(float posx, float posy, float velx, float vely)  {
        this->pos[0] = posx;;
        this->pos[1] = posy;
        this->vel[0] = velx;
        this->vel[1] = vely;
}

bool Particle::update(int box[4], float dt) {
        bool collision = false;
        // check if particle is in box then, update the position. If the particle is outside the box, invert the velocity and "teleport" it back a little. Box format is [x0, y0, x1, y1]
        if (pos[0]>=box[2]) {   // condense in one check using sgn(vel[0]) to get direction for correction!!!!!!!!!!!!!!
            vel[0]*=-1;
            pos[0]-=10;
            collision = true;
        }
        if (pos[0]<=box[0]) {
            vel[0]*=-1;
            pos[0]+=10;
            collision = true;
        }

        pos[0] += vel[0] * dt;
        
        if (pos[1]>=box[3]) {
            vel[1]*=-1;
            pos[1]-=10;
            collision = true;
        }
        if (pos[1]<=box[1]) {
            vel[1]*=-1;
            pos[1]+=10;
            collision = true;
        }
        pos[1] += vel[1] * dt;
        return collision;
}

void Particle::draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 210, 0, 0, 255);   // rosso
    drawCircle(renderer, static_cast<int>(pos[0]), static_cast<int>(pos[1]), 10, 80);
}
