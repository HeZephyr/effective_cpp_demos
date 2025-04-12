#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <string>
#include <fstream>

namespace FileSystem {

class FileHandler {
public:
    // 通过构造函数获取资源（条款13）
    explicit FileHandler(const std::string& filepath);
    
    // 析构函数自动释放资源（条款13）
    ~FileHandler();

    // 禁止拷贝（条款6）
    FileHandler(const FileHandler&) = delete;
    FileHandler& operator=(const FileHandler&) = delete;

    // 显式资源操作接口
    void write(const std::string& content);
    std::string read();

private:
    std::fstream file_;
    std::string filepath_;
};

} // namespace FileSystem

#endif