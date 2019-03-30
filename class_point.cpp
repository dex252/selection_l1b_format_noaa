#pragma once

#include <string.h>
#include <iostream>

using namespace std;

class Point
{
  private:
    double *x = new double;
    double *y = new double;

  public:

    void set_x(double x_point)
    {
        *x = x_point;
    }

    void set_y(double y_point)
    {
        *y = y_point;
    }

    double get_x()
    {
        return *x;
    }

    double get_y()
    {
        return *y;
    }
    
    ~Point()
    {
        delete x;
        delete y;
    }
};