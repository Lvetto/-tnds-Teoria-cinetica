#include "graphics.h"
#include "particle.h"

Particle::Particle(float posx, float posy, float velx, float vely)  {
        this->pos[0] = posx;;
        this->pos[1] = posy;
        this->vel[0] = velx;
        this->vel[1] = vely;
}

float Particle::update(float box[4], float *box_vel, float wall_mass) {
        float delta_q = 0;
        // check if particle is in box then, update the position. If the particle is outside the box, solve the collision and "teleport" it back a little. Box format is [x0, y0, x1, y1]
        // signs and components to use depend on wich wall the particle collided with
        if (pos[0]>=box[2]) {
            float *new_vels = solve_collision(vel[0], box_vel[2], 1, wall_mass);

            delta_q += abs(new_vels[0] - vel[0]);

            vel[0] = new_vels[0];
            box_vel[2] = new_vels[1];

            pos[0]-=10;
        }
        if (pos[0]<=box[0]) {
            float *new_vels = solve_collision(vel[0], box_vel[0], 1, wall_mass);

            delta_q += abs(new_vels[0] - vel[0]);

            vel[0] = new_vels[0];
            box_vel[0] = new_vels[1];

            pos[0]+=10;
        }

        pos[0] += vel[0];
        
        if (pos[1]>=box[3]) {
            float *new_vels = solve_collision(vel[1], box_vel[3], 1, wall_mass);

            delta_q += abs(new_vels[0] - vel[1]);

            vel[1] = new_vels[0];
            box_vel[3] = new_vels[1];

            pos[1]-=10;
        }
        if (pos[1]<=box[1]) {
            float *new_vels = solve_collision(vel[1], box_vel[1], 1, wall_mass);

            delta_q += abs(new_vels[0] - vel[1]);

            vel[1] = new_vels[0];
            box_vel[1] = new_vels[1];

            pos[1]+=10;
        }
        pos[1] += vel[1];

        return delta_q;
}

void Particle::draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 210, 0, 0, 255);   // rosso
    drawCircle(renderer, static_cast<int>(pos[0]), static_cast<int>(pos[1]), 10, 80);
}

// solve for velocities after collision using analytical solution from https://it.wikipedia.org/wiki/Urto_elastico
float *solve_collision(float v1, float v2, float m1, float m2) {
    float *out = new float[2];

    out[0] = ((m1-m2)*v1+2*m2*v2) / (m1+m2);
    out[1] = ((m2-m1)*v2 + 2*m1*v1) / (m1 + m2);

    return out;
}
