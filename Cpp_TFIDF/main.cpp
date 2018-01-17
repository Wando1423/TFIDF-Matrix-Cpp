#include <iostream>

#include "TDMatrix.h"



int main()
{
    std::wstring strPath;
    std::wcout << L"Enter the directory path: " << std::endl;
    //std::wcin >> strPath;
    strPath = L"C:\\testdir";

    auto matrix = std::make_unique<TDMatrix>(strPath);
    return 0;
}