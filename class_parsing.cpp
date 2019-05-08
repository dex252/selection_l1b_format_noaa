#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>
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
#include <time.h>

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

        Amax amax1(tempPath, newDataPath);
        Ambx ambx2(tempPath, newDataPath);
        Hirx hirx3(tempPath, newDataPath);
        Mhsx mhsx4(tempPath, newDataPath);

        amax = amax1;
        ambx = ambx2;
        hirx = hirx3;
        mhsx = mhsx4;

        RunFiles(track, tempPath);
        DeleteDataFiles();
    }

private:
    void RunFiles(Track track, string logs)
    {
        string logfile = logs + "logs.txt";
        std::FILE *file = fopen(logfile.c_str(), "w");
        fclose(file);
        std::ofstream logout(logfile.c_str(), ios::in);
        logout << "START..." << endl;
        //1 - write, 0 - clear file
        bool err_log = false;

        DIR *dir;
        struct dirent *ent;
        string in_f;
        int i = 0;

        clock_t start, end;

        if ((dir = opendir(temp.c_str())) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                in_f = ent->d_name;
                i++;
                start = clock();
                switch (TypeFinder(in_f))
                {
                case 'A':
                    cout << "AMAX" << endl;
                    err_log = amax.AmaxParser(track, temp + in_f);
                    end = clock();
                    logout << (((double)end - start) / ((double)CLOCKS_PER_SEC)) << " ms.    File  " << in_f << "  complite with code " << err_log << endl;
                    cout << endl
                         << i << "  :  " << (((double)end - start) / ((double)CLOCKS_PER_SEC)) << " ms" << endl;
                    break;
                case 'B':
                    cout << "AMBX" << endl;
                    err_log = ambx.AmbxParser(track, temp + in_f);
                    end = clock();
                    logout << (((double)end - start) / ((double)CLOCKS_PER_SEC)) << " ms.    File  " << in_f << "  complite with code " << err_log << endl;
                    cout << endl
                         << i << "  :  " << (((double)end - start) / ((double)CLOCKS_PER_SEC)) << " ms";
                    break;
                case 'H':
                    cout << "HIRX" << endl;
                    err_log = hirx.HirxParser(track, temp + in_f);
                    end = clock();
                    logout << (((double)end - start) / ((double)CLOCKS_PER_SEC)) << " ms.    File  " << in_f << "  complite with code " << err_log << endl;
                    cout << endl
                         << i << "  :  " << (((double)end - start) / ((double)CLOCKS_PER_SEC)) << " ms";
                    break;
                case 'M':
                    cout << "MHSX" << endl;
                    err_log = mhsx.MhsxParser(track, temp + in_f);
                    end = clock();
                    logout << (((double)end - start) / ((double)CLOCKS_PER_SEC)) << " ms.    File  " << in_f << "  complite with code " << err_log << endl;
                    cout << endl
                         << i << "  :  " << (((double)end - start) / ((double)CLOCKS_PER_SEC)) << " ms";
                    break;
                default:
                    cout << "ERROR: Unindetermined type." << endl;
                    end = clock();
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

    void DeleteDataFiles()
    {
        string in_f;
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(temp.c_str())) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                in_f = ent->d_name;
                remove((temp + in_f).c_str());
            }
        }
        closedir(dir);
    }
};