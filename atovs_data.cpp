#pragma once

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include "gzip_reader.cpp"
#include "class_track.cpp"
#include <dirent.h>

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

    downloadL1B(track);
  }

  void downloadL1B(Track track)
  {
    list<string> date;
    string dateInTrack("first date");

    for (Vector n : track.trackWay)
    {
      if (n.yymmdd != dateInTrack)
      {
        dateInTrack = n.yymmdd;
        date.push_back(dateInTrack);
      }
    }

    for (string fullPathToAtovs : date)
    {
      fullPathToAtovs = atovsPath + fullPathToAtovs;
      cout << "ALL ==  " << fullPathToAtovs << endl;
     // fullPathToAtovs = "https://irods4.satellite.dvo.ru:1247/irods4.satellite.dvo.ru/services/StorageManagementIRods?res=irods_storage#/home/slava/Projects/selection_l1b_format_noaa/TEMP/atovs";
    // https://beda.botik.ru:8080/beda.botik.ru/services/StorageManagementIRods?res=irods_storage#
      //ils ${IATOVSPATH}/${BEGIN_DATE_DIR} >${FILELIST}   str.c_str();
      copyAllFiles(fullPathToAtovs, tempDate);
    }
  }

  int copyAllFiles(std::string sorc, std::string dest)
  {

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(sorc.c_str())) != NULL)
    {
      while ((ent = readdir(dir)) != NULL)
      { /* print all the files and directories within directory */
        if (ent->d_name != std::string("."))
        { //REMOVE PWD
          if (ent->d_name != std::string(".."))
          { //REMOVE PARENT DIR

            std::string copy_sorc = sorc + "/" + ent->d_name;
            std::string copy_dest = dest + "/" + ent->d_name;

            std::cout << "cp " << copy_sorc << " -> " << copy_dest << std::endl;

            copyFile(copy_sorc.c_str(), copy_dest.c_str());
          }
        }
      }

      std::cout << std::endl;

      closedir(dir);
    }
    else
    {
      /* could not open directory */
      perror("");
      return EXIT_FAILURE;
    }

    return -3;
  }

  void copyFile(const char *fileNameFrom, const char *fileNameTo)
  {

    char buff[BUFSIZ];
    FILE *in, *out;
    size_t n;

    in = fopen(fileNameFrom, "rb");
    out = fopen(fileNameTo, "wb");
    while ((n = fread(buff, 1, BUFSIZ, in)) != 0)
    {
      fwrite(buff, 1, n, out);
    }
  }

  int dir(std::string path)
  {

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL)
    {

      while ((ent = readdir(dir)) != NULL)
      { /* print all the files and directories within directory */
        if (ent->d_name != std::string("."))
        { //REMOVE PWD
          if (ent->d_name != std::string(".."))
          { //REMOVE PARENT DIR

            std::cout << path << "/" << ent->d_name << std::endl;
          }
        }
      }

      std::cout << std::endl;

      closedir(dir);
    }
    else
    {
      /* could not open directory */
      perror("");
      return EXIT_FAILURE;
    }

    return -2;
  }
  /* void checkDate(Track track)
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
  } */

  void decompressTry(string input, string output)
  {
    GzipReader GzipReader;

    /* string input = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/NSS.AMAX.NK.D11172.S2050.E2244.B6813941.GC.gz";
        string output = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/AMAX.GC"; */

    GzipReader.decompress(input, output);
  }
};