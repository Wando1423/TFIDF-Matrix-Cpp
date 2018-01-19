#include <iostream>

#include "TDMatrix.h"


int main()
{
    std::string strPath{};
    std::string strSearchString{};
    std::cout << "Enter the directory path: " << std::endl;
    std::getline(std::cin, strPath);

    auto matrix = std::make_unique<TDMatrix>(strPath);
    matrix->ConvertToTFIDF();
    std::cout << "Enter the string you want to compare. If you wish to end the application, write 'end'" << std::endl;
    while (true)
    {
        std::getline(std::cin, strSearchString);
        if (strSearchString == "end")
            break;

        auto place = 1;
        auto vec = matrix->GetFileSimmRanking(strSearchString);
        std::cout << "Simmilarity ranking: " << std::endl;
        for (const auto& it : vec)
        {
            std::cout << place << ": " << it << std::endl;
            ++place;
        }
        strSearchString = "";
    }
    system("pause");
    return 0;
}
