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

PolyMesh::PolyMesh(char *file, Properties p)
{
  Transform *transform = new Transform();

  this->do_construct(file, transform, p);
}

PolyMesh::PolyMesh(char *file, Transform *transform, Properties p)
{
  this->do_construct(file, transform, p);
}

void PolyMesh::do_construct(char *file, Transform *transform, Properties p)
{
  ifstream myfile;
  string line;

  property = p;

  int count;
  ifstream meshfile(file);

  cerr << "Opening meshfile: " << file << endl;
  
  if (!meshfile.is_open())
  {
    cerr << "Problem reading meshfile (not found)." << endl;
    meshfile.close();
    exit(-1);
  }

  try {
    getline(meshfile, line);
  } catch(ifstream::failure e)
  {
    cerr << "Problem reading meshfile (getline failed)." << endl;
  }

  if (line.compare("kcply") != 0)
  {
    cerr << "Problem reading meshfile (not kcply). [" << line << "]" << endl;
    meshfile.close();
    exit(-1);
  }

  try {
    getline(meshfile, line);
  } catch(ifstream::failure e)
  {
    cerr << "Problem reading meshfile (getline failed)." << endl;
    exit(-1);
  }

  istringstream vertex_iss(line);
  string vertex_element;
  string vertex_label;

  vertex_iss >> vertex_element >> vertex_label >> vertex_count;

  if ((vertex_element.compare("element") != 0)||(vertex_label.compare("vertex") != 0))
  {
    cerr << "Problem reading meshfile (element vertex)."<< endl;
    meshfile.close();
    exit(-1);
  }
  
  try {
    getline(meshfile, line);
  } catch(ifstream::failure e)
  {
    cerr << "Problem reading meshfile (getline failed)." << endl;
    exit(-1);
  }

  istringstream triangle_iss(line);
  string triangle_element;
  string triangle_label;

  triangle_iss >> triangle_element >> triangle_label >> triangle_count;

  if ((triangle_element.compare("element") != 0)||(triangle_label.compare("face") != 0))
  {
    cerr << "Problem reading meshfile (element triangle)."<< endl;
    meshfile.close();
    exit(-1);
  }

  vertex = new Vertex[vertex_count];
  triangle = new TriangleIndex[triangle_count];

  int i;

  for (i = 0; i < vertex_count; i += 1)
  {
    try {
      getline(meshfile, line);
    } catch(ifstream::failure e)
    {
      cerr << "Problem reading meshfile (getline failed)." << endl;
      exit(-1);
    }

    vertex_iss.clear();
    vertex_iss.str(line);

    vertex_iss >> vertex[i].x >> vertex[i].y >>vertex[i].z;

    vertex[i].w = 1.0f;

    transform->apply(vertex[i]);
  }

  for (i = 0; i < triangle_count; i += 1)
  {
    try {
      getline(meshfile, line);
    } catch(ifstream::failure e)
    {
      cerr << "Problem reading meshfile (getline failed)." << endl;
      exit(-1);
    }

    triangle_iss.clear();
    triangle_iss.str(line);
    
    triangle_iss >> count >> triangle[i][0] >> triangle[i][1] >> triangle[i][2];
    // triangle[i][0] -= 1;
    // triangle[i][1] -= 1;
    // triangle[i][2] -= 1;

    if (count != 3)
    {
      cerr << "Problem reading meshfile (non-triangle present)." << endl;
      exit(-1);
    }
  }

  
  meshfile.close();
  cerr << "Meshfile read." << endl;
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


