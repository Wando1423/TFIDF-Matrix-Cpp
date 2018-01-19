#pragma once
#include <map>
#include <filesystem>
#include <string>

namespace filesystem = std::experimental::filesystem;

class TDMatrix
{
public:
    TDMatrix(std::string& vecFileNames);
    ~TDMatrix() = default;

    void        ConvertToTFIDF           (std::map<std::string, std::vector<float>>* mapToChange = nullptr);
    std::size_t GetFileIndex             (const std::string& strFileName);
    std::string GetMostSimmilarFile      (const std::string& strCompared);
    std::vector<std::string> GetFileSimmRanking(const std::string& strCompared);

private:
    void AddToMatrix              (const filesystem::directory_entry& fsDirectoryEntry);
    void CalculateCosineSimilarity(const std::string& strEntry, std::vector<float>* vecOutput);

    bool bIsTFIDF = false;
    std::vector<std::string>                  vecFileList;      /* Vector containing our files so we can easily get their index. */
    std::map<std::string, std::vector<float>> mapMatrixData;
};