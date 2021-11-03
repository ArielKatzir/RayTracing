#include "my_vector.h"
#include "vertex.h"
#include "my_triangle.h"
#include "polymesh.h"



class Utils {
    public:

        Utils(){}

        Vector3 add_to_vector(Vector3 u , Vertex v){
            return Vector3(u.x + v.x, u.y + v.y, u.z + v.z);
        }

        Vector3 get_vector_ab_from_vertices(Vertex a, Vertex b){
            return Vector3(b.x-a.x, b.y-a.y, b.z-a.z);
        }

        Vector3 deduct_from_vector(Vector3 u , Vertex v){
            return Vector3(u.x - v.x, u.y - v.y, u.z - v.z);
        }

        // add two vectors
        Vector3 add_vectors(Vector3 u , Vector3 v){
            return Vector3(u.x + v.x, u.y + v.y, u.z + v.z);
        }

        // multiply vector by a factor
        Vector3 multiply_vector_by_factor(float t , Vector3 v){
            return Vector3(t*v.x, t*v.y, t*v.z);
        }

        // multiply vector by a factor
        Vector3 multiply_vector_by_factor(Vector3 v, float t){
            return Vector3(t*v.x, t*v.y, t*v.z);
        }

        // divide vector by a factor
        Vector3 divide_vector_by_factor(Vector3 v, float t){
            return multiply_vector_by_factor((1/t) , v);
        }

        Vertex to_ver(Vector3 v){
            return Vertex(v.x,v.y,v.z);
        }

        Vector3 norm(Vector3 v){
            float x,y,z;
            x = v.x/v.length();
            y = v.y/v.length();
            z = v.z/v.length();
            return Vector3();
        }

        // get the dot product of two vectors
        inline double dot(const Vector3 &u, const Vector3 &v){
            return u.x * v.x + u.y * v.y + u.z * v.z;
        }
        // get the dot product of two vectors
        inline double dot(const Vector3 &u, const Vertex &v){
            return u.x * v.x + u.y * v.y + u.z * v.z;
        }

        // get the cross product of two vectors
        inline Vector3 cross(const Vector3 &u, const Vector3 &v){
            return Vector3( u.y * v.z - u.z * v.y,
                            u.z * v.x - u.x * v.z,
                            u.x * v.y - u.y * v.x
            );
        }

        // get the unit vector of a vector
        inline Vector3 unit_vector(Vector3 v){
            return divide_vector_by_factor(v , v.length());
        }

		Vector3 deduct_vectors(Vector3 vx, Vector3 v) {
			return Vector3(vx.x - v.x, vx.y - v.y, vx.z - v.z);
		}
         


    /* 
    * TODO
    * Not sure if its good here
    */
    float hit_sphere(const Vertex center, float radius, Ray& r) {
		Utils util = Utils();
		
		// get vector from origin to centre
		Vector3 oc = util.get_vector_ab_from_vertices(Vertex(0,0,0) , center);

		// set discriminant values
		float a = util.dot(r.get_direction(), r.get_direction());
		float b = 2.0 * util.dot(oc, r.get_direction());
		float c = util.dot(oc, oc) - radius*radius;
		float discriminant = b*b - 4*a*c;
		// return intersection value if there is an intersection - discriminant is less than 0.
        if (discriminant < 0){
            return -1.0;
        }
        else{
            // return -b- instead of -b+ because we want to closest value.
            return (-b - sqrt(discriminant))/ (2.0 * a);
        }
    }        

    float hit_triangle(Triangle triangle , Ray r){
        Vertex A = triangle.getVertex0();                                   
        Vertex B = triangle.getVertex1();                                  
        Vertex C = triangle.getVertex2(); 

        //std::cout <<  A.get_x() << " " << A.get_y() << " " << A.get_z() << "\n";  

        Vector3 AB = get_vector_ab_from_vertices(A,B);
        Vector3 AC = get_vector_ab_from_vertices(A,C);
        Vector3 BC = get_vector_ab_from_vertices(B,C);
        Vector3 CA = get_vector_ab_from_vertices(C,A);

        Vector3 normal = cross(AB,AC);
        normal.normalise();

        float d = -dot(normal,A);

        // find intersection points.
        float t = (-(dot(normal,r.origin) + d))/(dot(normal,r.direction));

        // intersection point
        Vertex P = to_ver(add_to_vector(multiply_vector_by_factor(r.direction,t),r.origin));

        Vector3 AP,BP,CP;
        AP = get_vector_ab_from_vertices(A,P);
        BP = get_vector_ab_from_vertices(B,P);
        CP = get_vector_ab_from_vertices(C,P);

        float dot_cross_pa = dot(cross(AB,AP),normal);
        float dot_cross_pb = dot(cross(BC,BP),normal);
        float dot_cross_pc = dot(cross(CA,CP),normal);

        if (dot_cross_pa>=0.0 && dot_cross_pb>=0.0 && dot_cross_pc>=0.0){
            return t;
        }else{
            return -1.0;
        }

    }

    void plot_polymesh_triangle(PolyMesh pm, Ray ray){

    }
         
};