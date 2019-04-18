#pragma once

#include <string.h>
#include <iostream>
#include <stdio.h>
#include "class_point.cpp"
#include <time.h>

using namespace std;

class Vector
{
public:
  Point start;
  Point end;
  string yymmdd;

  int milliseconds;
  int dateDay;
  int dateYear;

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

  void to_date_day(string date)
  {
    int yyyy = stoi(yymmdd.substr(0, 4));
    int mmmm = stoi(yymmdd.substr(5, 2));
    int dddd = stoi(yymmdd.substr(8, 2));

    dateYear = yyyy;
    dateDay = monthInDay(yyyy, mmmm) + dddd;
  }

private:
  int monthInDay(int year, int month)
  {
    int dayArray[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int days = 0;

    for (size_t i = 0; i < month - 1; i++)
    {
      days += dayArray[i];
    }

    if (month > 1)
    {
      days += visokosYear(year);
    }

    return days;
  }

  int visokosYear(int year)
  {
    if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
};