#ifndef PHOTONSET_H
#define PHOTONSET_H




#include "my_vector.h"
#include "vertex.h"
#include "light.h"
#include <vector>
#include <cmath>
#include "photon_hit.h"
#include "photon.h"
#include "utils.h"

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

    float randomNumber_z,randomNumber_x,randomNumber_y, random_float,signx,signy,signz;

    PhotonsSet(PointLight pl);
    PhotonsSet();
    void create_lookup_table();
    void generate_photons(int n);
    void generate_photons_to_object(std::vector<Photon> &photons, Vertex centre, float error, int amount, PointLight pl);
    Vector3 spherical_to_cartesian(int phi, int theta);
    Vector3 get_direction_from_table(int phi,int theta);
    Photon generate_diffuse_photon(Vector3 normal , Photon p, Vertex intersection, Properties prop);
    Photon generate_specular_photon(Vector3 normal , Photon p, Vertex intersection, Properties prop);
};

#endif
