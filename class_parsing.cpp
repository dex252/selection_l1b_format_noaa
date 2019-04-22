#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include "gzip_reader.cpp"
#include "class_track.cpp"
#include "class_vector.cpp"
#include <dirent.h>
#include <immintrin.h>
#include <endian.h>
#include <cmath>
#include <dirent.h>
#include "class_amax.cpp"

using namespace std;

class Parsing
{
public:
    string temp;

    Parsing(Track track, string tempPath, string newDataPath)
    {
        struct stat info;
        if (stat(newDataPath.c_str(), &info) != 0)
        {
            cout << "Cannot access to " << newDataPath << ". Create new dir" << endl;
            mkdir(newDataPath.c_str(), S_IRWXU);
        }

        temp = tempPath + "data/";

        string amaxFile = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/NSS.AMAX.NK.D14118.S0643.E0837.B8297374.WI";
        //  string amaxFile = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/1.WI";

        Amax amax(tempPath, newDataPath);
        amax.AmaxParser(track, amaxFile);

        //RunFiles(track, temp, amax);
    }

private:
    void RunFiles(Track track, string tempDate, Amax amax)
    {

        DIR *dir;
        struct dirent *ent;
        string in_f;
        int i = 0;
        if ((dir = opendir(tempDate.c_str())) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                in_f = ent->d_name;
                i++;
                switch (TypeFinder(in_f))
                {
                case 'A':
                    cout << "AMAX" << endl;
                    amax.AmaxParser(track, temp + in_f);
                    break;
                case 'B':
                    cout << "AMBX" << endl;
                    // AmbxParser(temp + in_f);
                    break;
                case 'H':
                    cout << "HIRX" << endl;
                    // HirxParser(temp + in_f);
                    break;
                case 'M':
                    cout << "MHSX" << endl;
                    //MhsxParser(temp + in_f);
                    break;
                default:
                    cout << "ERROR: Unindetermined type." << endl;
                    break;
                }
            }
            closedir(dir);
        }
    }

    char TypeFinder(string in_f)
    {
        if (in_f.find("AMAX") != std::string::npos)
        {
            return 'A';
        }
        if (in_f.find("AMBX") != std::string::npos)
        {
            return 'B';
        }
        if (in_f.find("HIRX") != std::string::npos)
        {
            return 'H';
        }
        if (in_f.find("MHSX") != std::string::npos)
        {
            return 'M';
        }
    }
};