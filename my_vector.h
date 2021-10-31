#ifndef VECTOR
#define VECTOR

// code inspired by https://raytracing.github.io/books/RayTracingInOneWeekend.html

#include <cmath>
#include <iostream>
using std::sqrt;

class Vector3 {
    // init with 0-1 vals
    public:
        // if none given , init with 0,0,0

        float x,y,z;

        Vector3(){
            x = 0.0;
            y = 0.0;
            z = 0.0;
        } 

        Vector3(float a, float b, float c) {
            x = a;
            y = b;
            z = c;
        }

        // return vector values
        float get_x() const { return x; }
        float get_y() const { return y; }
        float get_z() const { return z; }

        // return the negative of the vector
        Vector3 operator-() const { return Vector3(-x, -y, -z); }

        // add to each dimention in the vector
        Vector3& operator+=(const Vector3 &v) {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }
        // multipy vector by a factor t
        Vector3& operator*=(const float t) {
            x *= t;
            y *= t;
            z *= t;
            return *this;
        }

        // get the dot product of two vectors (this)
        inline double dot( const Vector3 &v){
            return x * v.x + y * v.y + z * v.z;
        }

        // divide vector by a factor t
        Vector3& operator/=(const float t) {
            return *this *= 1/t;
        }

        void normalise(){
            float len = (float)sqrt((double)(x*x + y*y + z*z));
            x = x / len;
            y = y / len;
            z = z / len;
        }

        // get the square of the magnitude
        double length_squared() const {
            return x*x + y*y + z*z;
        }

        // get magnitude of the vector 
        double length() const {
            return sqrt(length_squared());
        }
};
#endif