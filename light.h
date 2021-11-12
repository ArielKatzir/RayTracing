#include "vertex.h"


#pragma once

class PointLight {    

    public:

        Vertex light_location;
        
        PointLight(Vertex l){
            light_location = l;
        }

        PointLight(){
            light_location = Vertex(0,0,0);
        }



        Vertex get_light_position(){
            return light_location;
        }
};