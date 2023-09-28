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

// explicitly create instance of function average for the types used. Needed to successfully compile the project
template float average<Particle>(std::vector<Particle> v, float (*func)(Particle), int start=0, int end=-1);
template float average(std::vector<float> v, int start=0, int end=-1);
