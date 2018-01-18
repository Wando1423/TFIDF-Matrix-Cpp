#include "TDMatrix.h"
#include <Windows.h>
#include <vector>
#include <fstream>
#include <sstream>

TDMatrix::TDMatrix(const std::string& strPath)
{
    for (const auto& it : filesystem::directory_iterator(strPath))
    {
        if (!is_empty(it))
            this->AddToMatrix(it);
    }
}

int TDMatrix::GetFileIndex(const std::string& strFileName)
{
    const auto it = std::find(this->vecFileList.begin(), this->vecFileList.end(), strFileName);
    if (it == this->vecFileList.end())
        return -1;

    return std::distance(this->vecFileList.begin(), it);
}

void TDMatrix::AddToMatrix(const filesystem::directory_entry& fsDirectoryEntry)
{
    const std::string   strFileName(fsDirectoryEntry.path().filename().string());
    std::ifstream       file       (fsDirectoryEntry.path(), std::ios_base::binary);
    
    /* Double bracket so its not declared as a function */
    std::istringstream issFileContent(std::string(std::istreambuf_iterator<char>(file),
                                                  (std::istreambuf_iterator<char>())));


    std::string strTmp;
    std::vector<std::pair<std::string, int>> vecWorldEntries;
    while (issFileContent >> strTmp)
    {
        /* Convert to lower case */
        std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), ::tolower);

        const auto it = std::find_if(vecWorldEntries.begin(), vecWorldEntries.end(),
                                     [strTmp](const std::pair<std::string, int>& element){ return element.first == strTmp; });

        if (it == vecWorldEntries.end())
            vecWorldEntries.emplace_back(std::make_pair(strTmp, 1));
        else
            ++it->second;
    }


    this->vecFileList.push_back(strFileName);

    /* Column in which our file is located in the vector. -1  */
    const std::size_t iColumn = std::distance(vecFileList.begin(), vecFileList.end()) - 1;

    if (this->mapMatrixData.empty())
    {
        /* Fill the matrix with raw data from our vector */
        for (const auto& it : vecWorldEntries)
            this->mapMatrixData.emplace(it.first, std::vector<float>(1, static_cast<float>(it.second)));
    }
    else
    {
        for (const auto& it : vecWorldEntries)
        {
            const auto matrixIterator = this->mapMatrixData.find(it.first);
            if (matrixIterator == this->mapMatrixData.end())
            {
                std::vector<float> vecTmp(iColumn);
                vecTmp[iColumn - 1] = it.second;

                this->mapMatrixData.emplace(it.first, vecTmp);
            }
            else
                matrixIterator->second.push_back(it.second);
        }
    }

    /* Make all vectors have the same size to keep indexes same as filename ones. */
    for (auto& it : mapMatrixData)
        it.second.resize(iColumn);
}
