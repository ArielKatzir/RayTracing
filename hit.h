/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2019.
 *
 * Do what you like with this code as long as you retain this comment.
 */

// Hit is a class to store and maniplulate information about an intersection
// between a ray and an object.

#pragma once

#include "vertex.h"
#include "my_vector.h"
#include "ray.h"


class Object;

class Hit {
public:
	bool flag;
	float t;
	Object *what;
	Vertex position;
	Vector3 normal;


	Hit(bool _flag, float _t, Object *_what, Vertex _position, Vector3 _normal){
		flag = _flag;
		t = -t;
		what = _what;
		position = _position;
		normal = _normal;
	}

};
