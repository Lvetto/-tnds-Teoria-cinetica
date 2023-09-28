#include "particle.h"
#include <vector>

float speed_mod2(Particle p);   // compute squared speed given a particle

float max(std::vector<float> v);    // find maximun in float vector

template <typename type>
float average(std::vector<type> v, int start=0, int end=-1);    // compute average on a vector of elements cast-able to a float. Allows to average only over part of the vector

template <typename type>
float average(std::vector<type> v, float (*func)(type), int start=0, int end=-1);   // evaluates func on vector elements and computes average

//float* solve_collision(float v1, float v2, float m1=1, float m2=1);

template <typename type>
std::vector<type*> generateUniquePairs(std::vector<type> &vec);

float distance(Particle p1, Particle p2);
