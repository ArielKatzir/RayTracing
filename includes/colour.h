/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2019.
 *
 * Do what you like with this code as long as you retain this comment.
 */

// Ray is a class to store and maniplulate 3D rays.

#ifndef COLOUR_H
#define COLOUR_H


#pragma once

class Colour {

    public:
        float red;
        float green;
        float blue;

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

#endif 