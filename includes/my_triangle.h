#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vertex.h"


#pragma once
class Triangle{

  private:
    Vertex vertex_0_;
    Vertex vertex_1_;
    Vertex vertex_2_;

  public:
    Triangle(const Vertex v0, const Vertex v1, const Vertex v2)
    {
        vertex_0_ = v0;
        vertex_1_ = v1;
        vertex_2_ = v2;
    }
    Triangle()
    {
        vertex_0_ = Vertex(0,0,0);
        vertex_1_ = Vertex(0,0,0);
        vertex_2_ = Vertex(0,0,0);
    }

    Vertex getVertex0() const { return vertex_0_; }
    Vertex getVertex1() const { return vertex_1_; }
    Vertex getVertex2() const { return vertex_2_; }

};

#endif