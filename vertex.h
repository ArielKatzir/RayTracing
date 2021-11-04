#ifndef VERTEX
#define VERTEX


/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2019.
 *
 * Do what you like with this code as long as you retain this comment.
 */
#include "my_vector.h"

class Vertex {
	public:
		float x;
		float y;
		float z;
		float w;

		Vertex() {
			x = 0.0;
			y = 0.0;
			z = 0.0;
			w = 0.0;
		}

		Vertex(float a, float b, float c, float d) {
			x = a;
			y = b;
			z = c;
			w = d;
		}

		Vertex(float a, float b, float c) {
			x = a;
			y = b;
			z = c;
			w = 1.0f;
		}

		Vertex operator-(const Vertex &vertex) const{
            return Vertex(x - vertex.x, y - vertex.y, z - vertex.z);
        }
		
		
		// return vertex values
		float get_x() const { return x; }
		float get_y() const { return y; }
		float get_z() const { return z; }

		

};
#endif