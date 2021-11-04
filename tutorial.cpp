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
#include <cmath>






// compile with:  g++ lab_test.cpp framebuffer.cpp
// execute with: ./a.out     

int main() {

    // image setting
    const int img_w = 1000;
    const int img_h = 500;

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

    int theta = 90;

    // The following transform allows 4D homogeneous coordinates to be transformed. It moves the supplied teapot model to somewhere visible.
    Transform *transform = new Transform(   1.0f, 0.0f, 0.0f, 0.0f,
                                            0.0f, 1.0f, 0.0f, -2.0f,
                                            0.0f, 0.0f, 1.0f, 7.0f,
                                            0.0f,0.0f,0.0f,1.0f);

    // Read in the  model.
    PolyMesh *pm = new PolyMesh((char *)"teapot.ply", transform);        

    Vector3 top_left_corner;
    top_left_corner = origin-(horizontal/2);
    top_left_corner = top_left_corner-(vertical/2);
    top_left_corner = top_left_corner+Vector3(0, 0, focal_length);

    // iterate every pixel starting from top left
    for (int j = 0; j < img_h; j++) {
        std::cerr << "\rScanlines remaining: " << j << '/' << img_h << std::flush;
        for (int i = 0; i < img_w; i++) {
            
            // getting new u and v
            float u = double(i) / (img_w);
            float v = double(j) / (img_h);
            
            // getting the ray by adding up vectors from left corner of the view port,
            // its vector to the horizontall, its vector to its vertical and the origin.
            Vector3 vector_for_ray = horizontal*u;
            vector_for_ray = vector_for_ray+(vertical*v);
            vector_for_ray = vector_for_ray+top_left_corner;

            // normalise
            vector_for_ray.normalise();

            // generate ray from origin
            Ray r(Vertex(0,0,0), vector_for_ray);

            util.plot_polymesh_triangle(fb, pm, r, i ,j);
        }
    }
    fb->writeRGBFile((char*)("image_depth.ppm"));
    fb->writeDepthFile((char*)("image_rgb.ppm"));

    
    std::cerr << "\nDone.\n";
}
