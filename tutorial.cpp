#include <iostream>
#include <sstream>
#include "ray.h"
#include "my_vector.h"
#include "vertex.h"
#include "utils.h"
#include "hit.h"
#include "framebuffer.h"




// compile with:  g++ lab_test.cpp framebuffer.cpp
// execute with: ./a.out     

int main() {

    // image setting
    const int img_w = 512;
    const int img_h = 256;

    // init framebuffer object
    FrameBuffer *fb = new FrameBuffer(img_w,img_h);
    
    // camera and viewport settings
    float viewport_w = 2.0;
    float viewport_h = 1.0;
    float focal_length = 1.0;

    Vector3 origin = Vector3(0, 0, 0);
    Vector3 horizontal = Vector3(viewport_w, 0, 0);
    Vector3 vertical = Vector3(0, -viewport_h, 0);

    Utils util = Utils();

     //trisngle
    Vertex A = Vertex(0 , 1.25 , 3);                                    
    Vertex B = Vertex(2 , -1.25, 3);                                    
    Vertex C = Vertex(-2, -1.25, 3);  

    Vector3 AB = util.get_vector_ab_from_vertices(A,B);
    Vector3 AC = util.get_vector_ab_from_vertices(A,C);
    Vector3 BC = util.get_vector_ab_from_vertices(B,C);
    Vector3 CA = util.get_vector_ab_from_vertices(C,A);



    Vector3 normal = util.cross(AB,AC);
    normal.normalise();

    float d = -util.dot(normal,A);
    

    Vector3 top_left_corner;
    top_left_corner = util.deduct_vectors(origin, util.divide_vector_by_factor(horizontal,2));
    top_left_corner = util.deduct_vectors(top_left_corner, util.divide_vector_by_factor(vertical,2));
    top_left_corner = util.add_vectors(top_left_corner, Vector3(0, 0, focal_length));


    // iterate every pixel starting from top left
    for (int j = 0; j < img_h; j++) {
        for (int i = 0; i < img_w; i++) {
            
            // getting new u and v
            float u = double(i) / (img_w);
            float v = double(j) / (img_h);
            
            // getting the ray by adding up vectors from left corner of the view port,
            // its vector to the horizontall, its vector to its vertical and the origin.
            Vector3 vector_for_ray = util.multiply_vector_by_factor(horizontal, u);
            vector_for_ray = util.add_vectors(vector_for_ray, util.multiply_vector_by_factor(vertical, v));
            vector_for_ray = util.add_vectors(vector_for_ray, top_left_corner);
            
            // normalise
            vector_for_ray.normalise();

            //std::cerr << vector_for_ray.x << " " << vector_for_ray.y << " " << vector_for_ray.z << "\n";

            // generate ray from origin
            Ray r(Vertex(0,0,0), vector_for_ray);

            // find intersection points.
            float t = (-(util.dot(normal,r.origin) + d))/(util.dot(normal,r.direction));

            //std::cout << t << "\n";

            if (t<=0){
                fb->plotPixel(i,j,0.0,0.0,0.0);
                break;
            }



            // intersection point
            Vertex P = util.to_ver(util.add_to_vector(util.multiply_vector_by_factor(r.direction,t),r.origin));

            Vector3 AP,BP,CP;
            AP = util.get_vector_ab_from_vertices(A,P);
            BP = util.get_vector_ab_from_vertices(B,P);
            CP = util.get_vector_ab_from_vertices(C,P);

            float dot_cross_pa = util.dot(util.cross(AB,AP),normal);
            float dot_cross_pb = util.dot(util.cross(BC,BP),normal);
            float dot_cross_pc = util.dot(util.cross(CA,CP),normal);

            //std::cerr << dot_cross_pa << " " << dot_cross_pb << " " << dot_cross_pc << "\n";



            if (dot_cross_pa>=0.0 && dot_cross_pb>=0.0 && dot_cross_pc>=0.0){
                fb->plotPixel(i,j,0.0,1.0,1.0);
            }else{
                fb->plotPixel(i,j,0.0,0.0,0.0);
            }

            //std::cerr << dot_cross_pa << " " << dot_cross_pb << " " << dot_cross_pc << "\n";

            
            
        }
    }
    fb->writeRGBFile((char*)("image.ppm"));
    std::cerr << "\nDone.\n";
}
