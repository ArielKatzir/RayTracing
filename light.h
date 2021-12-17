#include "vertex.h"


#pragma once

class PointLight {    

    public:

        float total_energy; 
        Vertex light_location;
        
        PointLight(Vertex l){
            light_location = l;
            total_energy = 1000000; // million is a good amount?
        }
        PointLight(){light_location = Vertex(0,0,0);}
        Vertex get_light_position(){return light_location;}
};