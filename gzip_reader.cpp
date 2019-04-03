#pragma once

#include <iostream>
#include "zlib.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

using namespace std;

const static int KILOB = 4;

class GzipReader
{
  public:
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
