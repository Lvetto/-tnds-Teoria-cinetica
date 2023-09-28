#include "graphics.h"
#include "particle.h"

Particle::Particle(float posx, float posy, float velx, float vely)  {
        this->pos[0] = posx;;
        this->pos[1] = posy;
        this->vel[0] = velx;
        this->vel[1] = vely;
}

bool Particle::update(float box[4], float dt, float *box_vel) {
        bool collision = false;
        float m2 = 100;
        // check if particle is in box then, update the position. If the particle is outside the box, solve the collision and "teleport" it back a little. Box format is [x0, y0, x1, y1]
        if (pos[0]>=box[2]) {
            float *new_vels = solve_collision(vel[0], box_vel[2], 1, m2);

            vel[0] = new_vels[0];
            box_vel[2] = new_vels[1];

            pos[0]-=10;

            collision = true;
        }
        if (pos[0]<=box[0]) {
            float *new_vels = solve_collision(vel[0], box_vel[0], 1, m2);

            vel[0] = new_vels[0];
            box_vel[0] = new_vels[1];

            pos[0]+=10;

            collision = true;
        }

        pos[0] += vel[0] * dt;
        
        if (pos[1]>=box[3]) {
            float *new_vels = solve_collision(vel[1], box_vel[3], 1, m2);

            vel[1] = new_vels[0];
            box_vel[3] = new_vels[1];

            pos[1]-=10;

            collision = true;
        }
        if (pos[1]<=box[1]) {
            float *new_vels = solve_collision(vel[1], box_vel[1], 1, m2);

            vel[1] = new_vels[0];
            box_vel[1] = new_vels[1];

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

float *solve_collision(float v1, float v2, float m1, float m2) {
    float *out = new float[2];

    out[0] = ((m1-m2)*v1+2*m2*v2) / (m1+m2);
    out[1] = ((m2-m1)*v2 + 2*m1*v1) / (m1 + m2);

    return out;
}
