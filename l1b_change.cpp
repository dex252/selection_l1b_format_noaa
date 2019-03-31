#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <list>
#include "class_vector.cpp"

const static int BUF_SIZE = 4096;

using std::ios_base;
using namespace std;

int main(int argc, char *argv[])
{
    string home = "/home/slava/";
    string tempPath = home + "Projects/selection_l1b_format_noaa/TEMP/";
    string trackFile;
    FILE *fin = NULL;
    double lat, lon;
    double radius(5.00);
    list<Vector> trackWay;

    int i;

    if (argc != 2)
    {
        cout << "Error.Path not found." << endl;
        return -1;
    }

    string trackName;
    for (i = strlen(argv[1]); (argv[1][i] != '\\') && (argv[1][i] != '/') && i >= 0; --i)
        ;
    while (argv[1][++i] != '\0')
    {
        trackName += argv[1][i];
    }

    cout << trackName << endl;
    trackFile = tempPath + trackName;

    //copy source .trk file in TEMP dir
    std::ifstream inCopy(argv[1], ios_base::in | ios_base::binary);
    std::ofstream outCopy(trackFile, ios_base::out | ios_base::binary);
    char buf[BUF_SIZE];

    do
    {
        inCopy.read(&buf[0], BUF_SIZE);
        outCopy.write(&buf[0], inCopy.gcount());
    } while (inCopy.gcount() > 0);

    inCopy.close();
    outCopy.close();

    //read copy of source .trk
    std::ifstream in(trackFile);
    string line;

    //Skip first line and read temp-file .trk to end
    getline(in, line);
    for (line; getline(in, line);)
    {
        if (line.length() != 0)
        {
            std::istringstream iss(line);
            string skip;
            iss >> skip >> skip >> skip >> lon >> lat;
            Vector vector(lon-radius, lat-radius, lon+radius, lat+radius);
            trackWay.push_back(vector);
        }
    }

    cout << "======================================="<< endl;

    for (Vector n : trackWay)
        cout <<"minLat = " << n.start.lat << " minLon = " << n.start.lon << " maxLat =  "<< n.end.lat << " maxLon = "<< n.end.lon<<"\n";

    in.close();

    return 0;
}
