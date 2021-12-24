#ifndef UTILS_H
#define UTILS_H


#include "my_vector.h"
#include "vertex.h"
#include "my_triangle.h"
#include "polymesh.h"
#include "framebuffer.h"
#include "ray.h"
#include "my_sphere.h"
#include "photon_hit.h"
#include "rectangle.h"
#include <vector>
#include "colour.h"
using namespace std;


#pragma once

struct Point
    {
    float data[3];

    Point() {}
    Point(float x, float y, float z) {
        data[0] = x;
        data[1] = y;
        data[2] = z;
    }

    float operator [] (int i) const {
        return data[i];
    }

    bool operator == (const Point& p) const {
        return data[0] == p[0] && data[1] == p[1] && data[2] == p[2];
    }

    friend std::ostream& operator << (std::ostream& s, const Point& p) {
        return s << '(' << p[0] << ", " << p[1] << ", " << p[2] << ')';
    }
    };

class Utils {
    public:

        float max;

        Utils();
        void add(Colour &c1 , Colour &c2);
        void mean(Colour &sum, int n);
        void multiply(Colour &c1 , float n);
        void divide(Colour &c1 , float n);
        void colour_bleed(Colour &c , Colour bleed);
        void intersect_rectangle(Ray r, float &smallest_t, Vector3 &normal, Vertex &intersection, Properties &property, std::vector<Rectangle> rects);
        void intersect_sphere(Ray r, float &smallest_t, Vector3 &normal, Vertex &intersection, Properties &property, std::vector<Sphere> spheres);
        void intersect_mesh(Ray r, float &smallest_t, Vector3 &normal, Vertex &intersection, Properties &property, std::vector<PolyMesh*> meshes);
        void remove_after_refraction(std::vector<Photon_Hit> &hits);
        void convert_after_refraction(std::vector<Photon_Hit> &hits);
        void plot_colour_with_intensity(int i, int j, FrameBuffer *fb, float I, Colour c);
        void intersect_object(Ray r, float &smallest_t, Vector3 &normal, Vertex &intersection, Properties &property, std::vector<Rectangle> rects,std::vector<Sphere> spheres,std::vector<PolyMesh*> meshes);

        float hit_triangle_moller_trumbore(Triangle triangle, Ray r);
        float hit_rectangle_plane_algo(Rectangle rectangle, Ray r);
        float hit_triangle_plane_algo(Triangle triangle, Ray r);

        double dot(const Vector3 &u, const Vector3 &v);
        double dot(const Vector3 &u, const Vertex &v);

        Vertex to_ver(Vector3 v);
        Vertex vector_plus_vertex(Vector3 v ,Vertex u);


        Vector3 cross(const Vector3 &u, const Vector3 &v);
        Vector3 get_vector(Vertex a, Vertex b);
        
        std::vector<float> hit_sphere(Sphere s, Ray& r);
    };

#endif