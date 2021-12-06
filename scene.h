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
        float _viewport_w,_viewport_h,_focal_length;
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
        
        Scene(int width,int height,float viewport_w,float viewport_h,float focal_length){
            img_width = width;
            img_height = height;
            _viewport_h = viewport_h;
            _viewport_w = viewport_w;
            _focal_length = focal_length;

            // shadows are optionally added from main using add_shadows()
            include_shadow = false;

            // point light location and photons set initialisation
            point_light = PointLight(Vertex(-1.3,1.4,6.9));
            photons_set = PhotonsSet(point_light);

            // generating photons in random directions
            photons_set.generate_photons(photons_set.photon_number);
            cerr << photons_set.photon_number << " Photons were sucessfully generated" << "\n";
        
            // camera and viewport settings 
            origin = Vector3(0, 0, -1);
            horizontal = Vector3(viewport_w, 0, 0);
            vertical = Vector3(0, -viewport_h, 0);
            top_left_corner = origin-(horizontal/2);
            top_left_corner = top_left_corner-(vertical/2);
            top_left_corner = top_left_corner+Vector3(0, 0, focal_length);
            
            // others
            reflection_depth = 1;
            util = Utils();
            max = std::numeric_limits<float>::max();

        
        } 


        void add_sphere(Sphere s){
            sps.push_back(s);
        }
        void add_rectangle(Rectangle r){
            rects.push_back(r);
        }
        void add_mesh(PolyMesh *pm){
            meshes.push_back(pm);
        }
        void add_shadows(){
            include_shadow = true;
        }

        void render(FrameBuffer *fb){
            mesh_n = meshes.size();
            sp_n = sps.size();
            rect_n = rects.size();

            // getting an kd-tree of all of the photon intersection
            KD::Tree<CORE> hits_tree = first_pass_part_one();

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
                    Vertex camera_location = util.to_ver(origin);
                    Ray r(camera_location, vector_for_ray);
                    
                    float smallest_t = max;
                    float intensity = 1.0;
                    Vertex intersection_point;
                    Vector3 intersection_normal;

                    Properties property;
 

                    
                    //Finding the closest intersection and the normal between all of the objects in the scene
                    
                    util.intersect_object(r, smallest_t, intersection_normal, intersection_point, property, rects, sps,meshes);

                    
                    // getting mesh properties
                    Colour colour = property.get_colour();
                    float diffuse_coef = property.get_diffuse();
                    float specular_coef = property.get_specular();
                    float ambient_coef = property.get_ambient();
                    bool is_reflective = property.get_reflective();
                    bool is_refractive = property.get_refractive();

                    // each intersection method returns a maxfloat if no intersection is found, if this is the case,
                    // we colour the pixel black
                    if (smallest_t != max){
                        // if intersected object is reflective
                        if (is_reflective){
                            colour = reflected(intersection_point, r, intersection_normal, reflection_depth, intensity, point_light);
                            intensity*=phong_shading(intersection_point, intersection_normal, point_light, property);
                            util.plot_colour_with_intensity(i,j,smallest_t,r,fb, intensity, colour);
                        
                        }
                        //I only implemented refraction on spheres due to lack of time
                        else if (is_refractive){
                            colour = refracted(intersection_point, r, intersection_normal, intensity, point_light);
                            intensity*=phong_shading(intersection_point, intersection_normal, point_light, property);
                            util.plot_colour_with_intensity(i,j,smallest_t,r,fb, intensity, colour);
                        }
                        
                        //if object is neither refractive or reflective plot its intensity using Phong's model
                        else{
                            intensity = phong_shading(intersection_point, intersection_normal, point_light, property);
                            util.plot_colour_with_intensity(i,j,smallest_t,r,fb, intensity, colour);
                        }
                        
                        // checking if surface should be shadowed (pre-photon mapping implementation )
                        bool shadowed = false;
                        if (include_shadow){
                            shadowed = in_shadow(r, point_light, intersection_point);
                            if (shadowed){
                                // if surface is shadowed, reduce intensity to 0.3 and plot
                                intensity = 0.3;
                                util.plot_colour_with_intensity(i,j,smallest_t,r,fb, intensity, colour);
                            }
                        }
                    }
                }
            }
        }
 
        float phong_shading(Vertex intersection_point, Vector3 normal, PointLight pl, Properties property){
            float intensity = property.get_ambient(); 
            intensity+=diffuse(intersection_point, normal, pl, property);
            intensity+=specular(intersection_point, normal, pl, property);
            return intensity;
        }

        // shadow function before photon mapping implementation
        bool in_shadow(Ray r, PointLight pl, Vertex intersection_point){
            float shift_t = 0.01;

            // getting a shadow ray
            Vector3 vec_for_ray = util.get_vector(intersection_point,pl.get_light_position());
            vec_for_ray.normalise();

            // shifting the ray away from the intersection slightly
            Ray shadow_ray = Ray(intersection_point, vec_for_ray);
            shadow_ray.origin = shadow_ray.on_line(shift_t);

            float t_to_light = shadow_ray.get_t(pl.get_light_position());
            float t2;

            // loops every sphere
            for (int sp = 0; sp < sp_n; ++sp){
                Sphere sphere = sps[sp];

                t2 = util.hit_sphere(sphere, shadow_ray)[0];
                // if there is intersection and it doesnt face backwards from light
                if (t2 > 0 && t2 != max && t2 < t_to_light){
                    return true;
                }
            }
            // loops every mesh if there is one, plot
            for (int ms = 0; ms < mesh_n; ++ms){
                PolyMesh *pm = meshes[ms];
                if (meshes[ms]->triangle_count != 0){
                    for (int tri = 0; tri < pm->triangle_count; tri += 1){
                        Triangle triangle = Triangle(pm->vertex[pm->triangle[tri][0]], pm->vertex[pm->triangle[tri][1]], pm->vertex[pm->triangle[tri][2]]);
                        t2 = util.hit_triangle_moller_trumbore(triangle, shadow_ray);
                        // if there is intersection and it doesnt face backwards from light
                        if (t2 > 0 && t2 != max && t2 < t_to_light){
                            return true;
                        }
                    }
                }
            }
            return false;
        }
        
        float diffuse(Vertex intersection_point, Vector3 normal, PointLight pl, Properties property){
            Vector3 light_ray = util.get_vector(intersection_point, pl.get_light_position());
            light_ray.normalise();

            // get dot product between the normal and light ray
            float dot = util.dot(normal, light_ray);
            if (dot < 0){
                return 0;
            }
            return dot*property.get_diffuse();
        }

        float specular(Vertex intersection_point, Vector3 normal, PointLight pl, Properties property){
            Vector3 light_ray = util.get_vector(pl.get_light_position(), intersection_point);
            light_ray.normalise();

            // firing reflectved ray using: 𝑟=𝑑−2(𝑑⋅𝑛)𝑛
            Vector3 reflected_light_ray = light_ray - (normal * (2 * util.dot(light_ray,normal)));
            reflected_light_ray.normalise();

            Vector3 ray_to_viewer = util.get_vector(intersection_point, util.to_ver(origin));
            ray_to_viewer.normalise();

            // getting dot product of reflected ray and surface to viewer ray
            float dot = util.dot(ray_to_viewer, reflected_light_ray);
            
            if (dot < 0) return 0;

            return (pow(dot, 40)*property.get_specular());
        }

        // needs to return the intensity and the colour of the reflected object, so always pass intensity=0 as a parameter 
        // and modify it in the function, and return the colour normally 
        Colour reflected(Vertex intersection_point, Ray r, Vector3 normal, int depth, float &intensity , PointLight pl){

            float shift_t = 0.001;

            // firing reflectved ray using: 𝑟=𝑑−2(𝑑⋅𝑛)𝑛
            Vector3 vec_reflected = r.get_direction() - (normal * (2 * util.dot(r.get_direction(),normal)));
            vec_reflected.normalise();

            Ray reflected_ray = Ray(intersection_point, vec_reflected);
            reflected_ray.origin = reflected_ray.on_line(shift_t);

            float smallest_t = max;
            Properties property;
            Vector3 new_normal;
            Vertex new_intersection = intersection_point;

            util.intersect_object(reflected_ray, smallest_t, new_normal, new_intersection, property, rects, sps,meshes);

            if (smallest_t > 0 && smallest_t!=max){
                // depth of 1 stops the recursion
                if (depth > 1 && property.get_reflective()){
                    return reflected(new_intersection, reflected_ray, new_normal, depth-1, intensity, pl);
                }else{
                    intensity = phong_shading(new_intersection, new_normal, pl, property);
                    bool shadowed = in_shadow(reflected_ray, pl, new_intersection);
                    if (shadowed) intensity=0.3;
                    if (property.get_refractive()){
                        Colour c = refracted(new_intersection, reflected_ray, new_normal, intensity, pl);
                        intensity = phong_shading(new_intersection, new_normal, pl, property);
                        return c;
                    }
        
                    return property.get_colour();
                }
            // plot black if no intersection
            }else{
                return Colour(0,0,0);
            }
        }
        
        // this is only used for spheres -> walls(i just need to show it works), if i wanted it to work on any object,
        // the object would be passed as a parameter, and a second intersection from the inside could easily be calculated.
        Colour refracted(Vertex intersection_point, Ray r, Vector3 normal, float &intensity, PointLight pl){
            
            float shift_t = 0.001;
            // gets refractive indexes
            float n1 = 1.0;
            float n2 = 1.03; // like water (this is set to default, will only have 1 refracted object)
            float refractive_ratio_enter = n1/n2;
            float refractive_ratio_exit = n2/n1;
            float t = max;
            Properties property;
            Vector3 new_normal;
            Vertex new_intersection = intersection_point;

            // calculating the first refracted ray by adding up the perpendicular 
            //and parallel (from normal) rays by cos(theta)
            float cos_theta = fmin(util.dot(-r.get_direction(), normal), 1.0);
            Vector3 r_out_perp =  (r.get_direction() + normal*cos_theta)*refractive_ratio_enter;
            Vector3 r_out_parallel = normal* (-sqrt( fabs (-r_out_perp.length_squared()+1 )));
            Vector3 vector_for_refracted = r_out_perp + r_out_parallel;
            vector_for_refracted.normalise();
            Ray r2 = Ray(intersection_point, vector_for_refracted);
            r2.origin = r2.on_line(shift_t);

            util.intersect_object(r2, t, new_normal, new_intersection, property, rects, sps, meshes);

            if (t > 0 && t!=max){
                if (property.get_refractive() == true){
                    return refracted(new_intersection, r2, new_normal, intensity, pl);
                }else{
                    intensity = phong_shading(new_intersection, new_normal, pl, property);
                    bool shadowed = in_shadow(r2, pl, new_intersection);
                    if (shadowed) intensity=0.3;
                    return property.get_colour();
                }
            }
            
            return Colour(0,0,0);
        }


        // kd tree config
        struct CORE{

            typedef Photon_Hit* Item;  // The type we store in the tree.
            typedef Vertex  Point; // The type we read coordinates from.
            typedef float  Coord; // The type of individual coordinates.

            static const int DIMENSIONS =  3; // We're in a three-dimensional space.
            static const int MAX_DEPTH  = 10; // The tree will reach at most ten levels.
            static const int STORAGE    =  8; // Leaves can hold eight items before splitting.

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

        // first pass of photon mapping (random photons only), returns a kd_tree of photon intersections
        KD::Tree<CORE> first_pass_part_one(){
            /* psudocode:
            / for every photon generated:
            /   fire to scene to get all of its intersections
                    if russian roulette returns diffuse or specular
                        fire again
                add all of the photon intersections to a list of intersection
                add all of the intersections to the kd-tree 
            */
        
            // full list of photon intersections
            std::vector<Photon_Hit> photon_hits;
            
            // having a list of hits per photon so we could get the direct hit later
            int roulette_result;
            std::vector<Photon_Hit> intersections_temp; 
            Photon_Hit direct_hit;     
            Properties direct_object_property;     

            for (int p = 0; p < photons_set.photon_number; p++){
                // std::cerr << "\rPhotons remaining: " << p << '/' << photons_set.photon_number << std::flush;
                Photon photon = photons_set.photons[p];

                // fire photon into scene from light
                fire_photon(photon, intersections_temp, 0);

                // add all intersections to the full list of photons (use insert to kd tree later)
                for (int hit = 0; hit < intersections_temp.size(); ++hit){
                    photon_hits.push_back(intersections_temp[hit]);
                }
                intersections_temp.clear();
            }

            // inserting the recorded intersections into a kd-tree
            // implemented by: https://github.com/xavierholt/kd

            for (int pp = 0; pp < photon_hits.size(); ++pp){
                if (photon_hits[pp].depth > 1){
                    cerr << photon_hits[pp].depth << "\n";
                }
            }

            Vertex min = Vertex(-9, -9, -9);
            Vertex max = Vertex(9, 9, 9);  
            KD::Tree<CORE> kdtree(min, max);

            for (int hit = 0; hit < photon_hits.size(); ++hit){
                kdtree.insert(&photon_hits[hit]);
            }
            cerr << photon_hits.size() << " Photon intersections recorded and inserted into a KD-tree" << "\n";

            return kdtree;

        }

        // fire photon and create an intersections list if there is one (recursion)
        void fire_photon(Photon p, std::vector<Photon_Hit> &intersections_temp, int depth){
            float t;
            Photon_Hit closest_temp;
            Properties direct_object_property;
            Vertex intersection;
            Vector3 normal;

            // Cant use the usual intersect object function since we need to record every single intersection and not just the closest one

            // recangle intersection
            for (int rc = 0; rc < rect_n; ++rc){
                Rectangle rectangle = rects[rc];
                t = util.hit_rectangle_plane_algo(rectangle, p.photon_ray);
                
                // adding all the rectangle intersections into a list
                if (t != max){
                    add_intersection(p, closest_temp, t, rectangle.getProperty(),direct_object_property, intersections_temp, depth, rectangle.get_normal()); 
                }
            }

            // sphere intersection
            for (int sp = 0; sp < sp_n; ++sp){
                Sphere sphere = sps[sp];
                t = util.hit_sphere(sphere, p.photon_ray)[0];

                // adding all the sphere intersections into the list
                if (t != max){
                    intersection = p.photon_ray.on_line(t);
                    normal = util.get_vector(sphere.getCentre() , intersection);
                    normal.normalise();
                    add_intersection(p, closest_temp, t, sphere.getProperty(),direct_object_property, intersections_temp, depth, normal); 
                }
            }

            // mesh intersection
            for (int ms = 0; ms < mesh_n; ++ms){
                PolyMesh *pm = meshes[ms];
                if (meshes[ms]->triangle_count != 0){
                    for (int tri = 0; tri < pm->triangle_count; tri += 1){
                        Triangle triangle = Triangle(pm->vertex[pm->triangle[tri][0]], pm->vertex[pm->triangle[tri][1]], pm->vertex[pm->triangle[tri][2]]);
                        t = util.hit_triangle_moller_trumbore(triangle, p.photon_ray);
                        if(t > 0 && t != max){
                            intersection = p.photon_ray.on_line(t);
                            Vertex A = triangle.getVertex0();                                   
                            Vertex B = triangle.getVertex1();                                  
                            Vertex C = triangle.getVertex2(); 
                            Vector3 AB = util.get_vector(A,B);
                            Vector3 AC = util.get_vector(A,C);
                            normal = util.cross(AB,AC);
                            normal.normalise();
                            add_intersection(p, closest_temp, t, pm->getProperty(),direct_object_property, intersections_temp, depth, normal); 
                            // break here so we dont intersect the dark side of the mesh
                            break;
                        }
                    }
                }    
            }

    
            // default constructor for hit has intensity of 7777 to know its empty.
            if(closest_temp.intensity == 7777){
                return;
            } 

            intersection = closest_temp.position;
            int roulette_result = direct_object_property.russian_roulette();

            // adding more intersections with depth 1 - recursion
            if (roulette_result==0){
                Photon diffuse_p_prime = photons_set.generate_diffuse_photon(closest_temp.normal);
                fire_photon(diffuse_p_prime, intersections_temp, 1);
                
            }else if (roulette_result==1){
                Photon specular_p_prime = photons_set.generate_specular_photon(closest_temp.normal , p, intersection);
                fire_photon(specular_p_prime, intersections_temp, 1);
            }

        }

        void add_intersection(  Photon p,
                                Photon_Hit &closest_temp,
                                float t,
                                Properties property, 
                                Properties &closest_property,  
                                std::vector<Photon_Hit> &intersections, 
                                int depth,
                                Vector3 normal)
        {
            // checks to see if there is an intersection
            if (t > 0 && t != max){
                // if the intersections list isnt empty

                if (intersections.size() != 0){
                    // if the current intersection is further than than the closest temp, it is a shadow hit.
                    // Add the intersection with type shadow, and give it the ambient coefficient as intensity
                    if (t > closest_temp.t){
                        p.energy*=(property.ambient_coef);
                        intersections.push_back(Photon_Hit(p, t, depth, 0,p.energy , normal));
                    }else{
                        // finding the previous closest hit and changing it to shadow
                        for (int i = 0; i < intersections.size(); i++){
                            if (intersections[i].type == 0)intersections[i].type = 1;
                        }
                        // adding the new closest hit as direct with diffuse intensity to the list
                        p.energy*=(property.diffuse_coef);
                        closest_temp = Photon_Hit(p, t, depth, 0,p.energy, normal);
                        intersections.push_back(closest_temp);
                        closest_property = property;
                    }
                }
                
                // if the intersection list is empty, set the new intersection as direct 
                if (intersections.size() == 0){
                    p.energy*=(property.diffuse_coef);
                    closest_temp = Photon_Hit(p, t, depth, 0,p.energy, normal);
                    intersections.push_back(closest_temp);
                    closest_property = property;
                }
            }
        }

        void add_Cornell_box(){
            Properties front_wall_property = Properties(Colour(1,1,1), 0.5, 0.4, 0.2, false, false);
            Vector3 v_front_wall = Vector3(3.0,0.0,0.0); 
            Vector3 u_front_wall = Vector3(0.0,3.0,0.0); 
            Rectangle front_wall = Rectangle(Vertex(-1.5,-1.5,7), v_front_wall, u_front_wall, front_wall_property);

            Properties right_wall_property = Properties(Colour(0,1,0), 0.5, 0.75, 0.2, false, false);
            Vector3 v_right_wall = Vector3(0.0,0.0,-3.0); 
            Vector3 u_right_wall = Vector3(0.0,3.0,0.0); 
            Rectangle right_wall = Rectangle(Vertex(1.5,-1.5,7), v_right_wall, u_right_wall, right_wall_property);

            Properties left_wall_property = Properties(Colour(1,0,0), 0.5, 0.75, 0.2, false, false);
            Vector3 v_left_wall = Vector3(0.0,0.0,3.0); 
            Vector3 u_left_wall = Vector3(0.0,3.0,0.0); 
            Rectangle left_wall = Rectangle(Vertex(-1.5,-1.5,4), v_left_wall, u_left_wall, left_wall_property);

            Properties floor_property = Properties(Colour(1,1,1), 0.5, 0.75, 0.2, false, false);
            Vector3 v_floor = Vector3(3.0,0.0,0.0); 
            Vector3 u_floor = Vector3(0.0,0.0,3.0); 
            Rectangle floor = Rectangle(Vertex(-1.5,-1.5,4), v_floor, u_floor, floor_property);

            Properties ceiling_property = Properties(Colour(1,1,1), 0.5, 0.75, 0.2, false, false);
            Vector3 v_ceiling= Vector3(3.0,0.0,0.0); 
            Vector3 u_ceiling = Vector3(0.0,0.0,3.0); 
            Rectangle ceiling = Rectangle(Vertex(-1.5,1.5,4), v_ceiling, u_ceiling, ceiling_property);

            add_rectangle(front_wall);
            add_rectangle(right_wall);
            add_rectangle(left_wall);
            add_rectangle(floor);
            add_rectangle(ceiling);
        }
};



