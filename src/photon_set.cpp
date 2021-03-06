#include "photon_set.h"


    PhotonsSet::PhotonsSet(PointLight pl){
        origin = pl.get_light_position();
        pointlight = pl;
        photon_number = 10000;

        // creating spherical to cartesian look-up table
        create_lookup_table();

        util = Utils();
    }



    PhotonsSet::PhotonsSet(){}



    // function to convert spherical coordinates to cartesian
    Vector3 PhotonsSet::spherical_to_cartesian(int phi, int theta){
        float x,y,z;
        x = (sin(phi) * cos(theta))*(180/M_PI);
        y = (sin(theta) * sin(phi))*(180/M_PI);
        z = (cos(phi))*(180/M_PI);
        return Vector3(x,y,z);
    }

    Vector3 PhotonsSet::get_direction_from_table(int phi,int theta){
        Vector3 vect = conversion_lookup_table[phi*256+theta];
        return vect;
    }

    void PhotonsSet::create_lookup_table(){
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 256; j++) {
                conversion_lookup_table.push_back(spherical_to_cartesian(i, j));
            }
        }
    }

    // generating n photons
    void PhotonsSet::generate_photons(int n){
        srand((unsigned) time(0));

        // loop to generate photons in random directions around the scene
        for (int i = 0; i < n; i++){
            signx = ((rand() % 10)+1);
            signy = ((rand() % 10)+1);
            signz = ((rand() % 10)+1);
            
            random_float = (rand() % 1000);
            randomNumber_x = (rand() % 100);
            randomNumber_y = (rand() % 100);
            randomNumber_z = (rand() % 100);

            if (signx < 6){
                randomNumber_x*=-1;
            }
            if (signy < 6){
                randomNumber_y*=-1;
            }
            if (signz < 6){
                randomNumber_z*=-1;
            }

            // this line was used while testing on random photon generator using spherical coordinates
            //Vector3 photon_direction = get_direction_from_table(randomNumber_p+(random_float/1000),randomNumber_t+(random_float/1000));
            
            Vector3 photon_direction = Vector3(randomNumber_x/random_float, randomNumber_y/random_float,randomNumber_z/random_float);
            photon_direction.normalise();
            Photon p = Photon(origin, photon_direction , Colour(1,1,1));
            photons.push_back(p);
        }
    }

    void PhotonsSet::generate_photons_to_object(std::vector<Photon> &photons, Vertex centre, float error, int amount, PointLight pl){
        Vector3 photon_direction;
        int c = 0;
        while (c < amount){
            signx = ((rand() % 10)+1);
            signy = ((rand() % 10)+1);
            signz = ((rand() % 10)+1);
            random_float = (rand() % 1000);
            randomNumber_x = (rand() % 1000);
            randomNumber_y = (rand() % 1000);
            randomNumber_z = (rand() % 1000);

            if (signx < 6){
                randomNumber_x*=-1;
            }
            if (signy < 6){
                randomNumber_y*=-1;
            }
            if (signz < 6){
                randomNumber_z*=-1;
            }           
            photon_direction = Vector3(randomNumber_x/random_float, randomNumber_y/random_float,randomNumber_z/random_float);
            photon_direction.normalise();

            Vertex endpoint = Vertex(centre.x+photon_direction.x*error, centre.y+photon_direction.y*error, centre.z+photon_direction.z*error);
            // cerr << endpoint.x << " " << endpoint.y << " " << endpoint.z << "\n";
            Vector3 for_photon = util.get_vector(centre, endpoint);
            //cerr << for_photon.x << " " << for_photon.y << " " << for_photon.z << "\n";

            Photon p = Photon(pl.get_light_position(), for_photon, Colour(1,1,1));
            photons.push_back(p);
            c++;
            }
    }


    Photon PhotonsSet::generate_diffuse_photon(Vector3 normal , Photon p, Vertex intersection, Properties prop){
            Vector3 photon_direction;

            while (true){
                signx = ((rand() % 10)+1);
                signy = ((rand() % 10)+1);
                signz = ((rand() % 10)+1);
                random_float = (rand() % 1000);
                randomNumber_x = (rand() % 100);
                randomNumber_y = (rand() % 100);
                randomNumber_z = (rand() % 100);

                if (signx < 6){
                    randomNumber_x*=-1;
                }
                if (signy < 6){
                    randomNumber_y*=-1;
                }
                if (signz < 6){
                    randomNumber_z*=-1;
                }
                photon_direction = Vector3(randomNumber_x/random_float, randomNumber_y/random_float,randomNumber_z/random_float);
                photon_direction.normalise();

                // only fire diffuse photon if it
                float dot_product = util.dot(normal,photon_direction);
                if (dot_product > 0){ 
                    Photon p = Photon(origin, photon_direction, prop.get_colour());
                    photons.push_back(p);
                    return p;
                }else {
                    return generate_specular_photon(normal, p, intersection, prop);
                }
            }
    }

    Photon PhotonsSet::generate_specular_photon(Vector3 normal , Photon p, Vertex intersection, Properties prop){
        Vector3 vec_reflected = p.photon_ray.get_direction() - (normal * (2 * util.dot(p.photon_ray.get_direction(),normal)));
        vec_reflected.normalise();
        Photon p_reflected = Photon(intersection, vec_reflected , prop.get_colour());
        photons.push_back(p);
        return p_reflected;
    }
