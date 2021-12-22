#include "my_vector.h"
#include "vertex.h"
#include "properties.h"
using namespace std;



#pragma once
class Rectangle 
{
  public:
    Rectangle(){}
    Properties property;
    Vector3 v,u;
    Vertex rect_origin,b,c,d,centre;

    Rectangle(Vertex o, Vector3 vec, Vector3 vec2, Properties prop){
        property = prop;
        
        rect_origin = o;
        v = vec;
        u = vec2;

        rounding_error();

        b = from_origin(v);
        c = from_origin(v+u);
        d = from_origin(u);
        centre = Vertex(o.x+vec.x+vec2.x, o.y+vec.y+vec2.y, o.z+vec.z+vec2.z);


    }

    Properties getProperty()  { return property; }


    Vector3 get_normal(){
        float x = u.y * v.z - u.z * v.y;
        float y = u.z * v.x - u.x * v.z;
        float z = u.x * v.y - u.y * v.x;
        
        
        Vector3 vect = Vector3(x,y,z);
        vect.normalise();
        
        return vect;

    }

    Vertex from_origin(Vector3 vec){
        return Vertex(  rect_origin.x+vec.x,
                        rect_origin.y+vec.y,
                        rect_origin.z+vec.z
                    );
    }
    // max only takes 2 inputs, so this is done weirdly
    float maxx(){
        return max(max(max(rect_origin.x,b.x),c.x),d.x);
    }
    float maxy(){
        return max(max(max(rect_origin.y,b.y),c.y),d.y);
    }
    float maxz(){
        return max(max(max(rect_origin.z,b.z),c.z),d.z);
    }
    float minx(){
        return min(min(min(rect_origin.x,b.x),c.x),d.x);
    }
    float miny(){
        return min(min(min(rect_origin.y,b.y),c.y),d.y);
    }
    float minz(){
        return min(min(min(rect_origin.z,b.z),c.z),d.z);
    }  

    void rounding_error(){
        double const EPSILON = 0.0001;
        if (v.x == 0.0 && u.x==0.0){
            v.x+=EPSILON;
            v.x+=EPSILON;
        } 
        if (v.y == 0.0 && u.y==0.0){
            v.y+=EPSILON;
            v.y+=EPSILON;
        } 
        if (v.z == 0.0 && u.z==0.0){
            v.z+=EPSILON;
            v.z+=EPSILON;
        } 
    }  
};
