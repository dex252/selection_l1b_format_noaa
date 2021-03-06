#pragma once

#include "class_vector.cpp"
#include <sstream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <list>
#include <algorithm>

using std::ios_base;
using namespace std;

class Track
{
  public:
    list<Vector> trackWay;

    string trackName;
    string trackFile;

    double radius;

    string startDate;
    string endDate;

    int startMillisec;
    int endMillisec;

    Track(char *argv[], double rad, string tempPath)
    {
        int i;

        for (i = strlen(argv[1]); (argv[1][i] != '\\') && (argv[1][i] != '/') && i >= 0; --i)
            ;
        while (argv[1][++i] != '\0')
        {
            trackName += argv[1][i];
        }

        radius = rad;

        cout << "TrackName: " << trackName << endl;

        copyTrackInTemp(argv, tempPath);
        readTrackInTemp();
        readTrackInCash();

        remove(trackFile.c_str());
    }

    void copyTrackInTemp(char *argv[], string tempWay)
    {
        trackFile = tempWay + trackName;

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
    }

    void readTrackInTemp()
    {
        string *skip = new string;
        string yymmdd, hhmmss;
        double *lat = new double;
        double *lon = new double;
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
                iss >> *skip >> yymmdd >> hhmmss >> *lon >> *lat;

                Vector vector(*lon - radius, *lat - radius, *lon + radius, *lat + radius);

                std::replace(yymmdd.begin(), yymmdd.end(), '-', '/');
                vector.yymmdd = yymmdd;

                vector.to_ms_time(hhmmss);
                vector.to_date_day(yymmdd);

                trackWay.push_back(vector);
            }
        }

        in.close();

        delete lat;
        delete lon;
        delete skip;
    }

    void readTrackInCash()
    {
        int num = 0;

        for (Vector n : trackWay)
        {
            if (num == 0)
            {
                startMillisec = n.milliseconds;
                startDate = n.yymmdd;
            }

            if (num == trackWay.size() - 1)
            {
                endMillisec = n.milliseconds;
                endDate = n.yymmdd;
            }

            num++;
        }
    }
};