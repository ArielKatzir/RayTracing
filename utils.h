#include "my_vector.h"
#include "vertex.h"
#include "my_triangle.h"
#include "polymesh.h"
#include "framebuffer.h"
#include "ray.h"





class Utils {
    public:

        Utils(){}

        Vector3 get_vector(Vertex a, Vertex b){
            return Vector3(b.x-a.x, b.y-a.y, b.z-a.z);
        } 

        Vertex to_ver(Vector3 v){
            return Vertex(v.x,v.y,v.z);
        }

        Vertex vector_plus_vertex(Vector3 v ,Vertex u){
            return Vertex(v.x + u.x, v.y + u.y, v.z + u.z);
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
    
        float hit_sphere(const Vertex center, float radius, Ray& r) {
            Utils util = Utils();
            
            // get vector from origin to centre
            Vector3 oc = util.get_vector(Vertex(0,0,0) , center);

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

        float hit_triangle_plane_algo(Triangle triangle, Ray r){
            Vertex A = triangle.getVertex0();                                   
            Vertex B = triangle.getVertex1();                                  
            Vertex C = triangle.getVertex2(); 

            //std::cout <<  A.get_x() << " " << A.get_y() << " " << A.get_z() << "\n";  

            Vector3 AB = get_vector(A,B);
            Vector3 AC = get_vector(A,C);
            Vector3 BC = get_vector(B,C);
            Vector3 CA = -AC;

            Vector3 normal = cross(AB,AC);
            normal.normalise();

            float d = -dot(normal,A);

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
                return -1.0;
            }

        }

        void plot_polymesh_triangle(FrameBuffer *fb,
                                    PolyMesh *pm,
                                    Ray ray,
                                    int i,
                                    int j
        ){
            float maxfloat  = std::numeric_limits<float>::max();
            float small_t = maxfloat;
            for (int tri = 0; tri < pm->triangle_count; tri += 1){
                Triangle triangle = Triangle(pm->vertex[pm->triangle[tri][0]], pm->vertex[pm->triangle[tri][1]], pm->vertex[pm->triangle[tri][2]]);
                float t = hit_triangle_moller_trumbore(triangle, ray);
                if (t < maxfloat && t != -1.0){
                    small_t = t;
                }
            }

            if (small_t > 0 && small_t != maxfloat)
            {
                fb->plotPixel(i, j, 0.0, 1.0, 1.0);
                fb->plotDepth(i, j, small_t);
            }
        }


        float hit_triangle_moller_trumbore(Triangle triangle, Ray r){

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
                return -1.0;    // This ray is parallel to this triangle.
            f = 1.0/a;
            s = get_vector(vertex0,r.origin);
            u = f * dot(s,h);
            if (u < 0.0 || u > 1.0)
                return -1.0;
            q = cross(s,edge1);
            v = f * dot(r.direction,q);
            if (v < 0.0 || u + v > 1.0)
                return -1.0;
            // At this stage we can compute t to find out where the intersection point is on the line.
            float t = f * dot(edge2,q);
            if (t > EPSILON){
                return t;
            }
            else // This means that there is a line intersection but not a ray intersection.
                return -1.0;
        }



         
};