#include <iostream>
#include "class_track.cpp"
#include "gzip_reader.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    string home = "/home/slava/";
    string tempPath = home + "Projects/selection_l1b_format_noaa/TEMP/";
    double radius(5.00);

    int i;

    if (argc != 2)
    {
        cout << "Error.Path not found." << endl;
        return -1;
    }

    Track track(argv, radius, tempPath);

    GzipReader GzipReader;

    string input = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/NSS.AMAX.NK.D11172.S2050.E2244.B6813941.GC.gz";
    string output = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/AMAX.GC";

    GzipReader.decompress(input, output);

    return 0;
}
