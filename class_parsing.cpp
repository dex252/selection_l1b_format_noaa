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
#include "class_vector.cpp"
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

    Parsing(Track track, string tempPath)
    {
        temp = tempPath + "data/";
        //RunFiles(track, temp);
        string amaxFile = "/home/slava/Projects/selection_l1b_format_noaa/TEMP/data/NSS.AMAX.NK.D11171.S1811.E1941.B6812425.WI";
        AmaxParser(track, amaxFile);
    }

private:
    void RunFiles(Track track, string tempDate)
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
                    AmaxParser(track, temp + in_f);
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

    void AmaxParser(Track track, string in_f)
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

        //массив из 0 и 1 для пометки строк требующих записи или нет; отсчет строк в файле начинается с 1, но в массиве с 0
        bool *lineWrite = new bool[ama_h_scnlin];

        for (size_t i = 0; i < ama_h_scnlin; i++)
        {
            lineWrite[i] = false;
        }

        in.read((char *)&i2, 2);
        cout << "Count of Calibrated, Earth Located Scan Lines: " << htobe16(i2) << endl;
        //seek to end of header 148 -> 2560
        in.seekg(2412, ios_base::cur);

        //scan lines
        // for (int i = 0; i < ama_h_scnlin; i++)
        for (int i = 0; i < 1; i++)
        {
            int line, year, day, time;
            double lat, lon;
            bool PASS, timePass = false;

            in.read((char *)&i2, 2);

            cout << "-------------Line #: " << htobe16(i2) << endl;
            //номер строки - 1 для пометки в массиве
            line = htobe16(i2) - 1;

            in.read((char *)&i2, 2);
            cout << "Year: " << htobe16(i2) << endl;
            year = htobe16(i2);

            in.read((char *)&i2, 2);
            cout << "Day: " << htobe16(i2) << endl;
            day = htobe16(i2);

            in.seekg(2, ios_base::cur);
            in.read((char *)&i4, 4);
            cout << "UTC time: " << TimeConventer(htobe32(i4)) << endl;
            time = GetTime(htobe32(i4));

            cout << "Year = " << year << "   Dat = " << day << "    time = " << time << endl;

            //scan lines 2 - > 653
            in.seekg(640, ios_base::cur);
            //Проверка, входит ли момент времени рассматриваемой строки хотя бы в одну из точек трека
            for (Vector point : track.trackWay)
            {
                if (!timePass)
                {
                    if (point.dateYear == year)
                    {
                        if (point.dateDay == day)
                        {
                            if (time > (point.milliseconds - 5400000) && time < (point.milliseconds + 5400000))
                            {
                                timePass = true;
                            }
                        }
                    }
                }
            }
            //если момент времени включен в трек, то рассматриваем координаты строки;в противном случае пропускаем байты до следущей строки
            if (timePass)
            {
                timePass = false;

                for (size_t j = 2; j < 62; j += 2)
                {
                    in.read((char *)&i4, 4);
                    cout << j / 2 << ":"
                         << "LAT: " << htobe32s(i4) / pow(10, 4);
                    lat = htobe32s(i4) / pow(10, 4);

                    in.read((char *)&i4, 4);
                    cout << "     :"
                         << "LON: " << htobe32s(i4) / pow(10, 4) << endl;
                    lon = htobe32s(i4) / pow(10, 4);

                    //рассматриваем входит ли хотя бы одна из точек строки в область одной из точек трека в конкретный момент времени

                    for (Vector point : track.trackWay)
                    {
                        //рассматрение прерывается, если найдена хотя бы одна точка
                        if (!PASS)
                        {
                            //отбираем только те точки трека, которые входят в рассматриваемый промежуток времени
                            if (point.dateYear == year)
                            {
                                if (point.dateDay == day)
                                {
                                    if (time > (point.milliseconds - 5400000) && time < (point.milliseconds + 5400000))
                                    {
                                        //если момент времени совпал, то проверяем вхождение в область точки
                                        if (lat > point.start.lat && lat < point.end.lat && lon > point.start.lon && lon < point.end.lon)
                                        {
                                            //PASS сработает лишь единожды
                                            PASS = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (PASS)
                {
                    //записываем номер строки, для дальнейшей записи в легкий файл l1b
                    lineWrite[line] = true;
                    PASS = false;
                }
            }
            else
            {
                //если ни один момент времени не совпал, то пролистываем 240 байт координат
                in.seekg(240, ios_base::cur);
            }

            //892 -> 2560 = 1668
            in.seekg(1668, ios_base::cur);
        }
        //до того, как файл был закрыт - необходимо записать строки в новый легковесный файл: 2 вараинта
        //1: открыть файл по новой и вести запись последовательно
        //2: вести запись параллельно, управляя cur (предпочтительней)         in.seekg(-4, ios_base::cur); - смещение указателя назад на 4 позиции
        //во втором варианте в массиве нет необходимости, достаточно завести счетчик строк
        delete[] lineWrite;
        in.close();
    }

    string TimeConventer(int32_t utcTime)
    {
        int hh = utcTime / 3600000;
        int mm = (utcTime % 3600000) / 60000;
        int ss = ((utcTime % 3600000) % 60000) / 1000;

        string time = to_string(hh) + ":" + to_string(mm) + ":" + to_string(ss);

        return time;
    }

    //get time in ms
    int GetTime(int32_t utcTime)
    {
        int milihh = utcTime / 3600000;
        int milimm = (utcTime % 3600000) / 60000;
        int miliss = ((utcTime % 3600000) % 60000) / 1000;

        int time = milihh * 3600000 + milimm * 60000 + miliss * 1000;
        return time;
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