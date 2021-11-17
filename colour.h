/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2019.
 *
 * Do what you like with this code as long as you retain this comment.
 */

// Ray is a class to store and maniplulate 3D rays.

#pragma once

#include "vertex.h"
#include "my_vector.h"


class Colour {

    public:
        float red;
        float blue;
        float green;

        Colour(float r, float g, float b){
            red = r;
            blue = b;
            green = g;
        }

        Colour(){
            red = 0;
            blue = 0;
            green = 0; 
        }
};
