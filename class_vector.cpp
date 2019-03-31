#pragma once

#include <string.h>
#include <iostream>

#include "class_point.cpp"

using namespace std;

class Vector
{
  public:
    Point start;
    Point end;

    Vector(double lon1, double lat1, double lon2, double lat2)
    {
        start.lon = lon1;
        start.lat = lat1;

        end.lon = lon2;
        end.lat = lat2;
    }
};