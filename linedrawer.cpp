/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2019.
 *
 * Do what you like with this code as long as you retain this comment.
 */

/* This is the code you will need to replace for Lab 1.
 *
 * It contains two simple implementations that loop over the longest axis adding the gradient to the position on the other axis at each step.
 * The objective is for you to remove the use of floating point variables and work entirely with integers.
 * You should use Bresenhams Line Algorithm to achieve this.
 */

#include <iostream>
#include "linedrawer.h"
#include <cmath>
// didnt require these functions
int draw_x_line(FrameBuffer *fb, int sx, int sy, int ex, int ey)
{return 0;}

int draw_y_line(FrameBuffer *fb, int sx, int sy, int ex, int ey)
{return 0;}
 
int draw_line(FrameBuffer *fb, int sx, int sy, int ex, int ey){
  
    int dx,x_slope,dy,y_slope;
    float error,e2;

    // Defining slope direction value for both x and y.
    // And the error value
    dx = abs(ex-sx);
    x_slope = sx<ex ? 1 : -1;
    dy = -abs(ey-sy);
    y_slope = sy<ey ? 1 : -1;
    error = dx+dy;

    // while loop
    while(true){

      //plot
      fb->plotPixel(sx, sy, 1.0f, 1.0f, 1.0f);
      
      // if we finished iterating over the line, break
      if (sx == ex && sy == ey) break;

      e2 = 2*error;

      // if the doubled error value is larger than the change in y, we know to icrease the x - value
      if (e2 >= dy){
        error+=dy;
        sx+=x_slope; 
      // increase the y - value
      }else if(e2<=dx){
        error+=dx;
        sy+=y_slope;
      }
    }
}
