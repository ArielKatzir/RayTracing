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
#include "colour.h"


class Properties {

    public:
        Colour colour;
        float diffuse_coef;
        float specular_coef;
        float ambient_coef;
        bool reflective; 
        bool refractive;

        Properties(Colour c, float kd, float ks, float ka, bool kr, bool _refractive){
            colour = c;
            diffuse_coef = kd;
            specular_coef = ks;
            ambient_coef = ka;
            reflective = kr;
            refractive = _refractive;
        }

         Properties(){
            colour = Colour();
            diffuse_coef = 0;
            specular_coef = 0;
            ambient_coef = 1;
            reflective = 0;
            refractive = 0;
            
        }

        float get_diffuse(){
            return diffuse_coef;
        }

        float get_specular(){
            return specular_coef;
        }

        float get_ambient(){
            return ambient_coef;
        }

        bool get_reflective(){
            return reflective;
        }

        bool get_refractive(){
            return refractive;
        }

        Colour get_colour(){
            return colour;
        }


	
};
