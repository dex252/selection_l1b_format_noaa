#pragma once

#include <string.h>
#include <iostream>
#include <stdio.h>

#include "class_point.cpp"

using namespace std;

class Vector
{
public:
  Point start;
  Point end;
  string yymmdd;

  int milliseconds;

  Vector(double lon1, double lat1, double lon2, double lat2)
  {
    start.lon = lon1;
    start.lat = lat1;

    end.lon = lon2;
    end.lat = lat2;
  }

  void to_ms_time(string hhmmss)
  {
    int milihh = stoi(hhmmss.substr(0, 2));
    int milimm = stoi(hhmmss.substr(3, 2));
    int miliss = stoi(hhmmss.substr(6, 2));

    milliseconds = milihh * 3600000 + milimm * 60000 + miliss * 1000;
  }
};