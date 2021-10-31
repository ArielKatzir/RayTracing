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
	Vertex position;
	Vector3 direction;
	Vertex origin;

	Ray(){}

	Ray(Vertex p, Vector3 d)
	{
		position = p;
		direction = d;
		origin = Vertex(0,0,0);
	}

	Vertex get_origin(){
		return origin;
	}

	Vertex get_pos(){
		return position;
	} 

	Vector3 get_direction(){
		return direction;
	} 

	Vector3 on_line(float t) {
		float x,y,z;

		x = position.x + direction.x*t;
		y = position.y + direction.y*t;
		z = position.z + direction.z*t;

        return Vector3(x,y,z);
    }

	float get_t(Vertex v){
		return (v.x - position.x)/direction.x;
	}
};
