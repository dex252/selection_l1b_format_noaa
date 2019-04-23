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

class Ambx
{
public:
    string temp;
    string newData;

    Ambx()
    {
        //BLOCK_SIZE = 3072 bytes
    }

    Ambx(string tempPath, string dataPath)
    {
        temp = tempPath;
        newData = dataPath;
    }

    bool AmbxParser(Track track, string in_f)
    {
        int16_t i2;
        int32_t i4;
        char c[buf_s];
        char cc[BUF_SIZE];
        int AllCur = 0;
        int count = 0;
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

        in.seekg(28, ios_base::cur);
        // 132 - > 133-134 bytes - count of lines
        in.read((char *)&i2, 2);
        cout << "Count of scan: " << htobe16(i2) << endl;
        ama_h_scnlin = htobe16(i2);
        //массив из 0 и 1 для пометки строк требующих записи или нет; отсчет строк в файле начинается с 1, но в массиве с 0
        bool *lineWrite = new bool[ama_h_scnlin];

        for (size_t k = 0; k < ama_h_scnlin; k++)
        {
            lineWrite[k] = false;
        }

        in.read((char *)&i2, 2);
        cout << "Count of Calibrated, Earth Located Scan Lines: " << htobe16(i2) << endl;
        //seek to end of header 136 -> 3072
        in.seekg(2936, ios_base::cur);

        AllCur = 3072;
        //  cout << "ALL CUR = " << AllCur << endl;

        //scan lines
        for (int i = 0; i < ama_h_scnlin; i++)
        //for (int i = 0; i < 2; i++)
        {
            int CurrentCur = -1;
            int line, year, day, time;
            double lat, lon;
            bool timePass = false;
            bool PASS = false;

            in.read((char *)&i2, 2);

        //    cout << "-------------Line #: " << htobe16(i2) << "  i = " << i << endl;
            //номер строки
            line = htobe16(i2) - 1;

            in.read((char *)&i2, 2);
         //   cout << "Year: " << htobe16(i2) << endl;
            year = htobe16(i2);

            in.read((char *)&i2, 2);
         //   cout << "Day: " << htobe16(i2) << endl;
            day = htobe16(i2);

            in.seekg(2, ios_base::cur);
            in.read((char *)&i4, 4);
         //   cout << "UTC time: " << TimeConventer(htobe32(i4)) << endl;
            time = GetTime(htobe32(i4));

         //   cout << "Year = " << year << "   Day = " << day << "    time = " << time << endl;

            //scan lines 12 - > 740
            in.seekg(740, ios_base::cur);

            AllCur += 12;
            AllCur += 740;

            //     cout << "AllCur = " << AllCur << endl;

            CurrentCur += 740;
            //     cout << "CurrentCur = " << CurrentCur << endl;

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
                                // cout << "Time enter in LAT = " << point.start.lat + 5 << "; LON = " << point.start.lon + 5 << endl;
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
                for (size_t j = 2; j < 182; j += 2)
                {
                    in.read((char *)&i4, 4);
                    //    cout << j / 2 << ":"
                    //        << "LAT: " << htobe32s(i4) / pow(10, 4);
                    lat = htobe32s(i4) / pow(10, 4);

                    in.read((char *)&i4, 4);
                    //     cout << "     :"
                    //         << "LON: " << htobe32s(i4) / pow(10, 4) << endl;
                    lon = htobe32s(i4) / pow(10, 4);
                    CurrentCur += 8;
                    AllCur += 8;

                    /* if (i == 610)
                    {
                        cout << j / 2 << ":"
                             << "LAT: " << lat;
                        cout << "     :"
                             << "LON: " << lon << endl;
                    }*/

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
                                         //   cout << "PASS accept" << endl;
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
                  //  cout << "LINE++" << endl;
                    //записываем номер строки, для дальнейшей записи в легкий файл l1b
                    lineWrite[i] = true;
                    count++;
                    PASS = false;
                }
            }
            else
            {
                //если ни один момент времени не совпал, то пролистываем 720 байт координат
                in.seekg(720, ios_base::cur);
                AllCur += 720;
                //cout << "AllCur = " << AllCur << endl;

                CurrentCur += 720;
                //   cout << "CurrentCur = " << CurrentCur << endl;
            }

            //1472 -> 3072 = 1600
            in.seekg(1600, ios_base::cur);
            AllCur += 1600;
            CurrentCur += 1600;
            // cout << "CurrentCur = " << CurrentCur << endl;
            //  cout << "AllCur = " << AllCur << endl;
        }

      //  cout << "FinalAllCur = " << AllCur << endl;
        for (size_t i = 0; i < ama_h_scnlin; i++)
        {
        //    if (lineWrite[i])
         //       cout << i << ":" << lineWrite[i] << endl;
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
            //копирование первых 132 байт
            in.read((char *)&cc, 132);
            wr.write((char *)&cc, 132);
            //подмена кол-ва строк
            i2 = htobe16(count);
            wr.write((char *)&i2, 2);
            in.seekg(2, ios::cur);
            //копирование 2938 байт от 134 -> 3072
            in.read((char *)&cc, 2938);
            wr.write((char *)&cc, 2938);
            //запись необходимых строк
            for (size_t line = 0; line < ama_h_scnlin; line++)
            {
                if (lineWrite[line])
                {
                  //  cout << "LINE = " << line << endl;
                    in.read((char *)&cc, 3072);
                    wr.write((char *)&cc, 3072);
                }
                else
                {
                    in.seekg(3072, ios::cur);
                }
            }

            wr.close();
        }
        delete[] lineWrite;
        in.close();

        return count > 0 ? 1 : 0;
    }

private:
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
        return htobe32(x);
    }
};