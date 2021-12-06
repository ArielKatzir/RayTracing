#include "my_vector.h"
#include "vertex.h"
#include "ray.h"
#include "light.h"
#include <vector>
#include <cmath>
#include "photon_hit.h"
#include "photon.h"
#include "kd-master/src/tree.h"



using namespace std;

#pragma once
class PhotonsSet {
    public:
    
    Vertex origin;
    vector<Photon> photons;
    vector<Vector3> conversion_lookup_table;
    vector<Photon_Hit> photon_hits;
    int photon_number;
    PointLight pointlight;
    Utils util;

    PhotonsSet(PointLight pl){
        origin = pl.get_light_position();
        pointlight = pl;
        photon_number = 10000;

        // creating spherical to cartesian look-up table
        create_lookup_table();

        util = Utils();
    }



    PhotonsSet(){}



    // function to convert spherical coordinates to cartesian
    Vector3 spherical_to_cartesian(int phi, int theta){
        float x,y,z;
        x = (sin(phi) * cos(theta))*(180/M_PI);
        y = (sin(theta) * sin(phi))*(180/M_PI);
        z = (cos(phi))*(180/M_PI);
        return Vector3(x,y,z);
    }

    Vector3 get_direction_from_table(int phi,int theta){
        Vector3 vect = conversion_lookup_table[phi*256+theta];
        return vect;
    }

    void create_lookup_table(){
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 256; j++) {
                conversion_lookup_table.push_back(spherical_to_cartesian(i, j));
            }
        }
    }

    // generating n photons
    void generate_photons(int n){
        srand((unsigned) time(0));
        int randomNumber_p, randomNumber_t;

        // loop to generate photons in random directions around the scene
        for (int i = 0; i < n; i++){
            randomNumber_p = (rand() % 255) + 1;
            randomNumber_t = (rand() % 255) + 1;
            Vector3 photon_direction = get_direction_from_table(randomNumber_p,randomNumber_t);
            photon_direction.normalise();
            Photon p = Photon(origin, photon_direction);
            photons.push_back(p);
        }
    }

    Photon generate_diffuse_photon(Vector3 normal){
            srand((unsigned) time(0));
            int randomNumber_p, randomNumber_t;

            while (true){
                randomNumber_p = (rand() % 255) + 1;
                randomNumber_t = (rand() % 255) + 1;
                Vector3 photon_direction = get_direction_from_table(randomNumber_p,randomNumber_t);
                photon_direction.normalise();

                // only fire diffuse photon if it
                float dot_product = util.dot(normal,photon_direction);
                if (dot_product > 0){
                    Photon p = Photon(origin, photon_direction);
                    photons.push_back(p);
                    return p;
                }
            }
    }

    Photon generate_specular_photon(Vector3 normal , Photon p, Vertex intersection){
        Vector3 vec_reflected = p.photon_ray.get_direction() - (normal * (2 * util.dot(p.photon_ray.get_direction(),normal)));
        vec_reflected.normalise();
        Photon p_reflected = Photon(intersection, vec_reflected);
        photons.push_back(p);
        return p_reflected;
    }
};