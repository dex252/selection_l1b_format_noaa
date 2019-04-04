#pragma once

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include "gzip_reader.cpp"
#include "class_track.cpp"

using namespace std;

class AtovsData
{
private:
  string atovsPath;
  string tempAtovs;
  string tempDate;
  string date;

public:
  AtovsData(Track track, string iAtovsPath, string temp)
  {
    atovsPath = iAtovsPath;
    tempAtovs = temp + "atovs/";
    tempDate = temp + "date/";

    checkDate(track);
  }

  void downloadL1B()
  {
  }

  void checkDate(Track track)
  {
    //data = track.startDate.substr(0, 4);
    string dateFile = tempDate + "dates";
    cout << "DataTEMPFile: " << dateFile << endl;

    remove(dateFile.c_str());
    std::ofstream out(dateFile.c_str(), std::ios::app);

    for (Vector n : track.trackWay)
    {

      out << n.yymmdd << std::endl;
    }

    out.close();
  }
  void decompressTry(string input, string output)
  {
    GzipReader GzipReader;

    /* string input = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/NSS.AMAX.NK.D11172.S2050.E2244.B6813941.GC.gz";
        string output = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/AMAX.GC"; */

    GzipReader.decompress(input, output);
  }
};