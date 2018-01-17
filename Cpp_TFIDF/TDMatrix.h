#pragma once
#include <unordered_map>
#include <filesystem>
#include <string>

namespace filesystem = std::experimental::filesystem;

class TDMatrix
{
public:
    TDMatrix(const std::wstring& vecFileNames);

private:
    void AddToMatrix(const filesystem::directory_entry& fsDirectoryEntry);
    std::vector<std::wstring> vecFileList;
    std::unordered_map<std::wstring, std::vector<float>> tupleMatrixData;
};

