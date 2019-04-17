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
#include <dirent.h>
#include <immintrin.h>
#include <endian.h>
#include <cmath>

const int buf_s = 64;

using namespace std;

class Parsing
{
public:
    string temp;

    Parsing(string tempPath)
    {
        temp = tempPath + "data/";
        //RunFiles(temp);
        string amaxFile = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/NSS.AMAX.NK.D11171.S1811.E1941.B6812425.WI";
        AmaxParser(amaxFile);
    }

private:
    void RunFiles(string tempDate)
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
                    AmaxParser(temp + in_f);
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

    string TimeConventer(int32_t utcTime)
    {
        int hh = utcTime / 3600000;
        int mm = (utcTime % 3600000) / 60000;
        int ss = ((utcTime % 3600000) % 60000) / 1000;

        string time = to_string(hh) + ":" + to_string(mm) + ":" + to_string(ss);

        return time;
    }

    void AmaxParser(string in_f)
    {
        int16_t i2;
        int32_t i4;
        char c[buf_s];

        //HEADER
        int ama_h_scnlin;

        ifstream in(in_f.c_str(), ios::binary);

        // in.read((char *)&c, 3);
        cout << "===================    " << in_f << "   ========================" << endl;
        //START DATA
        in.seekg(84, ios_base::cur);
        in.read((char *)&i2, 2);
        cout << "Start Year: " << htobe16(i2) << endl;

        in.read((char *)&i2, 2);
        cout << "Start Day: " << htobe16(i2) << endl;

        in.read((char *)&i4, 4);
        cout << "Start UTC time: " << TimeConventer(htobe32(i4)) << endl;
        //END DATA
        in.seekg(4, ios_base::cur);

        in.read((char *)&i2, 2);
        cout << "End Year: " << htobe16(i2) << endl;

        in.read((char *)&i2, 2);
        cout << "End Day: " << htobe16(i2) << endl;

        in.read((char *)&i4, 4);
        cout << "End UTC time: " << TimeConventer(htobe32(i4)) << endl;
        //104 befor + 40 after, 145
        in.seekg(40, ios_base::cur);

        in.read((char *)&i2, 2);
        cout << "Count of scan: " << htobe16(i2) << endl;
        ama_h_scnlin = htobe16(i2);

        in.read((char *)&i2, 2);
        cout << "Count of Calibrated, Earth Located Scan Lines: " << htobe16(i2) << endl;
        //seek to end of header 148 -> 2560
        in.seekg(2412, ios_base::cur);

        //scan lines
        // for (int i = 0; i < ama_h_scnlin; i++)
        for (int i = 0; i < 1; i++)
        {
            in.read((char *)&i2, 2);
            cout << "Line #: " << htobe16(i2) << endl;

            //scan lines 2 - > 653
            in.seekg(650, ios_base::cur);

            for (size_t j = 2; j < 62; j += 2)
            {
                in.read((char *)&i4, 4);
                cout << j / 2 << ":"
                     << "LAT: " << htobe32s(i4) / pow(10, 4);

                in.read((char *)&i4, 4);
                cout << "     :"
                     << "LON: " << htobe32s(i4) / pow(10, 4) << endl; // Какого хрена?
            }

            //time???
            //892 -> 2560

            in.seekg(2560 - 892, ios_base::cur);
        }

        in.close();
    }

    int32_t htobe32s(int32_t x)
    {
        union {
            int32_t u32;
            int8_t v[4];
        } ret;
        ret.v[0] = (uint8_t)(x >> 24);
        ret.v[1] = (uint8_t)(x >> 16);
        ret.v[2] = (uint8_t)(x >> 8);
        ret.v[3] = (uint8_t)x;
        return ret.u32;
    }
};