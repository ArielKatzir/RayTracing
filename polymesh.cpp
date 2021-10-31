/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2019.
 *
 * Do what you like with this code as long as you retain this comment.
 */

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <list>


#include "polymesh.h"

using namespace std;

PolyMesh::PolyMesh(char *file)
{
  Transform *transform = new Transform();

  this->do_construct(file, transform);
}

PolyMesh::PolyMesh(char *file, Transform *transform)
{
  this->do_construct(file, transform);
}

void PolyMesh::do_construct(char *file, Transform *transform)
{
  ifstream myfile;
  string line;

  myfile.open("teapot.ply");
 
  // Getting vertex count and triangle count  
  while (vertex_count==NULL || triangle_count==NULL)
  {
    getline(myfile, line);
    if (in(line, "vertex")) vertex_count = get_count(line);
    if (in(line, "face")) triangle_count = get_count(line);
  }
  
  // allocating memory for the vertex list
  vertex = (Vertex*) malloc(1+vertex_count*sizeof(Vertex));

  // creating counter for while loop
  int v_counter = 0;
  
  // extracting all the vertices 
  while(v_counter <= vertex_count-1){
    getline(myfile, line);
    Vertex v = extract_vertex_from_str(line);
    transform->apply(v);
    vertex[v_counter] = v;
    v_counter++;   
  }

  // freeing memory
  free(vertex);

  // allocating memory for triangle
  triangle = (TriangleIndex*) malloc(triangle_count*sizeof(TriangleIndex));

  // extracting all the triangles
  for (int i = 0; i <= triangle_count; i++){
    getline(myfile, line);
    std::stringstream ss(line);
    int face_num,x,y,z;
    ss >> face_num >> x >> y >> z;
    TriangleIndex t = {x, y, z};

    for (int j=0; j<=2; j++){
      triangle[i][j] = t[j] -1;
    }
  }
  // freeing truangle memory
  free(triangle);
}





// utility function to extract an integer from a string 
int get_count(string s)
{
  stringstream str_strm;
  str_strm << s; //convert the string s into stringstream
  string temp_str;
  int temp_int;
  while (!str_strm.eof())
  {
    str_strm >> temp_str;               //take words into temp_str one by one
    stringstream(temp_str) >> temp_int; 
    temp_str = "";                      
  }
  return temp_int;
}

// utility function to check if a string is in another string
bool in(string s1, string s2)
{
  if (s1.find(s2) != std::string::npos)
  {
    return true;
  }
  return false;
}

// utility function to extract vertex from line
Vertex extract_vertex_from_str(string s)
{
  std::stringstream ss(s);
  float x,y,z;
  ss >> x >> y >> z;
  return Vertex(x, y, z);
}
