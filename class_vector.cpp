#pragma once

#include <string.h>
#include <iostream>

#include "class_point.cpp"

using namespace std;

class Vector
{
  private:
    Point start;
    Point end;

  public:
    Vector(double x1, double y1, double x2, double y2)
    {
        Vector::set_start(x1, y1);
        Vector::set_end(x2, y2);
    }

    void set_start(double x_point, double y_point)
    {
        start.set_x(x_point);
        start.set_y(y_point);
    }

    void set_end(double x_point, double y_point)
    {
        end.set_x(x_point);
        end.set_y(y_point);
    }

    Point get_start()
    {
        return start;
    }

    Point get_end()
    {
        return end;
    }

    double set_start_x(double x_point)
    {
        start.set_x(x_point);
    }

    double set_start_y(double y_point)
    {
        start.set_y(y_point);
    }

    double set_end_x(double x_point)
    {
        end.set_x(x_point);
    }

    double set_end_y(double y_point)
    {
        end.set_y(y_point);
    }

    double get_start_x()
    {
        return start.get_x();
    }

    double get_start_y()
    {
        return start.get_y();
    }

    double get_end_x()
    {
        return end.get_x();
    }

    double get_end_y()
    {
        return end.get_y();
    }

    ~Vector()
    {
        start.~Point();
        end.~Point();
    }
};