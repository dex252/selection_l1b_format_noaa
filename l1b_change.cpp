#include <iostream>
#include "class_track.cpp"
#include "atovs_data.cpp"
#include "class_amax.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    string home = "/home/slava/";
    string tempPath = home + "Projects/selection_l1b_format_noaa/TEMP/";
    string iAtovsPath="/satellite/data/ATOVS/";
    //fullPathToAtovs = "https://irods4.satellite.dvo.ru:1247/irods4.satellite.dvo.ru/services/StorageManagementIRods?res=irods_storage#/home/slava/Projects/selection_l1b_format_noaa/TEMP/atovs";

    //string iAtovsPath = "https://irods4.satellite.dvo.ru:1247/irods4.satellite.dvo.ru/services/StorageManagementIRods?res=irods_storage#/satellite/data/ATOVS/";

    double radius(5.00);

    int16_t i;

    if (argc != 2)
    {
        cout << "Error.Path not found." << endl;
        return -1;
    }

    Track track(argv, radius, tempPath);
    AtovsData atovsData(track, iAtovsPath, tempPath);
    //system("./scripts/endian");
    //string amaxFile = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/NSS.AMAX.NK.D14118.S1010.E1204.B8297576.GC";
    //AMAX amax(amaxFile);

    return 0;
}
