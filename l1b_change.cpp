#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include "class_vector.cpp"

const static int BUF_SIZE = 4096;

using std::ios_base;
using namespace std;

int main(int argc, char *argv[])
{
    string home = "/home/slava/";
    string tempPath = home + "Projects/selection_l1b_format_noaa/TEMP/";

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

    std::ifstream in(argv[1], ios_base::in | ios_base::binary);
    std::ofstream out(tempPath + trackName, ios_base::out | ios_base::binary);
    char buf[BUF_SIZE];

    do
    {

        in.read(&buf[0], BUF_SIZE);

        out.write(&buf[0], in.gcount());

    } while (in.gcount() > 0);

    in.close();
    out.close();

    return 0;
}
