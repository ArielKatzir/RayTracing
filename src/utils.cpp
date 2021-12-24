#include <vector>
#include "utils.h"
using namespace std;


Utils::Utils(){
    max = std::numeric_limits<float>::max();
}

Vector3 Utils::get_vector(Vertex a, Vertex b){
    return Vector3(b.x-a.x, b.y-a.y, b.z-a.z);
} 

Vertex Utils::to_ver(Vector3 v){
    return Vertex(v.x,v.y,v.z);
}

Vertex Utils::vector_plus_vertex(Vector3 v ,Vertex u){
    return Vertex(v.x + u.x, v.y + u.y, v.z + u.z);
}

// get the dot product of two vectors
double Utils::dot(const Vector3 &u, const Vector3 &v){
    return u.x * v.x + u.y * v.y + u.z * v.z;
}
// get the dot product of two vectors
double Utils::dot(const Vector3 &u, const Vertex &v){
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

// get the cross product of two vectors
Vector3 Utils::cross(const Vector3 &u, const Vector3 &v){
    return Vector3( u.y * v.z - u.z * v.y,
                    u.z * v.x - u.x * v.z,
                    u.x * v.y - u.y * v.x
    );
}

void Utils::add(Colour &c1 , Colour &c2){
    c1.red+=c2.red;
    c1.green+=c2.green;
    c1.blue+=c2.blue;
}

void Utils::mean(Colour &sum, int n){
    sum.red/=n;
    sum.green/=n;
    sum.blue/=n;
}

void Utils::multiply(Colour &c1 , float n){
    c1.red*=n;
    c1.green*=n;
    c1.blue*=n;
}

void Utils::divide(Colour &c1 , float n){
    c1.red/=n;
    c1.green/=n;
    c1.blue/=n;
}

// in this, take another parameter: intensity to know the exact ration needed.
void Utils::colour_bleed(Colour &c , Colour bleed){
    multiply(c , 5);
    multiply(bleed , 5);
    add(c,bleed);
    divide(c,10);
}

float Utils::hit_triangle_plane_algo(Triangle triangle, Ray r){
    // https://courses.cs.washington.edu/courses/cse557/09au/lectures/extras/triangle_intersection.pdf
    Vertex A = triangle.getVertex0();                                   
    Vertex B = triangle.getVertex1();                                  
    Vertex C = triangle.getVertex2(); 

    Vector3 AB = get_vector(A,B);
    Vector3 AC = get_vector(A,C);
    Vector3 BC = get_vector(B,C);
    Vector3 CA = -AC;

    Vector3 normal = cross(AB,AC);
    normal.normalise();

    float d = -dot(normal,A);

    float maxfloat = std::numeric_limits<float>::max();

    // find intersection points.
    float t = (-(dot(normal,r.origin) + d))/(dot(normal,r.direction));

    // intersection point
    Vertex P = vector_plus_vertex(r.direction*t,r.origin);

    Vector3 AP,BP,CP;
    AP = get_vector(A,P);
    BP = get_vector(B,P);
    CP = get_vector(C,P);

    float dot_cross_pa = dot(cross(AB,AP),normal);
    float dot_cross_pb = dot(cross(BC,BP),normal);
    float dot_cross_pc = dot(cross(CA,CP),normal);

    if (dot_cross_pa>=0.0 && dot_cross_pb>=0.0 && dot_cross_pc>=0.0){
        return t;
    }else{
        return maxfloat;
    }

}

std::vector<float> Utils::hit_sphere(Sphere s, Ray& r) {
    Utils util = Utils();

    Vertex centre = s.getCentre();
    float radius = s.getRadius();
    
    // get vector from origin to centre
    Vector3 co = util.get_vector(centre , r.origin);

    float maxfloat = std::numeric_limits<float>::max();
    float max = maxfloat;

    // set discriminant values
    float a = util.dot(r.get_direction(), r.get_direction());
    float b = 2.0 * util.dot(co, r.get_direction());
    float c = util.dot(co, co) - radius*radius;
    float discriminant = b*b - 4*a*c;
    std::vector<float> intersections;
    // return intersection value if there is an intersection - discriminant is less than 0.
    if (discriminant < 0){
        intersections.push_back(max);
        intersections.push_back(max);
        return intersections;
    }
    else{
        intersections.push_back( (-b - sqrt(discriminant))/ (2.0 * a));
        intersections.push_back( (-b + sqrt(discriminant))/ (2.0 * a));
        return intersections;
    }
}        

float Utils::hit_triangle_moller_trumbore(Triangle triangle, Ray r){
    //https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

    float maxfloat = std::numeric_limits<float>::max();

    const float EPSILON = 0.0000001;
    Vertex vertex0 = triangle.getVertex0();
    Vertex vertex1 = triangle.getVertex1();  
    Vertex vertex2 = triangle.getVertex2();
    Vector3 edge1, edge2, h, s, q;
    float a,f,u,v;

    edge1 = get_vector(vertex0,vertex1);
    edge2 = get_vector(vertex0,vertex2);

    h = cross(r.direction,edge2);
    a = dot(edge1,h);

    if (a > -EPSILON && a < EPSILON)
        return maxfloat;    // This ray is parallel to this triangle.
    f = 1.0/a;
    s = get_vector(vertex0,r.origin);
    u = f * dot(s,h);
    if (u < 0.0 || u > 1.0)
        return maxfloat;
    q = cross(s,edge1);
    v = f * dot(r.direction,q);
    if (v < 0.0 || u + v > 1.0)
        return maxfloat;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * dot(edge2,q);
    if (t > EPSILON){
        return t;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return maxfloat;
}

void Utils::plot_colour_with_intensity(int i, int j, FrameBuffer *fb, float I, Colour c){
        
        float red = c.red*I;
        float green = c.green*I;
        float blue = c.blue*I;

        if (red > 1){
            red = 1;
        }
        if (blue > 1){
            blue = 1;
        }
        if (green > 1){
            green = 1;
        }

        fb->plotPixel(i, j, red, green, blue);
}

float Utils::hit_rectangle_plane_algo(Rectangle rectangle, Ray r){
    float maxfloat = std::numeric_limits<float>::max();

    Vector3 normal = rectangle.get_normal();            
    float d = -dot(normal,rectangle.b);

    // find intersection points.
    float t = (-(dot(normal,r.origin) + d))/(dot(normal,r.direction));

    // intersection point
    Vertex p = vector_plus_vertex(r.direction*t,r.origin);


    if (p.x<=rectangle.maxx() && p.y<=rectangle.maxy() && p.z<=rectangle.maxz() && p.x>=rectangle.minx() && p.y>=rectangle.miny() && p.z>=rectangle.minz()){
        return t;
    }else{
        return maxfloat;
    }

}


void Utils::intersect_rectangle(Ray r, float &smallest_t, Vector3 &normal, Vertex &intersection, Properties &property, std::vector<Rectangle> rects){
    for (int rc = 0; rc < rects.size(); ++rc){
        Rectangle rectangle = rects[rc];
        float t = hit_rectangle_plane_algo(rectangle, r);
        if (t < smallest_t && t > 0 && t!=max){
            smallest_t = t;
            intersection = r.on_line(smallest_t);
            normal = rectangle.get_normal();
            normal.normalise();
            property = rectangle.getProperty();
        }
    }

}

void Utils::intersect_sphere(Ray r, float &smallest_t, Vector3 &normal, Vertex &intersection, Properties &property, std::vector<Sphere> spheres){
    for (int sp = 0; sp < spheres.size(); ++sp){
        Sphere sphere = spheres[sp];
        // take the closest t
        float t = hit_sphere(sphere, r)[0];
        if (t < smallest_t && t > 0 && t!=max){
            smallest_t = t;
            intersection = r.on_line(smallest_t);
            normal = get_vector(sphere.getCentre() , intersection);
            normal.normalise();
            property = sphere.getProperty();
        }
    }

}   

void Utils::intersect_mesh(Ray r, float &smallest_t, Vector3 &normal, Vertex &intersection, Properties &property, std::vector<PolyMesh*> meshes){
    // loop every polymesh
    float t;
    for (int mesh = 0; mesh < meshes.size(); ++mesh){
        PolyMesh *pm = meshes[mesh];
        if (meshes[mesh]->triangle_count != 0){
            for (int tri = 0; tri < pm->triangle_count; tri += 1){
                Triangle triangle = Triangle(pm->vertex[pm->triangle[tri][0]], pm->vertex[pm->triangle[tri][1]], pm->vertex[pm->triangle[tri][2]]);
                t = hit_triangle_moller_trumbore(triangle, r);
                if (t < smallest_t && t > 0 && t!=max){
                    smallest_t = t;

                    // getting the intersection point using t and the normal from surface
                    intersection = r.on_line(smallest_t);
                    Vertex A = triangle.getVertex0();                                   
                    Vertex B = triangle.getVertex1();                                  
                    Vertex C = triangle.getVertex2(); 
                    Vector3 AB = get_vector(A,B);
                    Vector3 AC = get_vector(A,C);
                    normal = cross(AB,AC);
                    normal.normalise();

                    property = pm->getProperty();
                }
            }
        }
    }
}

// removes all of the recorded intersections after hitting a 
// refractive object since these intersections were not refracted when hit the object
void Utils::remove_after_refraction(std::vector<Photon_Hit> &hits){
    for (int i = 0; i < hits.size(); i++){
        if(hits[i].type == 2){
            for (int j = 0; j < hits.size(); j++){
                if (hits[j].depth == hits[i].depth && hits[j].t > hits[i].t){
                    hits.erase(hits.begin() + j);
                }
            }
        }
    }
}

// converts all of the recorded intersections after hitting a 
// refractive object to lit from shadow
void Utils::convert_after_refraction(std::vector<Photon_Hit> &hits){
    for (int i = 0; i < hits.size(); i++){
        if(hits[i].type == 2){
            for (int j = 0; j < hits.size(); j++){
                if (hits[j].depth == hits[i].depth && hits[j].t > hits[i].t){
                    hits[j].photon.energy = 1.2;
                }
            }
        }
    }
}




void Utils::intersect_object(  Ray r, 
                        float &smallest_t, 
                        Vector3 &normal, 
                        Vertex &intersection, 
                        Properties &property, 
                        std::vector<Rectangle> rects,
                        std::vector<Sphere> spheres,
                        std::vector<PolyMesh*> meshes
                        ){
    intersect_rectangle(r,smallest_t,normal,intersection,property,rects);
    intersect_sphere(r,smallest_t,normal,intersection,property,spheres);
    intersect_mesh(r,smallest_t,normal,intersection,property,meshes);

}