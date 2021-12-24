#ifndef SPHERE_H
#define SPHERE_H


#include "vertex.h"
#include "properties.h"

#pragma once

class Sphere
{

  private:
    Properties property;
    Vertex centre;
    float radius;


  public:
    Sphere(Vertex c, float r, Properties p){
        centre = c;
        radius = r;
        property = p;
    }

    Sphere(){
        centre = Vertex(0,0,1);
        radius = 0;
        property = Properties();
    }

    Vertex getCentre()  { return centre; }
    float getRadius()  { return radius; }
    Properties getProperty()  { return property; }


};
#endif