#include <iostream>

#include "TDMatrix.h"


int main()
{
    std::string strPath;
    std::cout << "Enter the directory path: " << std::endl;
    //std::cin >> strPath;
    strPath = "C:\\testdir";

    auto matrix = std::make_unique<TDMatrix>(strPath);

    system("pause");
    return 0;
}
