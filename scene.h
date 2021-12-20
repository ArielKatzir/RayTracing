#include "my_vector.h"
#include "vertex.h"
#include "my_sphere.h"
#include "vertex.h"
#include "polymesh.h"
#include "framebuffer.h"
#include "properties.h"
#include "rectangle.h"
#include "utils.h"
#include "light.h"
#include "photon.h"
#include "Photons_set.h"
#include "photon_hit.h"
#include "kd-master/src/tree.h"


#include <vector>
#include <cmath>

using namespace std;

#pragma once



class Scene {
    public:
        // init camera and world variables
        int img_width,img_height, reflection_depth;
        float _viewport_w,_viewport_h,_focal_length,max_intensity;
        Vector3 origin,horizontal,vertical,top_left_corner;
        bool include_shadow;
        PointLight point_light; // make this an array to add more
        Utils util;
        PhotonsSet photons_set;


        // setting the max number of objects in the scene
        std::vector<PolyMesh*> meshes;
        std::vector<Sphere> sps;
        std::vector<Rectangle> rects;

        // other
        float max;
        int mesh_n,sp_n,rect_n;
        
        Scene(int width,int height,float viewport_w,float viewport_h,float focal_length);

        // kd tree config
        struct CORE{

            typedef Photon_Hit* Item;  // The type we store in the tree.
            typedef Vertex  Point; // The type we read coordinates from.
            typedef float  Coord; // The type of individual coordinates.

            static const int DIMENSIONS =  3; // We're in a three-dimensional space.
            static const int MAX_DEPTH  = 100; // The tree will reach at most ten levels.
            static const int STORAGE    =  20; // Leaves can hold eight items before splitting.

            // Get the distance of a point along the given axis.
            static Coord coordinate(const Point& point, int axis){
                if(axis==0)return point.x;
                else if(axis==1)return point.y;
                else if(axis==2)return point.z;
                cerr << "wrong axis" << "\n";

                return 0;
            }

            // Get the location of an item
            static const Vertex& point(const Item& item){
                return item->point();
            }

        };


        void add_sphere(Sphere s);
        void add_rectangle(Rectangle r);
        void add_mesh(PolyMesh *pm);
        void add_shadows();
        void render(FrameBuffer *fb);
        void second_pass(Vertex &intersection_point, KD::Tree<CORE> &tree, float &total_intensity, Colour &c);
        void fire_photon(Photon p, std::vector<Photon_Hit> &intersections_temp, int depth);
        void add_intersection(Photon p,Photon_Hit &closest_temp,float t,Properties property,Properties &closest_property,std::vector<Photon_Hit> &intersections,int depth, Vector3 normal);
        void add_Cornell_box();
        bool in_shadow(Ray r, PointLight pl, Vertex intersection_point);
        float phong_shading(Vertex intersection_point, Vector3 normal, PointLight pl, Properties property);
        float diffuse(Vertex intersection_point, Vector3 normal, PointLight pl, Properties property);
        float specular(Vertex intersection_point, Vector3 normal, PointLight pl, Properties property);
        Colour reflected(Vertex new_intersection, Ray r, Vector3 normal, int depth, float &intensity , PointLight pl, KD::Tree<CORE> &hits_tree);
        Colour refracted(Vertex &intersection_point, Ray r, Vector3 normal, float &intensity, PointLight pl, KD::Tree<CORE> &hits_tree);
        KD::Tree<CORE> first_pass_part_one();
        
};



