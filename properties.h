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

        // ks+kd<2 must be true
        Properties(Colour c, float kd, float ks, float ka, bool kr, bool _refractive){
            colour = c;
            diffuse_coef = kd;
            specular_coef = ks;
            ambient_coef = ka;
            reflective = kr;
            refractive = _refractive;

            if (kr+ka >= 2){
                std::cerr << "kr + ka of an object dont equal to 2 or more, russian roulette issue." << "\n";
                exit(-1);
            }
        }

         Properties(){
            colour = Colour();
            diffuse_coef = 0;
            specular_coef = 0;
            ambient_coef = 1;
            reflective = 0;
            refractive = 0;
            
        }

        // for russian roulette to work, p(kd/2) + p(ks/2) + p(absorbed) = 1
        // coefficients are divided by two for easier input of coefficients between 0 and 1
        int russian_roulette(){
            // random number between 0 and 1
            float p = (float)rand() / RAND_MAX;
            float pd = diffuse_coef/2;
            float ps = specular_coef/2;
            // 0 = send diffuse photon
            // 1 = send specular
            // 2 = absorb photon
            if (p <= pd)return 0;
            if (p <= ps)return 1;
            else return 2;

            
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
