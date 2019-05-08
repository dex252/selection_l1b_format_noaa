#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include "class_track.cpp"
#include "class_vector.cpp"
#include <dirent.h>
#include <immintrin.h>
#include <endian.h>
#include <cmath>

using namespace std;

class Hirx
{
public:
    string temp;
    string newData;

    Hirx()
    {
        //BLOCK_SIZE = 4608 bytes
    }

    Hirx(string tempPath, string dataPath)
    {
        temp = tempPath;
        newData = dataPath;
    }

    bool HirxParser(Track track, string in_f)
    {
        int16_t i2;
        int32_t i4;
        char c[buf_s];
        char cc[BUF_SIZE];
        int count = 0;
        //HEADER
        int ama_h_scnlin;

        ifstream in(in_f.c_str(), ios::binary);

        cout << "===================    " << in_f << "   ========================" << endl;
        //START DATA
        in.seekg(headerFiller, ios_base::cur);
        in.read((char *)&i2, headerStartYear);
        cout << "Start Year: " << htobe16(i2) << endl;

        in.read((char *)&i2, headerStartDay);
        cout << "Start Day: " << htobe16(i2) << endl;

        in.read((char *)&i4, headerStartUtcTime);
        cout << "Start UTC time: " << TimeConventer(htobe32(i4)) << endl;
        //END DATA
        in.seekg(headerTimeFiller, ios_base::cur);

        in.read((char *)&i2, headerEndYear);
        cout << "End Year: " << htobe16(i2) << endl;

        in.read((char *)&i2, headerEndDay);
        cout << "End Day: " << htobe16(i2) << endl;

        in.read((char *)&i4, headerEndUtcTime);
        cout << "End UTC time: " << TimeConventer(htobe32(i4)) << endl;

        in.seekg(countFillerHirx, ios_base::cur);
        // 104 - > 129-130 bytes - count of lines
        in.read((char *)&i2, countOfScan);
        cout << "Count of scan: " << htobe16(i2) << endl;
        ama_h_scnlin = htobe16(i2);
        //массив из 0 и 1 для пометки строк требующих записи или нет; отсчет строк в файле начинается с 1, но в массиве с 0
        bool *lineWrite = new bool[ama_h_scnlin];

        for (size_t k = 0; k < ama_h_scnlin; k++)
        {
            lineWrite[k] = false;
        }

        in.read((char *)&i2, countOfCalibrated);
        cout << "Count of Calibrated, Earth Located Scan Lines: " << htobe16(i2) << endl;
        //seek to end of header 132 -> 4608
        in.seekg(fillerToLinesHirx, ios_base::cur);

        //scan lines
        for (int i = 0; i < ama_h_scnlin; i++)
        {
            int line, year, day, time;
            double lat, lon;
            bool timePass = false;
            bool PASS = false;

            in.read((char *)&i2, numCount);

            //номер строки
            line = htobe16(i2) - 1;

            in.read((char *)&i2, yearOfCount);
            //  cout << "Year: " << htobe16(i2) << endl;
            year = htobe16(i2);

            in.read((char *)&i2, dayOfCount);
            //   cout << "Day: " << htobe16(i2) << endl;
            day = htobe16(i2);

            in.seekg(deltaTimeOfCount, ios_base::cur);
            in.read((char *)&i4, utcTimeOfCount);
            //  cout << "UTC time: " << TimeConventer(htobe32(i4)) << endl;
            
          // time = GetTime(htobe32(i4));
            time = htobe32(i4);

            //scan lines 12 - > 1000
            in.seekg(fillerToCoordinatesHirx, ios_base::cur);

            //Проверка, входит ли момент времени рассматриваемой строки хотя бы в одну из точек трека
            for (Vector point : track.trackWay)
            {
                if (!timePass)
                {
                    if (point.dateYear == year)
                    {
                        if (point.dateDay == day)
                        {
                            if (time > (point.milliseconds - deviationTime) && time < (point.milliseconds + deviationTime)) //const
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
                //  cout << "Time accept" << endl;
                for (size_t j = 2; j < countOfCoordinatesHirx; j += 2)
                {
                    in.read((char *)&i4, latitude);
                    lat = htobe32s(i4) / pow(10, 4);

                    in.read((char *)&i4, longitude);
                    lon = htobe32s(i4) / pow(10, 4);

                    //рассматриваем входит ли хотя бы одна из точек строки в область одной из точек трека в конкретный момент времени
                    if (!PASS)
                    {
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
                                        if (time > (point.milliseconds - deviationTime) && time < (point.milliseconds + deviationTime))
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
                }
                if (PASS)
                {
                    //записываем номер строки, для дальнейшей записи в легкий файл l1b
                    lineWrite[i] = true;
                    count++;
                    PASS = false;
                }
            }
            else
            {
                //если ни один момент времени не совпал, то пролистываем 720 байт координат
                in.seekg(timePassFillerHirx, ios_base::cur);
            }

            //1448 -> 4608 = 3160
            in.seekg(fillerToNextBlockInHirx, ios_base::cur);
        }

        cout << "Count lines for write: " << count << endl;

        //Проверка и запись файла
        if (count != 0)
        {
            //выделяем иия файла
            std::string inf = in_f;
            std::string fileName = "";
            int i = inf.length() - 1;
            while (inf[i--] != '/')
                ;
            inf.erase(i, 1);
            while (inf[i++] != '\0')
                fileName += inf[i];
            // старт записи
            string wr_f = newData + fileName;
            cout << "Start writing..." << wr_f << endl;
            ofstream wr(wr_f.c_str(), ios::binary);
            //установка курсора в начала файла
            in.seekg(0, ios::beg);
            //копирование первых 128 байт
            in.read((char *)&cc, copyBytesHirxHeader);
            wr.write((char *)&cc, copyBytesHirxHeader);
            //подмена кол-ва строк
            i2 = htobe16(count);
            wr.write((char *)&i2, countOfScan);
            in.seekg(countOfScan, ios::cur);
            //копирование 4478 байт от 130 -> 4608
            in.read((char *)&cc, copyBytesHirxBlock);
            wr.write((char *)&cc, copyBytesHirxBlock);
            //запись необходимых строк
            for (size_t line = 0; line < ama_h_scnlin; line++)
            {
                if (lineWrite[line])
                {
                    in.read((char *)&cc, blockSizeHirx);
                    wr.write((char *)&cc, blockSizeHirx);
                }
                else
                {
                    in.seekg(blockSizeHirx, ios::cur);
                }
            }

            wr.close();
        }
        delete[] lineWrite;
        in.close();

        return count > 0 ? 0 : 1;
    }

private:
    string TimeConventer(int32_t utcTime)
    {
        int hh = utcTime / msInHour;
        int mm = (utcTime % msInHour) / msInMin;
        int ss = ((utcTime % msInHour) % msInMin) / msInSec;

        string time = to_string(hh) + ":" + to_string(mm) + ":" + to_string(ss);

        return time;
    }

    //get time in ms
    int GetTime(int32_t utcTime)
    {
        int milihh = utcTime / msInHour;
        int milimm = (utcTime % msInHour) / msInMin;
        int miliss = ((utcTime % msInHour) % msInMin) / msInSec;

        int time = milihh * msInHour + milimm * msInMin + miliss * msInSec;
        return time;
    }

    int32_t htobe32s(int32_t x)
    {
        return htobe32(x);
    }
};