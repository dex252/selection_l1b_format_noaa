#pragma once

const int buf_s = 64;

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
#include "class_ambx.cpp"
#include "class_hirx.cpp"
#include "class_mhsx.cpp"

using namespace std;

class Parsing
{
public:
    string temp;
    Amax amax;
    Ambx ambx;
    Hirx hirx;
    Mhsx mhsx;

    Parsing(Track track, string tempPath, string newDataPath)
    {
        struct stat info;
        if (stat(newDataPath.c_str(), &info) != 0)
        {
            cout << "Cannot access to " << newDataPath << ". Create new dir" << endl;
            mkdir(newDataPath.c_str(), S_IRWXU);
        }

        temp = tempPath + "data/";

        //  string amaxFile = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/NSS.AMAX.NK.D14118.S0643.E0837.B8297374.WI";
        //   string ambxFile = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/NSS.AMBX.NK.D14118.S0643.E0837.B8297374.WI";
        //  string hirxFile = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/NSS.HIRX.NK.D14118.S0643.E0837.B8297374.WI";
        // string mhsxFile = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/NSS.MHSX.NN.D14119.S1701.E1856.B4607273.WI";

        Amax amax1(tempPath, newDataPath);
        Ambx ambx2(tempPath, newDataPath);
        Hirx hirx3(tempPath, newDataPath);
        Mhsx mhsx4(tempPath, newDataPath);

        // amax1.AmaxParser(track, amaxFile);
        // ambx2.AmbxParser(track, ambxFile);
        // hirx3.HirxParser(track, hirxFile);
        // mhsx4.MhsxParser(track, mhsxFile);

        amax = amax1;
        ambx = ambx2;
        hirx = hirx3;
        mhsx = mhsx4;

        RunFiles(track);
    }

private:
    void RunFiles(Track track)
    {
        std::FILE* file = fopen("/home/slava/Projects/selection_l1b_format_noaa/TEMP/logs.txt", "w");
        fclose(file);
        std::ofstream logout("/home/slava/Projects/selection_l1b_format_noaa/TEMP/logs.txt", ios::in);
        logout << "START..." << endl;
        //1 - write, 0 - clear file
        bool err_log = false;

        DIR *dir;
        struct dirent *ent;
        string in_f;
        int i = 0;
        if ((dir = opendir(temp.c_str())) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                in_f = ent->d_name;
                i++;
                switch (TypeFinder(in_f))
                {
                case 'A':
                    cout << "AMAX" << endl;
                    err_log = amax.AmaxParser(track, temp + in_f);
                    logout << "File " << in_f << "complite with code " << err_log << endl;
                    break;
                case 'B':
                    cout << "AMBX" << endl;
                    err_log = ambx.AmbxParser(track, temp + in_f);
                    logout << "File " << in_f << "complite with code " << err_log << endl;
                    break;
                case 'H':
                    cout << "HIRX" << endl;
                    err_log = hirx.HirxParser(track, temp + in_f);
                    logout << "File " << in_f << "complite with code " << err_log << endl;
                    break;
                case 'M':
                    cout << "MHSX" << endl;
                    err_log = mhsx.MhsxParser(track, temp + in_f);
                    logout << "File " << in_f << "complite with code " << err_log << endl;
                    break;
                default:
                    cout << "ERROR: Unindetermined type." << endl;
                    logout << "File " << in_f << " don't work" << endl;
                    break;
                }
            }
            closedir(dir);
            logout << "...END";
            logout.close();
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