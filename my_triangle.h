#include "my_vector.h"
#include "vertex.h"




/* Used by intersection algorithms within this file.
 * Determines precision of when a number is treated as a 0. */

class Triangle
{

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

    Vertex getVertex0() const { return vertex_0_; }
    Vertex getVertex1() const { return vertex_1_; }
    Vertex getVertex2() const { return vertex_2_; }

};
