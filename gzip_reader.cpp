#pragma once

#include <iostream>
#include "zlib.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <fstream>
#include <dirent.h>
#include <cctype>

using namespace std;

const static int KILOB = 4;

class GzipReader
{
  public:
    GzipReader(string tempDate)
    {
        decompressTryAll(tempDate);
    }

    void decompressTryAll(string tempDate)
    {

        DIR *dir;
        struct dirent *ent;
        string fileName;
        if ((dir = opendir(tempDate.c_str())) != NULL)
        {
            /* print all the files and directories within directory */
            while ((ent = readdir(dir)) != NULL)
            {
                fileName = ent->d_name;

                if (get_ext(fileName) == "GZ" || get_ext(fileName) == "gz" || get_ext(fileName) == "Gz" || get_ext(fileName) == "gZ")
                {
                    cout << "FIND " << fileName << endl;

                    string in_f = tempDate + fileName;
                    string out_f = in_f.substr(0, in_f.size() - 3);

                    cout << "READY: " << out_f << endl;

                    decompress(in_f, out_f);
                    remove(in_f.c_str());
                }
            }
            closedir(dir);
        }
    }

    string get_ext(const string &st)
    {
        size_t pos = st.rfind('.');
        if (pos <= 0)
            return "";
        return st.substr(pos + 1, string::npos);
    }

    void decompress(string inFile, string outFile)
    {
        char cb[KILOB];

        FILE *outfile = fopen(outFile.c_str(), "wb");
        gzFile gfile = gzopen(inFile.c_str(), "rb");
        int len = 0;

        do
        {
            len = gzread(gfile, cb, KILOB);
            if (len < KILOB)
                cb[len] = 0;
            if (len != 0)
                fwrite(cb, KILOB, 1, outfile);
        } while (len == KILOB);
        fclose(outfile);
        gzclose(gfile);
    }
};
