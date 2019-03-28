#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    cout << argc << endl;
    if (argc != 2)
    {
        cout << "Error.Path not found." << endl;
        return -1;
    }
    else
    {
    }

    for (int i = 0; i < argc; i++)
    {
        // Выводим список аргументов в цикле
        cout << "Argument " << i << " : " << argv[i] << endl;
    }
    return 0;
}
