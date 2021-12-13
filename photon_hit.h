#include "my_vector.h"
#include "vertex.h"
#include "ray.h"
#include "light.h"
#include "photon.h"
#include "util.h"


#include <vector>

using namespace std;


#pragma once
class Photon_Hit {
    public:
        
        Vertex position;
        Vector3 normal;
        int depth;
        Photon photon;
        float intensity, t;
        int type; // 0 = direct, 1 = shadow;
        Utils util;

        Photon_Hit(Photon &p, float t_, int depth_, int type_, float intensity_, Vector3 &normal_){
            photon = p;
            position = p.photon_ray.on_line(t_);
            t = t_;
            depth = depth_;
            type = type_;
            intensity = intensity_;
            normal = normal_;
            util = Utils();
        }

        Photon_Hit(){
            intensity = 7777;
        }

        const Vertex& point() const{
            return position;
        }

        
};