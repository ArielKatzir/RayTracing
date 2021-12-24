#ifndef PHOTON_H
#define PHOTON_H



#include "my_vector.h"
#include "vertex.h"
#include "ray.h"
#include <vector>
#include <cmath>
#include "colour.h"

using namespace std;

#pragma once
class Photon {
    public:

    Vector3 direction;
    Ray photon_ray;
    float energy;
    Colour colour;

    Photon(Vertex origin, Vector3 _direction, Colour c){
        
        direction = _direction;
        photon_ray = Ray(origin,direction);
        energy = 1.0;
        colour = c;
    }

    Photon(){

    }
};

#endif