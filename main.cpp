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
// - fix stretch if moved - camera issue as problem occures with teapot aend sphere. 
// - shadow and reflection is only implemented if spheres obstruct the view
// - fix small teapot rotation
// - fix memory issue when trying to add 2 teapots - combine the meshes
// - use two lights (iterate lights)
// - use different colours lights
// - change reflection from bool to colour
// - fix the rounding error in rectangle


// compile with:  g++ main.cpp framebuffer.cpp polymesh.cpp
// execute with: ./a.out     


int main() {

    // image setting
    const int img_w = 2048;
    const int img_h = 2048;
    
    // camera and viewport settings
    float viewport_w = 1.0;
    float viewport_h = 1.0;
    float focal_length = 2;

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
    Transform *transform = new Transform(   1.0f, 0.0f, 0.0f, 1.0f,
                                            0.0f, 1.0f, 0.0f, -2.0f,
                                            0.0f, 0.0f, 1.0f, 5.5f,
                                            0.0f,0.0f,0.0f,1.0f);

    // Read in the  model.
    Properties teapot_property = Properties();
    PolyMesh *pm = new PolyMesh((char *)"teapot.ply", transform, teapot_property);        

    Properties sp_property = Properties(Colour(0.3,0.5,0.7), 0.8, 0.8, 0.5, false);
    Sphere sp = Sphere(Vertex(0.8,-0.6,5.2) , 0.2, sp_property);

    Properties sp_property2 = Properties(Colour(0.6,0.1,0.9), 0.8, 0.8, 0.5, false);
    Sphere sp2 = Sphere(Vertex(0.5,0,5.2) , 0.2, sp_property2);

    Properties sp_property3 = Properties(Colour(0.3,0.7,0.9), 0.8, 0.5, 0.9, true);
    Sphere sp3 = Sphere(Vertex(0.3,-0.4,5.5) , 0.22, sp_property3);


    // *** Ambient light is assumed to be on always ***
    //scene.add_mesh(pm);
    scene.add_sphere(sp);
    scene.add_sphere(sp2);
    scene.add_sphere(sp3);
    scene.add_shadows();
    scene.add_Cornell_box();


    scene.render(fb);

    fb->writeRGBFile((char*)("image.ppm"));
    //fb->writeDepthFile((char*)("image_depth.ppm"));

    
    std::cerr << "\nDone.\n";
}
