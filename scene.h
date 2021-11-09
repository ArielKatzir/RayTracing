#include "my_vector.h"
#include "vertex.h"
#include "my_sphere.h"
#include "vertex.h"
#include "polymesh.h"
#include "framebuffer.h"
#include "utils.h"
#include <vector>



#pragma once

class Scene {
    public:
        // init camera and world variables
        int img_width,img_height;
        float _viewport_w,_viewport_h,_focal_length;
        Vector3 origin,horizontal,vertical,top_left_corner;

        // setting the max number of objects in the scene
        std::vector<PolyMesh*> meshes;
        std::vector<Sphere> sps;
        
        Scene(int width,int height,float viewport_w,float viewport_h,float focal_length){
            img_width = width;
            img_height = height;
            _viewport_h = viewport_h;
            _viewport_w = viewport_w;
            _focal_length = focal_length;

            origin = Vector3(0, 0, 0);
            horizontal = Vector3(viewport_w, 0, 0);
            vertical = Vector3(0, -viewport_h, 0);

            top_left_corner = origin-(horizontal/2);
            top_left_corner = top_left_corner-(vertical/2);
            top_left_corner = top_left_corner+Vector3(0, 0, focal_length);

        } 

        void add_sphere(Sphere s){
            sps.push_back(s);
        }

        void add_mesh(PolyMesh *pm){
            meshes.push_back(pm);
        }


        void render(FrameBuffer *fb){
            int mesh_n = meshes.size();
            int sp_n = sps.size();

            Utils util = Utils();
            // iterate every pixel starting from top left
            for (int j = 0; j < img_height; j++)
            {
                std::cerr << "\rScanlines remaining: " << j << '/' << img_height << std::flush;
                for (int i = 0; i < img_width; i++){

                    // getting new u and v
                    float u = double(i) / (img_width);
                    float v = double(j) / (img_height);

                    // getting the ray by adding up vectors from left corner of the view port,
                    // its vector to the horizontall, its vector to its vertical and the origin.
                    Vector3 vector_for_ray = horizontal * u;
                    vector_for_ray = vector_for_ray + (vertical * v);
                    vector_for_ray = vector_for_ray + top_left_corner;

                    // normalise
                    vector_for_ray.normalise();

                    // generate ray from origin
                    Ray r(Vertex(0, 0, 0), vector_for_ray);
                    
                    float maxfloat = std::numeric_limits<float>::max();
                    float small_t = maxfloat;
                    float t = 0;

                    // loops every sphere if there is one, subs the depth
                    for (int sp = 0; sp < sp_n; ++sp){
                        Sphere sphere = sps[sp];
                        if (sps[sp].getRadius() != 0){
                            t = util.hit_sphere(sphere.getCentre(), sphere.getRadius(), r);
                            if (t < small_t){
                                small_t = t;
                                fb->plotDepth(i,j,small_t);
                            }
                        }
                    }

                    for (int mesh = 0; mesh < mesh_n; ++mesh){
                        PolyMesh *pm = meshes[mesh];
                        if (meshes[mesh]->triangle_count != 0){
                            for (int tri = 0; tri < pm->triangle_count; tri += 1){
                                Triangle triangle = Triangle(pm->vertex[pm->triangle[tri][0]], pm->vertex[pm->triangle[tri][1]], pm->vertex[pm->triangle[tri][2]]);
                                t = util.hit_triangle_moller_trumbore(triangle, r);
                                if (t < small_t){
                                    small_t = t;
                                    fb->plotDepth(i,j,small_t);
                                }
                            }
                        }
                    }

                    if(small_t == maxfloat) small_t = 0;


                    if (small_t == maxfloat){
                        fb->plotDepth(i, j, 0);
                    }

                    // if (small_t > 0 && small_t != maxfloat)
                    // {
                    //     Vector3 norm = r.on_line(small_t) - Vector3(0,0,0);
                    //     norm.normalise();
                    //     //norm = cross(norm, temp_tri.getVertex0().vect(temp_tri.getVertex1()));

                    //     float c1 = (norm.x+1)*0.5;
                    //     float c2 = (norm.y+1)*0.5;
                    //     float c3 = (norm.z+1)*0.5;

                    //     fb->plotPixel(i, j, c1, c2, c3);
                    //     fb->plotDepth(i, j, small_t);
                    // }
                }
            }
        }
        


};