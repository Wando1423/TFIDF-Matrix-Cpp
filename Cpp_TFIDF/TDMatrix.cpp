#include "TDMatrix.h"
#include <vector>
#include <fstream>
#include <sstream>

TDMatrix::TDMatrix(const std::wstring& strPath)
{
    for (const auto& it : filesystem::directory_iterator(strPath))
    {
        if (!is_empty(it))
            this->AddToMatrix(it);
    }
}

void TDMatrix::AddToMatrix(const filesystem::directory_entry& fsDirectoryEntry)
{
    std::wstring   strFileName(fsDirectoryEntry.path().filename().wstring());
    std::wifstream file       (fsDirectoryEntry.path().wstring());

    

    /* Double bracket so its not defined as a function */
    std::wistringstream issFileContent(std::wstring(std::istreambuf_iterator<wchar_t>(file), (std::istreambuf_iterator<wchar_t>())));

    std::vector<std::pair<std::wstring, int>> vecWorldEntries;

    std::wstring strTmp;
    while (issFileContent >> strTmp)
    {
        const auto it = std::find_if(vecWorldEntries.begin(), vecWorldEntries.end(),
            [strTmp](const std::pair<std::wstring, int>& element) { return element.first == strTmp; });

        if (it == vecWorldEntries.end())
            vecWorldEntries.emplace_back(std::make_pair(strTmp, 1));
        else
            ++it->second;
    }
}
