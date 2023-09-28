#include "functions.h"

float max(std::vector<float> v) {
    float out = v[0];
    for (auto &i: v)
        if (i > out)
            out = i;
    return out;
}


template <typename type>
float average(std::vector<type> v, int start, int end) {
    if (end < start)
        end = v.size() - 1;
    float out = 0;

    for (int i=start; i <= end; i++)
        out+=static_cast<float>(v[i]) / static_cast<float>(end - start + 1);

    return out;
}

template <typename type>
float average(std::vector<type> v, float (*func)(type), int start, int end) {
    if (end < start)
        end = v.size() - 1;
    float out = 0;

    for (int i=start; i <= end; i++)
        out+=func(v[i]) / static_cast<float>(end - start + 1);

    return out;    
}

float speed_mod2(Particle p) {
    return pow(p.vel[0], 2) + pow(p.vel[1], 2);
}


// compute speeds after impact using analytical solution from https://it.wikipedia.org/wiki/Urto_elastico
/* float *solve_collision(float v1, float v2, float m1, float m2) {
    float *out = new float[2];

    out[1] = ((m1-m2)*v1+2*m2*v2) / (m1+m2);
    out[2] = ((m2-m1)*v2 + 2*m1*v1) / (m1 + m2);

    return out;
} */

// generates unique pairs of elements from a vector, used to check for collisions
template <typename type>
std::vector<type*> generateUniquePairs(std::vector<type> &vec) {
    int n = vec.size();
    std::vector<type*> out;
    
    if (n < 2)
        return out;

    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            type *t = new type[2];
            t[0] = vec[i];
            t[1] = vec[j];
            out.push_back(t);
        }
    }

    return out;
}

float distance(Particle p1, Particle p2) {
    return pow(pow(p1.pos[0] - p2.pos[0], 2) + pow(p1.pos[1] - p2.pos[1], 2), 0.5);
}


// explicitly create instance of function average for the types used. Needed to successfully compile the project
template float average<Particle>(std::vector<Particle> v, float (*func)(Particle), int start=0, int end=-1);
template float average(std::vector<float> v, int start=0, int end=-1);

//template std::vector<Particle*> generateUniquePairs(std::vector<Particle> &vec);
