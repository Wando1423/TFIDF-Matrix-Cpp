#define NOMINMAX
#include "TDMatrix.h"
#include <vector>
#include <fstream>
#include <sstream>

TDMatrix::TDMatrix(std::string& strPath)
{
    if (strPath.at(strPath.size() - 1) != '\\')
        strPath.push_back('\\');

    for (const auto& it : filesystem::directory_iterator(strPath))
    {
        if (!is_empty(it))
            this->AddToMatrix(it);
    }
}


void TDMatrix::ConvertToTFIDF(std::map<std::string, std::vector<float>>* mapToChange)
{
    static float docCount = 1;
    std::map<std::string, std::vector<float>>* mapTemp;
    if (!mapToChange)
    {
        this->bIsTFIDF = true;
        docCount       = static_cast<float>(this->vecFileList.size());
        mapTemp        = &this->mapMatrixData;
    }
    else
        mapTemp = mapToChange;

    /* Loop to calculate and insert weighted counters */
    for (auto& it : *mapTemp)
    {
        static float       iCounter = 0;
        std::vector<float> vecWeight;
        for (auto vecIter : it.second)
        {
            if (vecIter > 0)
                ++iCounter;

            vecWeight.push_back(vecIter);
        }

        if (iCounter != 0)
        {
            for (auto& vecIter : vecWeight)
                vecIter *= std::log2f(docCount / iCounter);

            for (std::size_t vecIter = 0; vecIter < it.second.size(); ++vecIter)
                it.second.at(vecIter) *= vecWeight.at(vecIter);
        }
    }
}


std::size_t TDMatrix::GetFileIndex(const std::string& strFileName)
{
    const auto it = std::find(this->vecFileList.begin(), this->vecFileList.end(), strFileName);
    if (it == this->vecFileList.end())
        return -1;

    return std::distance(this->vecFileList.begin(), it);
}


std::string TDMatrix::GetMostSimmilarFile(const std::string& strCompared)
{
    std::vector<float> vecSimilarities;
    this->CalculateCosineSimilarity(strCompared, &vecSimilarities);

    if (vecSimilarities.size() == 1u)
        return this->vecFileList.at(0);

    auto        flBiggestValue = 0.f;
    std::size_t idxClosestFile = 0u;

    for (std::size_t it = 0; it < vecSimilarities.size(); ++it)
    {
        if (vecSimilarities.at(it) > flBiggestValue)
        {
            idxClosestFile = it;
            flBiggestValue = vecSimilarities.at(it);
        }
    }

    return flBiggestValue == 0.f ? "No matches." : this->vecFileList.at(idxClosestFile);
}

std::vector<std::string> TDMatrix::GetFileSimmRanking(const std::string& strCompared)
{
    std::vector<float> vecSimilarities;
    this->CalculateCosineSimilarity(strCompared, &vecSimilarities);

    auto vecSorted = vecSimilarities;
    std::sort(vecSorted.rbegin(), vecSorted.rend());

    std::vector<std::string> vecResult { };
    for (const auto& it : vecSorted)
    {
        const auto foundVal      = std::find(vecSimilarities.begin(), vecSimilarities.end(), it);
        const auto dist          = std::distance(vecSimilarities.begin(), foundVal);
        vecSimilarities.at(dist) = -1;
        vecResult.push_back(this->vecFileList.at(dist));
    }
    return vecResult;
}


void TDMatrix::AddToMatrix(const filesystem::directory_entry& fsDirectoryEntry)
{
    const auto    strFileName(fsDirectoryEntry.path().filename().string());
    std::ifstream file(fsDirectoryEntry.path(), std::ios_base::binary);
    
    /* Double bracket so its not declared as a function */
    std::istringstream issFileContent(std::string(std::istreambuf_iterator<char>(file),
                                                  (std::istreambuf_iterator<char>())));


    std::string strTmp { };                                         /* Temporary string holding one word                 */
    std::vector<std::pair<std::string, int>> vecWorldEntries { };   /* Temporary vector holding words and their counters */
    while (issFileContent >> strTmp)
    {
        /* Convert to lower case and remove punctuaction */
        std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), ::tolower);
        std::remove_if(strTmp.begin(), strTmp.end(), [](char ch) { return ispunct(static_cast<unsigned char>(ch)); });

        const auto  it = std::find_if(vecWorldEntries.begin(), vecWorldEntries.end(),
                                      [strTmp](const std::pair<std::string, int>& element) { return element.first == strTmp; });

        if (it == vecWorldEntries.end())
            vecWorldEntries.emplace_back(std::make_pair(strTmp, 1));
        else
            ++it->second;
    }

    /* Add our file to file list vector to keep the proper index saved. */
    this->vecFileList.push_back(strFileName);

    /* Column in which our file is located in the vector. -1 because we are checking end index */
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
            const auto& matrixIterator = this->mapMatrixData.find(it.first);
            if (matrixIterator == this->mapMatrixData.end())
            {
                /* If the word is not found, create vector multiple entries so our word counter has a proper index. */
                std::vector<float> vecTmp(iColumn + 1);
                vecTmp[iColumn] = static_cast<float>(it.second);

                this->mapMatrixData.emplace(it.first, vecTmp);
            }
            else
                matrixIterator->second.push_back(static_cast<const float>(it.second));
        }
    }

    /* Make all vectors have the same size to keep indexes same as filename ones. */
    for (auto& it : mapMatrixData)
        it.second.resize(this->vecFileList.size());
}


void TDMatrix::CalculateCosineSimilarity(const std::string& strEntry, std::vector<float>* vecOutput)
{
    std::istringstream issStringContent(strEntry);
    std::string        strTmp { };

    std::map<std::string, std::vector<float>> mapEntryData { };
    while (issStringContent >> strTmp)
    {
        /* Convert to lower case and remove punctuaction */
        std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), ::tolower);
        std::remove_if(strTmp.begin(), strTmp.end(), [](char ch) { return ispunct(static_cast<unsigned char>(ch)); });

        const auto it = mapEntryData.find(strTmp);

        if (it == mapEntryData.end())
            mapEntryData.emplace(strTmp, std::vector<float>(1, 1.f));
        else
            ++it->second[0];
    }

    /* Convert our TF martix into TFIDF format*/
    if (this->bIsTFIDF)
        this->ConvertToTFIDF(&mapEntryData);


    std::vector<std::string> vecWordList;
    /* Disable multiple memory reallocations */
    vecWordList.reserve(mapEntryData.size());

    for (const auto& it : mapEntryData)
        vecWordList.push_back(it.first);

    /* Gets similarity angle */
    const auto cosineSimilarity = [vecWordList, mapEntryData, this](std::size_t idx) -> float
    {
        auto flMultiply = 0.f, flSumAPow = 0.f, flSumBPow = 0.f;
        for (const auto& it : vecWordList)
        {
            auto valueA = 0.f, valueB = 0.f;

            /* Searches and returns word counter */
            const auto getVal = [idx, mapEntryData, this, it](bool bIsStringA) -> float
            {
                const auto* mapPtr = bIsStringA ? &mapEntryData : &this->mapMatrixData;

                auto find = mapPtr->find(it);
                if (find != mapPtr->end())
                    return find->second.at(bIsStringA ? 0 : idx);

                return 0.f;
            };

            valueA = getVal(true);
            valueB = getVal(false);

            flMultiply += valueA * valueB;
            flSumAPow += std::powf(valueA, 2.f);
            flSumBPow += std::powf(valueB, 2.f);
        }
        if (flSumAPow && flSumBPow)
            return flMultiply / (std::sqrtf(flSumAPow) * std::sqrtf(flSumBPow));
        else
            return 0;
    };


    vecOutput->reserve(this->vecFileList.size());
    for (std::size_t it = 0; it < this->vecFileList.size(); ++it)
        vecOutput->push_back(cosineSimilarity(it));
}
