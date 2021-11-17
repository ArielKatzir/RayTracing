#include "my_vector.h"
#include "vertex.h"
#include "my_sphere.h"
#include "vertex.h"
#include "polymesh.h"
#include "framebuffer.h"
#include "properties.h"
#include "utils.h"
#include "light.h"
#include <vector>
#include <cmath>

using namespace std;

#pragma once

class Scene {
    public:
        // init camera and world variables
        int img_width,img_height;
        float _viewport_w,_viewport_h,_focal_length;
        Vector3 origin,horizontal,vertical,top_left_corner;
        bool include_shadow, include_diffuse, include_specular;
        PointLight point_light;


        // setting the max number of objects in the scene
        std::vector<PolyMesh*> meshes;
        std::vector<Sphere> sps;
        
        Scene(int width,int height,float viewport_w,float viewport_h,float focal_length){
            img_width = width;
            img_height = height;
            _viewport_h = viewport_h;
            _viewport_w = viewport_w;
            _focal_length = focal_length;

            include_shadow = false;
            include_diffuse = false;
            include_specular = false;

            point_light = PointLight(Vertex(-2,3,-2));

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

        void add_shadows(){
            include_shadow = true;
        }

        void add_diffuse(){
            include_diffuse = true;
        }

        void add_specular(){
            include_specular = true;
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
                    float u = float(i) / (img_width);
                    float v = float(j) / (img_height);

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
                    float intensity = 1.0;
                    Vertex intersection_point;
                    Vector3 intersection_normal;

                    // variables used to know which objects the ray intersects with 
                    bool hit_sphere = false;
                    bool hit_mesh = false;
                    int object_indice; // within the lists of objects  

                    // loops every sphere.  if ray intersects, plot colour on pixel i,j.
                    for (int sp = 0; sp < sp_n; ++sp){
                        Sphere sphere = sps[sp];
                        t = util.hit_sphere(sphere, r);
                        if (t < small_t){
                            small_t = t;
                            intersection_point = r.on_line(small_t);
                            intersection_normal = util.get_vector(sphere.getCentre() , intersection_point);
                            intersection_normal.normalise();

                            // getting mesh properties
                            float ambient = sphere.getProperty().get_ambient();
                            Colour c = sphere.getProperty().get_colour();

                            util.plot_colour_with_intensity(i, j, small_t, r, fb, intensity*ambient, c);
                            hit_sphere = true;
                            object_indice = sp;
                        }
                    }

                    // loop every polymesh
                    for (int mesh = 0; mesh < mesh_n; ++mesh){
                        PolyMesh *pm = meshes[mesh];
                        if (meshes[mesh]->triangle_count != 0){
                            for (int tri = 0; tri < pm->triangle_count; tri += 1){
                                Triangle triangle = Triangle(pm->vertex[pm->triangle[tri][0]], pm->vertex[pm->triangle[tri][1]], pm->vertex[pm->triangle[tri][2]]);
                                t = util.hit_triangle_moller_trumbore(triangle, r);
                                if (t < small_t){
                                    small_t = t;

                                    // getting the intersection point using t and the normal from surface
                                    intersection_point = r.on_line(small_t);
                                    Vertex A = triangle.getVertex0();                                   
                                    Vertex B = triangle.getVertex1();                                  
                                    Vertex C = triangle.getVertex2(); 
                                    Vector3 AB = util.get_vector(A,B);
                                    Vector3 AC = util.get_vector(A,C);
                                    intersection_normal = util.cross(AB,AC);
                                    intersection_normal.normalise();

                                    // getting mesh properties
                                    float ambient = pm->getProperty().get_ambient();
                                    Colour c = pm->getProperty().get_colour();

                                    util.plot_colour_with_intensity(i,j,small_t,r,fb, intensity, c);
                                    hit_sphere = false;
                                    hit_mesh = true;
                                    object_indice = mesh;
                                }
                            }
                        }
                    }
                    
                    Properties property;

                    // getting intersected object
                    if (hit_sphere){
                        property = sps[object_indice].getProperty();
                        
                    }else if(hit_mesh){
                        PolyMesh *poly = meshes[object_indice];
                        property = poly->getProperty();
                    }

                    Colour c = property.get_colour();

                    float diffuse_coef = property.get_diffuse();
                    float specular_coef = property.get_specular();
                    float ambient_coef = property.get_ambient();

                    // adding diffuse effect
                    if (small_t != maxfloat && include_diffuse){
                        intensity = diffuse(intersection_point, intersection_normal, point_light, intensity, diffuse_coef);
                        util.plot_colour_with_intensity(i,j,small_t,r,fb, intensity, c);
                    }
                    // adding specular effect
                    if (small_t != maxfloat && include_specular){
                        intensity = specular(intersection_point, intersection_normal, point_light, intensity, specular_coef);
                        util.plot_colour_with_intensity(i,j,small_t,r,fb, intensity, c);
                    }

                    // checking if surface shoul be shadowed
                    bool shadowed = false;
                    if (small_t != maxfloat && include_shadow){
                        shadowed = in_shadow(r, point_light, intersection_point);
                        if (shadowed){
                            // if surface is shadowed, reduce intensity to 0.3 and plot
                            intensity = 0.3;
                            util.plot_colour_with_intensity(i,j,small_t,r,fb, intensity, c);
                        }
                    }
                }
            }
        }

        bool in_shadow(Ray r, PointLight pl, Vertex intersection_point){
            Utils util = Utils();
            // set up
            float maxfloat = std::numeric_limits<float>::max();
            float max = maxfloat;
            int mesh_n = meshes.size();
            int sp_n = sps.size();
            float shift_t = 0.03;

            // getting a shadow ray
            Vector3 vec_for_ray = util.get_vector(intersection_point,pl.get_light_position());
            vec_for_ray.normalise();

            // shifting the ray away from the sphere slightly
            Ray shadow_ray = Ray(intersection_point, vec_for_ray);
            shadow_ray.origin = shadow_ray.on_line(shift_t);
            
            // loops every sphere if there is one, plot
            for (int sp = 0; sp < sp_n; ++sp){
                Sphere sphere = sps[sp];

                float t2 = util.hit_sphere(sphere, shadow_ray);
                // if there is intersection and it doesnt face backwards from light
                if (t2 > 0 && t2 != max){
                    return true;
                }
            }
            return false;
        }
        
        float diffuse(Vertex intersection_point, Vector3 normal, PointLight pl, float intensity, float diffuse){
            Utils util = Utils();
            Vector3 light_ray = util.get_vector(intersection_point, pl.get_light_position());
            light_ray.normalise();

            // get dot product between the normal and light ray
            float dot = util.dot(normal, light_ray);
            if (dot < 0){
                return intensity;
            }
            return (dot*diffuse)+intensity;
        }

        float specular(Vertex intersection_point, Vector3 normal, PointLight pl, float intensity, float specular){
            Utils util = Utils();
            Vector3 light_ray = util.get_vector(pl.get_light_position(), intersection_point);
            light_ray.normalise();

            // firing reflectved ray using: 𝑟=𝑑−2(𝑑⋅𝑛)𝑛
            Vector3 reflected_light_ray = light_ray - (normal * (2 * util.dot(light_ray,normal)));
            reflected_light_ray.normalise();

            Vector3 ray_to_viewer = util.get_vector(intersection_point, util.to_ver(origin));
            ray_to_viewer.normalise();

            // getting dot product of reflected ray and surface to viewer ray
            float dot = util.dot(ray_to_viewer, reflected_light_ray);
            
            if (dot < 0) return intensity;

            return (pow(dot, 20)*specular)+intensity;
        }



};