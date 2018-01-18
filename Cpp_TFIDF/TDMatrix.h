#pragma once
#include <map>
#include <filesystem>
#include <string>

namespace filesystem = std::experimental::filesystem;

class TDMatrix
{
public:
    TDMatrix(const std::string& vecFileNames);
    ~TDMatrix() = default;

    void ConvertToTFIDF();
    int  GetFileIndex(const std::string& strFileName);

private:
    void AddToMatrix(const filesystem::directory_entry& fsDirectoryEntry);
    std::vector<std::string> vecFileList;
    std::map<std::string, std::vector<float>> mapMatrixData;
};