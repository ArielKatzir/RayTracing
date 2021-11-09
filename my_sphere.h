#include "my_vector.h"
#include "vertex.h"

#pragma once

class Sphere
{

  private:
    Vertex centre;
    float radius;

  public:
    Sphere(Vertex c, float r){
        centre = c;
        radius = r;
    }

    Sphere(){
        centre = Vertex(0,0,1);
        radius = 0;
    }

    Vertex getCentre()  { return centre; }
    float getRadius()  { return radius; }


};
