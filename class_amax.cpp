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

const int buf_s = 64;

using namespace std;

class AMAX
{
public:
    AMAX(string in_f)
    {
        AmaxParser(in_f);
    }

    void AmaxParser(string in_f)
    {
        int16_t i2;
        int32_t i4;
        char c[buf_s];

        ifstream in(in_f.c_str(), ios::binary);
        
        in.read ((char*)&c, 3);
        cout << "ID: " << c << endl;

        in.seekg(143, ios_base::cur);

        in.read((char*)&i2, 2);
        cout << "Count of scan: " << htobe16(i2) <<endl;

        in.close();
    }
    
};