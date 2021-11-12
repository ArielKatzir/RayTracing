/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2019.
 *
 * Do what you like with this code as long as you retain this comment.
 */

// Ray is a class to store and maniplulate 3D rays.

#pragma once

#include "vertex.h"
#include "my_vector.h"

class Ray {
public:
	Vector3 direction;
	Vertex origin;

	Ray(){}

	Ray(Vertex o, Vector3 d)
	{
		direction = d;
		origin = o;
	}

	Vertex get_origin(){
		return origin;
	}

	Vector3 get_direction(){
		return direction;
	} 

	Vertex on_line(float t) {
		float x,y,z;

		x = origin.x + direction.x*t;
		y = origin.y + direction.y*t;
		z = origin.z + direction.z*t;

        return Vertex(x,y,z);
    }

	float get_t(Vertex v){
		return (v.x - origin.x)/direction.x;
	}
};
