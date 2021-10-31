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
    const float aspect_ratio = 16.0/9.0;
    const int img_w = 400;
    const int img_h = static_cast<int>(img_w / aspect_ratio);

    // init framebuffer object
    FrameBuffer *fb = new FrameBuffer(img_w,img_h);
    
    // camera and viewport settings
    float viewport_h = 2.0;
    float viewport_w = aspect_ratio * viewport_h;
    float focal_length = 1.0;

    Vector3 origin = Vector3(0, 0, 0);
    Vector3 horizontal = Vector3(viewport_w, 0, 0);
    Vector3 vertical = Vector3(0, viewport_h, 0);

    Utils util = Utils();
    Vector3 lower_left_corner;

    lower_left_corner = util.deduct_vectors(origin, util.divide_vector_by_factor(horizontal,2));
    lower_left_corner = util.deduct_vectors(lower_left_corner, util.divide_vector_by_factor(vertical,2));
    lower_left_corner = util.deduct_vectors(lower_left_corner, Vector3(0, 0, focal_length));


    int counter = img_h/2;

    // iterate every pixel starting from top left
    for (int j = img_h-1; j >= 0; --j) {
        for (int i = 0; i < img_w; ++i) {
            
            // getting new u and v
            float u = double(i) / (img_w-1);
            float v = double(j) / (img_h-1);

            
            // getting the ray by adding up vectors from left corner of the view port,
            // its vector to the horizontall, its vector to its vertical and the origin.
            Vector3 vector_for_ray;
            Vector3 dist_x = util.multiply_vector_by_factor(u,horizontal);
            Vector3 dist_y = util.multiply_vector_by_factor(v,vertical);

            // transforming the vector into position
            vector_for_ray = util.add_vectors(dist_x , lower_left_corner);
            vector_for_ray = util.add_vectors(vector_for_ray, util.multiply_vector_by_factor(v,vertical));
            vector_for_ray = util.deduct_vectors(vector_for_ray, origin);

            // normalise
            vector_for_ray.normalise();

            // generate ray from origin
            Ray r(Vertex(0,0,0), vector_for_ray);

            // find intersection points.
            float t = util.hit_sphere(Vertex(0,0,1), 0.5, r);
            
            if (t > 0){
                // getting the normal if there is an intersection
                Vector3 normal = util.deduct_vectors(r.on_line(t), Vector3(0,0,1));

                normal.normalise();
                float R = (normal.x+1)/2;
                float G = (normal.y+1)/2;
                float B = (normal.z+1)/2;

                // plot pixel
                // fb->plotPixel(i,j, R,G,B);
                fb->plotDepth(i, j, t);
                
            }else{
                // fb->plotPixel(i,j, 0.0,0.0,0.0);
                fb->plotDepth(i, j, 0.0);
            }
        }
    }
    fb->writeDepthFile((char*)("image.ppm"));
    std::cerr << "\nDone.\n";
}