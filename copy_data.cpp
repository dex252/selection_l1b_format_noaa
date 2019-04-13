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

//#include <sys/file.h>   //  <- для копирования файлов

using namespace std;

class CopyData
{
private:
    string atovsPath;
    string tempDate;
    struct stat info;

public:
    CopyData(Track track, string iAtovsPath, string temp)
    {
        atovsPath = iAtovsPath;
        tempDate = temp + "data/";

        //cout << tempDate.c_str() << endl;

        if (stat(tempDate.c_str(), &info) != 0)
        {
            cout << "Cannot access " << tempDate << ". Create new dir" << endl;
            mkdir(tempDate.c_str(), S_IRWXU);
        }

        // downloadL1B(track);
        writeDataTemp(track, temp);
    }

    void writeDataTemp(Track track, string temp)
    {
        string dateInTrack("first date");

        ofstream foutDate;
        ofstream foutTime;

        foutDate.open(temp + "Dates.txt");
        foutTime.open(temp + "Times.txt");

        for (Vector n : track.trackWay)
        {
            if (n.yymmdd != dateInTrack)
            {
                dateInTrack = n.yymmdd;
                foutDate << dateInTrack << endl;
            }
        }

        foutTime << track.startDate << " " << std::to_string(track.startMillisec) << endl;
        foutTime << track.endDate << " " << std::to_string(track.endMillisec) << endl;

        foutDate.close();
        foutTime.close();

        // system("./scripts/cp.sh");
    }

    void downloadL1B(Track trackFile)
    {
        list<string> date;
        string dateInTrack("first date");

        for (Vector n : trackFile.trackWay)
        {
            if (n.yymmdd != dateInTrack)
            {
                dateInTrack = n.yymmdd;
                date.push_back(dateInTrack);
            }
        }

        for (string fullPathToAtovs : date)
        {
            fullPathToAtovs = atovsPath + fullPathToAtovs;

            cout << "ALL ==  " << fullPathToAtovs << endl;
            //Credentials get-запрос
            //fullPathToAtovs = "https://irods4.satellite.dvo.ru:1247/irods4.satellite.dvo.ru/services/StorageManagementIRods?res=irods_storage#/home/slava/Projects/selection_l1b_format_noaa/TEMP/atovs";
            //  ils ${IATOVSPATH}/${BEGIN_DATE_DIR} >${FILELIST}
            //    string iAtovsPath="/satellite/data/ATOVS/";
            //string iAtovsPath = "https://irods4.satellite.dvo.ru:1247/irods4.satellite.dvo.ru/services/StorageManagementIRods?res=irods_storage#/satellite/data/ATOVS/";

            copyAllFiles(fullPathToAtovs, tempDate);
            // system("./cp.sh");
        }
    }

    int copyAllFiles(std::string sorc, std::string dest)
    {

        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(sorc.c_str())) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            { /* print all the files and directories within directory */
                if (ent->d_name != std::string("."))
                { //REMOVE PWD
                    if (ent->d_name != std::string(".."))
                    { //REMOVE PARENT DIR

                        std::string copy_sorc = sorc + "/" + ent->d_name;
                        std::string copy_dest = dest + "/" + ent->d_name;

                        std::cout << "cp " << copy_sorc << " -> " << copy_dest << std::endl;

                        copyFile(copy_sorc.c_str(), copy_dest.c_str());
                    }
                }
            }

            std::cout << std::endl;

            closedir(dir);
        }
        else
        {
            /* could not open directory */
            perror("");
            return EXIT_FAILURE;
        }

        return -3;
    }

    void copyFile(const char *fileNameFrom, const char *fileNameTo)
    {

        char buff[BUFSIZ];
        FILE *in, *out;
        size_t n;

        in = fopen(fileNameFrom, "rb");
        out = fopen(fileNameTo, "wb");
        while ((n = fread(buff, 1, BUFSIZ, in)) != 0)
        {
            fwrite(buff, 1, n, out);
        }
    }

    int dir(std::string path)
    {

        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(path.c_str())) != NULL)
        {

            while ((ent = readdir(dir)) != NULL)
            { /* print all the files and directories within directory */
                if (ent->d_name != std::string("."))
                { //REMOVE PWD
                    if (ent->d_name != std::string(".."))
                    { //REMOVE PARENT DIR

                        std::cout << path << "" << ent->d_name << std::endl;
                    }
                }
            }

            std::cout << std::endl;

            closedir(dir);
        }
        else
        {
            /* could not open directory */
            perror("");
            return EXIT_FAILURE;
        }

        return -2;
    }
};