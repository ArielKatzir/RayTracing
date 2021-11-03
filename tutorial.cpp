#include <iostream>
#include <sstream>
#include "ray.h"
#include "my_vector.h"
#include "vertex.h"
#include "utils.h"
#include "hit.h"
#include "framebuffer.h"
#include "transform.h"
#include "polymesh.h"
#include <limits>





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

    //triangle for single triangle testing
    Vertex A = Vertex(0 , 1.25 , 3);                                    
    Vertex B = Vertex(2 , -1.25, 3);                                    
    Vertex C = Vertex(-2, -1.25, 3);  

    Triangle tri = Triangle(A,B,C);

    // The following transform allows 4D homogeneous coordinates to be transformed. It moves the supplied teapot model to somewhere visible.
    Transform *transform = new Transform(   1.0f, 0.0f, 0.0f, 0.0f,
                                            0.0f, 1.0f, 0.0f, -2.0f,
                                            0.0f, 0.0f, 1.0f, 7.0f,
                                            0.0f,0.0f,0.0f,1.0f);

    // Read in the teapot model.
    PolyMesh *pm = new PolyMesh((char *)"teapot.ply", transform);

    Vector3 top_left_corner;
    top_left_corner = util.deduct_vectors(origin, util.divide_vector_by_factor(horizontal,2));
    top_left_corner = util.deduct_vectors(top_left_corner, util.divide_vector_by_factor(vertical,2));
    top_left_corner = util.add_vectors(top_left_corner, Vector3(0, 0, focal_length));

    float maxfloat  = std::numeric_limits<float>::max();

    // iterate every pixel starting from top left
    for (int j = 0; j < img_h; j++) {
        std::cerr << "\rScanlines remaining: " << j << '/' << img_h << std::flush;
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

            // generate ray from origin
            Ray r(Vertex(0,0,0), vector_for_ray);


            float small_t = maxfloat;
            for (int tri = 0; tri< pm->triangle_count; tri += 1){
                Triangle triangle = Triangle(pm->vertex[pm->triangle[tri][0]],pm->vertex[pm->triangle[tri][1]],pm->vertex[pm->triangle[tri][2]]);
                // std::cerr << triangle.getVertex0().x << " " << triangle.getVertex0().y << " " << triangle.getVertex0().z << "\n";
                float t = util.hit_triangle(triangle, r);
                if (t < maxfloat && t != -1.0){
                    small_t = t;
                    //std::cerr << small_t << " ";
                }
            }
            
            if (small_t > 0 && small_t !=maxfloat){
            fb->plotPixel(i,j,0.0,1.0,1.0);
            fb->plotDepth(i,j,small_t);
            }
        }
    }
    fb->writeRGBFile((char*)("image_depth.ppm"));
    fb->writeDepthFile((char*)("image_rgb.ppm"));

    
    std::cerr << "\nDone.\n";
}
