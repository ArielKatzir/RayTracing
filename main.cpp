#include <iostream>
#include <sstream>
#include "ray.h"
#include "my_vector.h"
#include "vertex.h"
#include "utils.h"
#include "framebuffer.h"
#include "transform.h"
#include "polymesh.h"
#include "scene.h"

#include <limits>
#include <cmath>


// TDOO
// - *** MUST - Have reflective and refractive indexes instead of boolean values, and therefore implement fresnel's law. ***
// - make a function that iterates over all of the objects, it should be called in render,reflect and refract
// - shadow and reflection is only implemented if spheres obstruct the view
// - look for places where i send a ray/photon and dont move it slightly away from the surface


// compile with:  g++ main.cpp framebuffer.cpp polymesh.cpp
// execute with: ./a.out     

int main() {

    
    // image setting
    const int img_w = 2500;
    const int img_h = 2500;
    
    // camera and viewport settings
    float viewport_w = 1.0;
    float viewport_h = 1.0;
    float focal_length = 2.7;

    // init scene object (also includes camera settings)
    Scene scene = Scene(img_w,img_h,viewport_w,viewport_h,focal_length);

    // init framebuffer object
    FrameBuffer *fb = new FrameBuffer(img_w,img_h);


    //triangle for single triangle testing
    Vertex A = Vertex(0 , 1.25 , 3);                                    
    Vertex B = Vertex(2 , -1.25, 3);                                    
    Vertex C = Vertex(-2, -1.25, 3);  
    Triangle tri = Triangle(A,B,C);

    int theta = 90;

    // The following transform allows 4D homogeneous coordinates to be transformed. It moves the supplied teapot model to somewhere visible.
    Transform *transform = new Transform(   0.2f, 0.0f, 0.0f, 0.7f,
                                            0.0f, 0.0f, 0.2f, -1.0f,
                                            0.0f, -0.2f, 0.0f, 6.0f,
                                            0.0f,0.0f,0.0f,1.0f);

    
    

    // teapot
    Properties teapot_property = Properties(Colour(0.9,0.5,0.7), 0.8, 0.8, 0.2, false, false);
    PolyMesh *pm = new PolyMesh((char *)"teapot_smaller.ply", transform, teapot_property);     

    Properties sp_property = Properties(Colour(0.3,0.5,0.7), 0.8, 0.8, 0.2, false, false);
    Sphere sp = Sphere(Vertex(1.3,1,4.5) , 0.2, sp_property);

    Properties sp_property2 = Properties(Colour(0.6,0.1,0.9), 0.8, 0.8, 0.2, false, false);
    Sphere sp2 = Sphere(Vertex(0,-0.3,5.3) , 0.2, sp_property2);

    Properties sp_property3 = Properties(Colour(0.8,0.0,0.0), 0.8, 0.8, 0.2, true, false);
    Sphere sp3 = Sphere(Vertex(-0.7,0.8,5.5) , 0.22, sp_property3);

    Properties sp_property4 = Properties(Colour(0.8,0.0,0.0), 0.8, 0.8, 0.2, false, true);
    Sphere sp4 = Sphere(Vertex(-1.11,-1.07,5.2) , 0.22, sp_property4);

    // *** Ambient light is assumed to be on always ***
    scene.add_mesh(pm);
    scene.add_sphere(sp);
    scene.add_sphere(sp2);
    scene.add_sphere(sp3);
    scene.add_sphere(sp4);
    scene.add_shadows();
    scene.add_Cornell_box();

    
    scene.render(fb);

    fb->writeRGBFile((char*)("image.ppm"));
    //fb->writeDepthFile((char*)("image_depth.ppm"));

    
    std::cerr << "\nDone.\n";
}
