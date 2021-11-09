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
#include "scene.h"

#include <limits>
#include <cmath>


// TDOO
// - fix weird depth 
// - fix stretch if moved - camera issue as problem occures with teapot aend sphere. 
// - lighting
// - fix small teapot rotation



// compile with:  g++ lab_test.cpp framebuffer.cpp polymesh.cpp
// execute with: ./a.out     

int main() {

    // image setting
    const int img_w = 500;
    const int img_h = 250;
    
    // camera and viewport settings
    float viewport_w = 2.0;
    float viewport_h = 1.0;
    float focal_length = 1.0;

    // init scene object (also includes camera settings)
    Scene scene = Scene(img_w,img_h,viewport_w,viewport_h,focal_length);

    // init framebuffer object
    FrameBuffer *fb = new FrameBuffer(img_w,img_h);


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
                                            0.0f, 0.0f, 1.0f, 8.0f,
                                            0.0f,0.0f,0.0f,1.0f);

    // Read in the  model.
    PolyMesh *pm = new PolyMesh((char *)"teapot.ply", transform);        

    Sphere sp = Sphere(Vertex(1,0,1.3) , 0.5);
    Sphere sp2 = Sphere(Vertex(-1,0,1.3) , 0.5);

    scene.add_mesh(pm);
    scene.add_sphere(sp);
    scene.add_sphere(sp2);
    scene.render(fb);

    //fb->writeRGBFile((char*)("image_rgb.ppm"));
    fb->writeDepthFile((char*)("image_depth.ppm"));

    
    std::cerr << "\nDone.\n";
}
