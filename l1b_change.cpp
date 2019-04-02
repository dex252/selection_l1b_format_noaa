#include <iostream>
#include "class_track.cpp"

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

    return 0;
}
