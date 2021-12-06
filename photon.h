#include "my_vector.h"
#include "vertex.h"
#include "ray.h"
#include "light.h"
#include <vector>
#include <cmath>
#include "light.h"



using namespace std;

#pragma once
class Photon {
    public:

    Vector3 direction;
    Ray photon_ray;
    float energy;
    int x;

    Photon(Vertex origin, Vector3 _direction){

        x = rand();

        direction = _direction;
        photon_ray = Ray(origin,direction);
        energy = 1.0;
    }

    Photon(){

    }
};