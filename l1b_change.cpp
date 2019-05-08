const int BUF_SIZE = 16384;//размер буфера для копирования трека и чтения длинных записей в файлах
const int buf_s = 64;//размер буфера для чтения коротких записей в файлах

const int msInHour = 3600000;//миллисекунд в часе
const int msInMin = 60000;//миллисекунд в минуте
const int msInSec = 1000; //миллисекунд в секунде

const int deviationTime = 5400000;//отклонение по времени между точками трека и строками файла (+- полтора часа в ms)

const int headerFiller = 84;//отступ в header от начала файла до даты

const int headerStartYear = 2;//год начала сканированя
const int headerStartDay = 2;//день начала сканирования
const int headerStartUtcTime = 4;//время начала сканирования в UTC
const int headerTimeFiller = 4;//пропуск м/у датой начала и датой конца сканирования
const int headerEndYear = 2;//год конца сканирования
const int headerEndDay = 2;//день конца сканирования
const int headerEndUtcTime = 4;//время конца сканирования в UTC

const int countFillerAmax = 40;//отступ от конца даты сканирования до количества строк в файле amax
const int countFillerAmbxMhsx = 28;//отступ от конца даты сканирования до количества строк в файле ambx/mhsx
const int countFillerHirx = 24;//отступ от конца даты сканирования до количества строк в файле hirx

const int countOfScan = 2;//число строк в файле
const int countOfCalibrated = 2;//число откалиброванных строк в файле

const int fillerToLinesAmax = 2412;//отступ от header до блока строк в amax
const int fillerToLinesAmbxMhsx = 2936;//отступ от header до блока строк в ambx/mhsx
const int fillerToLinesHirx = 4476;//отступ от header до блока строк в hirx

const int numCount = 2;//номер строки в файле
const int yearOfCount = 2;//год сканирования строки
const int dayOfCount = 2;//день сканирования строки (от начала года)
const int deltaTimeOfCount = 2;//satellite clock drift in ms
const int utcTimeOfCount = 4;//время сканирования строки

const int fillerToCoordinatesAmax = 640;//отступ От даты и времени строки До набора координат (широты и долготы) в amax
const int fillerToCoordinatesAmbxMhsx = 740;//отступ От даты и времени строки До набора координат (широты и долготы) в ambx/mhsx
const int fillerToCoordinatesHirx = 988;//отступ От даты и времени строки До набора координат (широты и долготы) в hirx

const int latitude = 4;//широта
const int longitude = 4;//долгота

const int countOfCoordinatesAmax = 62;//количество координат в файле amax (считается: ([число]-2)/2 , т.е. 30)
const int countOfCoordinatesAmbxMhsx = 182;//количество координат в файле ambx/mhsx (считается: ([число]-2)/2 , т.е. 90)
const int countOfCoordinatesHirx = 114;//количество координат в файле hirx (считается: ([число]-2)/2 , т.е. 56)

const int timePassFillerAmax = 240;//пропуск блока координат в amax(если ни одна из точек трека не входит во временной промежуток строки)
const int timePassFillerAmbxMhsx = 720;//пропуск блока координат в ambx/mhsx(если ни одна из точек трека не входит во временной промежуток строки)
const int timePassFillerHirx = 448;//пропуск блока координат в hirx(если ни одна из точек трека не входит во временной промежуток строки)

const int fillerToNextBlockInAmax = 1668;//пропуск данных От конца блока координат строки До следующей строки в amax
const int fillerToNextBlockInAmbxMhsx = 1600;//пропуск данных От конца блока координат строки До следующей строки в ambx/mhsx
const int fillerToNextBlockInHirx = 3160;//пропуск данных От конца блока координат строки До следующей строки в hirx

const int blockSizeAmax = 2560;//размер блока строки/header в amax
const int blockSizeAmbxMhsx = 3072;//размер блока строки/header в ambx/mhsx
const int blockSizeHirx = 4608;//размер блока строки/header в hirx

const int copyBytesAmaxHeader = 144;//копирование первых байт в amax
const int copyBytesAmbxMhsxHeader = 132;//копирование первых байт в ambx/mhsx
const int copyBytesHirxHeader = 128;//копирование первых байт в hirx

const int copyBytesAmaxBlock = 2414;//копирование оставшейся части
const int copyBytesAmbxMhsxBlock = 2938;//копирование оставшейся части
const int copyBytesHirxBlock = 4478;//копирование оставшейся части

#include <iostream>
#include "class_track.cpp"
#include "class_parsing.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    string home = "/home/slava/irus17/";
    string tempPath = home + "selection_l1b_format_noaa/TEMP/";
    string newDataPath = home + "full_System_new/data/";
   
    clock_t start, end;
    start = clock();

    double radius(5.00);

    if (argc != 2)
    {
        cout << "Error.Path not found." << endl;
        return -1;
    }

    Track track(argv, radius, tempPath);

    Parsing parsing(track, tempPath, newDataPath);

    end = clock();
    cout << endl << "Time " << (((double)end - start) / ((double)CLOCKS_PER_SEC)) << endl;

    return 0;
}
