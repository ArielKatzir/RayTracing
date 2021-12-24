/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2019.
 */

#ifndef POLYMESH_H
#define POLYMESH_H

#pragma once

#include "vertex.h"
#include "transform.h"
#include "properties.h"
#include <string>

typedef int TriangleIndex[3];

// Adding utility functions
int get_count(std::string s);
bool in(std::string s1, std::string s2);
Vertex extract_vertex_from_str(std::string s);

class PolyMesh {
public:
	Properties property;
	int vertex_count;
	int triangle_count;
    	Vertex *vertex;
	TriangleIndex *triangle;


	void do_construct(char *file, Transform *transform, Properties p);
	PolyMesh(char *file, Properties p);
	PolyMesh(char *file, Transform *transform, Properties p);
	
	Properties getProperty(){
  		return property;
	}
};


#endif