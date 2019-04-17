#pragma once

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include "gzip_reader.cpp"
#include "class_track.cpp"
#include <dirent.h>
#include "copy_data.cpp"

using namespace std;

class AtovsData
{
private:
  string atovsPath;
  string tempDate;

public:
  AtovsData(Track track, string iAtovsPath, string temp)
  {
    atovsPath = iAtovsPath;
    tempDate = temp + "data/";

    //CopyData copyData(track, iAtovsPath, temp);    //problems checkout
    GzipReader gzipReader(tempDate);
  }
};