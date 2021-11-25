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


        // setting the max number of objects in the scene
        std::vector<PolyMesh*> meshes;
        std::vector<Sphere> sps;
        std::vector<Rectangle> rects;
        
        Scene(int width,int height,float viewport_w,float viewport_h,float focal_length){
            img_width = width;
            img_height = height;
            _viewport_h = viewport_h;
            _viewport_w = viewport_w;
            _focal_length = focal_length;

            include_shadow = false;
            point_light = PointLight(Vertex(-1.3,1.4,6.9));

            origin = Vector3(0, 0, -1);
            horizontal = Vector3(viewport_w, 0, 0);
            vertical = Vector3(0, -viewport_h, 0);

            top_left_corner = origin-(horizontal/2);
            top_left_corner = top_left_corner-(vertical/2);
            top_left_corner = top_left_corner+Vector3(0, 0, focal_length);
            reflection_depth = 1;

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
            int mesh_n = meshes.size();
            int sp_n = sps.size();
            int rect_n = rects.size();

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
                    Ray r(Vertex(0, 0, -1), vector_for_ray);
                    
                    float maxfloat = std::numeric_limits<float>::max();
                    float small_t = maxfloat;
                    float t = 0;
                    float intensity = 1.0;
                    Vertex intersection_point;
                    Vector3 intersection_normal;

                    // variables used to know which objects the ray intersects with 
                    bool hit_sphere = false;
                    bool hit_mesh = false;
                    bool hit_rectangle = false;
                    int object_indice; // within the lists of objects  

                    // loops every rectangle.  if ray intersects, plot colour on pixel i,j.
                    for (int rc = 0; rc < rect_n; ++rc){
                        Rectangle rectangle = rects[rc];
                        t = util.hit_rectangle_plane_algo(rectangle, r);
                        if (t < small_t){
                            small_t = t;
                            intersection_point = r.on_line(small_t);
                            intersection_normal = rectangle.get_normal();
                            intersection_normal.normalise();

                            // getting mesh properties
                            float ambient = rectangle.getProperty().get_ambient();
                            Colour c = rectangle.getProperty().get_colour();
                            intensity*=ambient;
                            util.plot_colour_with_intensity(i, j, small_t, r, fb, intensity, c);
                            hit_rectangle = true;
                            object_indice = rc;
                        }
                    }

                    // loops every sphere.  if ray intersects, plot colour on pixel i,j.
                    for (int sp = 0; sp < sp_n; ++sp){
                        Sphere sphere = sps[sp];
                        t = util.hit_sphere(sphere, r)[0];
                        if (t < small_t){
                            small_t = t;
                            intersection_point = r.on_line(small_t);
                            intersection_normal = util.get_vector(sphere.getCentre() , intersection_point);
                            intersection_normal.normalise();

                            // getting mesh properties
                            float ambient = sphere.getProperty().get_ambient();
                            Colour c = sphere.getProperty().get_colour();
                            intensity*=ambient;
                            util.plot_colour_with_intensity(i, j, small_t, r, fb, intensity, c);
                            hit_sphere = true;
                            hit_rectangle = false;
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
                                    intensity*=ambient;

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
                    }else if(hit_rectangle){
                        property = rects[object_indice].getProperty();
                    }

                    Colour c = property.get_colour();
                    float diffuse_coef = property.get_diffuse();
                    float specular_coef = property.get_specular();
                    float ambient_coef = property.get_ambient();
                    bool is_reflective = property.get_reflective();
                    bool is_refractive = property.get_refractive();

                    if (is_reflective){
                        c = reflected(intersection_point, r, intersection_normal, reflection_depth, intensity, point_light);
                        util.plot_colour_with_intensity(i,j,small_t,r,fb, intensity, c);
                    }

                    if (small_t != maxfloat){
                        if (is_reflective){
                            intensity*=get_surface_intensity(intersection_point, intersection_normal, point_light, property);
                            util.plot_colour_with_intensity(i,j,small_t,r,fb, intensity, c);
                        }else{
                            intensity = get_surface_intensity(intersection_point, intersection_normal, point_light, property);
                            util.plot_colour_with_intensity(i,j,small_t,r,fb, intensity, c);
                        }
                        
                        // checking if surface shoul be shadowed
                        bool shadowed = false;
                        if (include_shadow){
                            shadowed = in_shadow(r, point_light, intersection_point);
                            if (shadowed){
                                // if surface is shadowed, reduce intensity to 0.3 and plot
                                intensity = 0.3;
                                util.plot_colour_with_intensity(i,j,small_t,r,fb, intensity, c);
                            }
                        }
                    }
                    if (is_refractive && hit_sphere){
                        c = refracted(intersection_point, r, intersection_normal, sps[object_indice], intensity, point_light);
                        intensity*=get_surface_intensity(intersection_point, intersection_normal, point_light, property);
                        util.plot_colour_with_intensity(i,j,small_t,r,fb, intensity, c);
                    }
                }
            }
        }
 
        float get_surface_intensity(Vertex intersection_point, Vector3 normal, PointLight pl, Properties property){
            float intensity = property.get_ambient(); 
            intensity+=diffuse(intersection_point, normal, pl, property);
            intensity+=specular(intersection_point, normal, pl, property);
            return intensity;
        }

        bool in_shadow(Ray r, PointLight pl, Vertex intersection_point){
            Utils util = Utils();
            // set up
            float maxfloat = std::numeric_limits<float>::max();
            float max = maxfloat;
            int mesh_n = meshes.size();
            int sp_n = sps.size();
            float shift_t = 0.01;

            // getting a shadow ray
            Vector3 vec_for_ray = util.get_vector(intersection_point,pl.get_light_position());
            vec_for_ray.normalise();

            // shifting the ray away from the sphere slightly
            Ray shadow_ray = Ray(intersection_point, vec_for_ray);
            shadow_ray.origin = shadow_ray.on_line(shift_t);

            float t_to_light = shadow_ray.get_t(pl.get_light_position());
            
            // loops every sphere if there is one, plot
            for (int sp = 0; sp < sp_n; ++sp){
                Sphere sphere = sps[sp];

                float t2 = util.hit_sphere(sphere, shadow_ray)[0];
                // if there is intersection and it doesnt face backwards from light
                if (t2 > 0 && t2 != max && t2 < t_to_light){
                    return true;
                }
            }
            return false;
        }
        
        float diffuse(Vertex intersection_point, Vector3 normal, PointLight pl, Properties property){
            Utils util = Utils();
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
            
            if (dot < 0) return 0;

            return (pow(dot, 40)*property.get_specular());
        }

        // needs to return the intensity and the colour of the reflected object, so always pass intensity=0 as a parameter 
        // and modify it in the function, and return the colour normally 
        Colour reflected(Vertex intersection_point, Ray r, Vector3 normal, int depth, float &intensity , PointLight pl){

            Utils util = Utils();
            float shift_t = 0.0001;
            float maxfloat = std::numeric_limits<float>::max();
            float max = maxfloat;
            int mesh_n = meshes.size();
            int sp_n = sps.size();
            int rect_n = rects.size();

            // firing reflectved ray using: 𝑟=𝑑−2(𝑑⋅𝑛)𝑛
            Vector3 vec_reflected = r.get_direction() - (normal * (2 * util.dot(r.get_direction(),normal)));
            vec_reflected.normalise();

            Ray reflected_ray = Ray(intersection_point, vec_reflected);
            reflected_ray.origin = reflected_ray.on_line(shift_t);

            // is there an intersection with a sphere?
            for (int sp = 0; sp < sp_n; ++sp){
                Sphere sphere = sps[sp];
                float t2 = util.hit_sphere(sphere, reflected_ray)[0];
                Vertex new_intersection_point = reflected_ray.on_line(t2);
                Vector3 new_intersection_normal = util.get_vector(sphere.getCentre() , new_intersection_point);
                new_intersection_normal.normalise();

                //if there is intersection and it doesnt face backwards from light and if the depth is larger than 1,
                //call reflected again to find a new object to intersect with --depth
                if (t2 > 0 && t2 != max){
                    if (depth>1){
                        cout << sphere.getProperty().get_refractive() << "\n";
                        

                        if (!sphere.getProperty().get_reflective()){
                            intensity = get_surface_intensity(new_intersection_point, new_intersection_normal, pl, sphere.getProperty());
                            bool shadowed_prime = in_shadow(reflected_ray, point_light, new_intersection_point);
                            if (shadowed_prime) intensity = 0.3;
                            return sphere.getProperty().get_colour();
                        }
                        return reflected(new_intersection_point, reflected_ray, new_intersection_normal, depth - 1, intensity, pl);
                    }else{
                        intensity = get_surface_intensity(new_intersection_point, new_intersection_normal, pl, sphere.getProperty());
                        bool shadowed_prime = in_shadow(reflected_ray, point_light, new_intersection_point);
                        if (shadowed_prime) intensity = 0.3;
                        if (sphere.getProperty().get_refractive()){
                                Colour c = refracted(new_intersection_point, reflected_ray, new_intersection_normal, sphere, intensity, pl);
                                intensity*=get_surface_intensity(new_intersection_point, new_intersection_normal, pl, sphere.getProperty());
                                return c;
                        }
                        return sphere.getProperty().get_colour();
                    }
                    
                }
            }

            //is there an intersection with a rectangle?
            for (int rc = 0; rc < rect_n; ++rc){
                Rectangle rectangle = rects[rc];
                float t2 = util.hit_rectangle_plane_algo(rectangle, reflected_ray);
                Vertex new_intersection_point = reflected_ray.on_line(t2);
                Vector3 new_intersection_normal = rectangle.get_normal();

                //if there is intersection and it doesnt face backwards from light and if the depth is larger than 1,
                //call reflected again to find a new object to intersect with --depth
                if (t2 > 0 && t2 != max){
                    if (depth>1){
                        if (!rectangle.getProperty().get_reflective()){
                            intensity = get_surface_intensity(new_intersection_point, new_intersection_normal, pl, rectangle.getProperty());
                            return rectangle.getProperty().get_colour();
                        }
                        return reflected(new_intersection_point, reflected_ray, new_intersection_normal, depth - 1, intensity, pl);
                    }else{
                        intensity = get_surface_intensity(new_intersection_point, new_intersection_normal, pl, rectangle.getProperty());
                        return rectangle.getProperty().get_colour();
                    }
                    
                }
            }
            //BUG HERE: weird with deoth = 2
            // if reflected ray intersectes with nothing plot white

            return Colour(0,0,0);
        }
        
        // this is only used for spheres -> walls(i just need to show it works), if i wanted it to work on any object,
        // the object would be passed as a parameter, and a second intersection from the inside could easily be calculated.

        Colour refracted(Vertex intersection_point, Ray r, Vector3 normal, Sphere s, float &intensity, PointLight pl){
            float shift_t = 0.001;
            float maxfloat = std::numeric_limits<float>::max();
            float max = maxfloat;
            Utils util = Utils();
            // gets refractive indexes
            float n1 = 1.0;
            float n2 = 1.02; // like water (this is set to default, will only have 1 refracted object)
            float refractive_ratio_enter = n1/n2;
            float refractive_ratio_exit = n2/n1;
            
            // calculating the first refracted ray by adding up the perpendicular 
            //and parallel (from normal) rays by cos(theta)
            float cos_theta = fmin(util.dot(-r.get_direction(), normal), 1.0);
            Vector3 r_out_perp =  (r.get_direction() + normal*cos_theta)*refractive_ratio_enter;
            Vector3 r_out_parallel = normal* (-sqrt( fabs (-r_out_perp.length_squared()+1 )));
            Vector3 vector_for_refracted = r_out_perp + r_out_parallel;
            vector_for_refracted.normalise();
            Ray r2 = Ray(intersection_point, vector_for_refracted);
            //r2.origin = r2.on_line(shift_t);

            // second intersection with the sphere
            float t2 = util.hit_sphere(s, r2)[1];
            Vertex intersection_point2 = r2.on_line(t2);
            Vector3 intersection_normal2 = util.get_vector(intersection_point2, s.getCentre() );
            intersection_normal2.normalise();

            // calculating the first refracted ray by adding up the perpendicular 
            //and parallel (from normal) rays by cos(theta)
            float cos_theta2 = fmin(util.dot(-r2.get_direction(), intersection_normal2), 1.0);
            Vector3 r_out_perp2 =  (r2.get_direction() + intersection_normal2*cos_theta2)*refractive_ratio_exit;
            Vector3 r_out_parallel2 = intersection_normal2* (-sqrt( fabs (-r_out_perp2.length_squared()+1 )));
            Vector3 vector_for_refracted2 = r_out_perp2+ r_out_parallel2;
            vector_for_refracted2.normalise();
            Ray r3 = Ray(intersection_point2, vector_for_refracted2);
            //r3.origin = r3.on_line(shift_t);

            

            //is there an intersection with a rectangle?
            int rect_n = rects.size();
            for (int rc = 0; rc < rect_n; ++rc){
                Rectangle rectangle = rects[rc];
                float t_final = util.hit_rectangle_plane_algo(rectangle, r3);
                Vertex final_intersection_point = r3.on_line(t_final);
                Vector3 final_intersection_normal = rectangle.get_normal();

                if (t_final > 0 && t_final != max){
                    intensity = get_surface_intensity(final_intersection_point, final_intersection_normal, pl, rectangle.getProperty())+0.03;
                    return rectangle.getProperty().get_colour();
                }
            }
            // cout << "Origin: " << r.get_origin().x << "," << r.get_origin().y << "," << r.get_origin().z << ",  ";
            // cout << r2.get_origin().x << "," << r2.get_origin().y << "," << r2.get_origin().z << ",  ";
            // cout << r3.get_origin().x << "," << r3.get_origin().y << "," << r3.get_origin().z << "   t2:" << t2 << "\n";
            // cout << intersection_normal2.x << "," << intersection_normal2.y << "," << intersection_normal2.z  << "\n";
            // cout << "Direction: " << r.get_direction().x << "," << r.get_direction().y << "," << r.get_direction().z << ",  ";
            // cout << r2.get_direction().x << "," << r2.get_direction().y << "," << r2.get_direction().z << ",  ";
            // cout << r3.get_direction().x << "," << r3.get_direction().y << "," << r3.get_direction().z << "\n\n";
            Colour c = Colour(0,0,0);
            return c;
        }


        void add_Cornell_box(){
            Properties front_wall_property = Properties(Colour(1,1,1), 0.5, 0.4, 0.2, false, false);
            Vector3 v_front_wall = Vector3(3.0,0.0,0.0); 
            Vector3 u_front_wall = Vector3(0.0,3.0,0.0); 
            Rectangle front_wall = Rectangle(Vertex(-1.5,-1.5,7), v_front_wall, u_front_wall, front_wall_property);

            Properties right_wall_property = Properties(Colour(0,1,0), 0.5, 1, 0.2, false, false);
            Vector3 v_right_wall = Vector3(0.0,0.0,-3.0); 
            Vector3 u_right_wall = Vector3(0.0,3.0,0.0); 
            Rectangle right_wall = Rectangle(Vertex(1.5,-1.5,7), v_right_wall, u_right_wall, right_wall_property);

            Properties left_wall_property = Properties(Colour(1,0,0), 0.5, 1, 0.2, false, false);
            Vector3 v_left_wall = Vector3(0.0,0.0,3.0); 
            Vector3 u_left_wall = Vector3(0.0,3.0,0.0); 
            Rectangle left_wall = Rectangle(Vertex(-1.5,-1.5,4), v_left_wall, u_left_wall, left_wall_property);

            Properties floor_property = Properties(Colour(1,1,1), 0.5, 1, 0.2, false, false);
            Vector3 v_floor = Vector3(3.0,0.0,0.0); 
            Vector3 u_floor = Vector3(0.0,0.0,3.0); 
            Rectangle floor = Rectangle(Vertex(-1.5,-1.5,4), v_floor, u_floor, floor_property);

            Properties ceiling_property = Properties(Colour(1,1,1), 0.5, 0.7, 0.2, false, false);
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



