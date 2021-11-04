#include "my_vector.h"
#include "vertex.h"
#include "utils.h"


int main(){
    Utils ut = Utils();
    Vector3 v = Vector3(1,2,3);
    Vector3 u = Vector3(3,3,3);
    Vector3 k;
    u/=3;
    float f2 = ut.dot(u,v);
    std::cout << f2 << "\n";

    

    std::cout << v.x << " " << v.y << " " << v.z << "\n";
    std::cout << u.x << " " << u.y << " " << u.z << "\n";
    std::cout << k.x << " " << k.y << " " << k.z << "\n";

}

//     Vector3 top_left_corner;
//     meta = origin-=(horizontal/=2);
//     meta = meta-=(vertical/=2);
//     meta = meta+=Vector3(0, 0, focal_length);




//     // getting the ray by adding up vectors from left corner of the view port,
//             // its vector to the horizontall, its vector to its vertical and the origin.
//             Vector3 vector_for_ray = horizontal*=u;
//             vector_for_ray = vector_for_ray+=(vertical*=v);
//             vector_for_ray = vector_for_ray+=top_left_corner;

            

// }