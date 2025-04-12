#include "FileHandler.hpp"
#include <stdexcept>

namespace FileSystem {

FileHandler::FileHandler(const std::string& filepath) 
    : filepath_(filepath) {
    // 以读写和追加模式打开文件
    // std::ios::in：可读；std::ios::out：可写；std::ios::app：写入时追加到文件末尾
    file_.open(filepath_, std::ios::in | std::ios::out | std::ios::app);
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath_);
    }
}

FileHandler::~FileHandler() {
    if (file_.is_open()) {
        file_.close(); // RAII保证资源释放（条款13）
    }
}

void FileHandler::write(const std::string& content) {
    if (file_.is_open()) {
        file_ << content;
    }
}

std::string FileHandler::read() {
    // 用于存储从文件中读取的内容
    std::string content;
    if (file_.is_open()) {

        // 将文件指针移动到文件末尾
        file_.seekg(0, std::ios::end);

        // 根据文件的大小调整存储内容的字符串的大小
        content.resize(file_.tellg());

        // 将文件指针移动到文件开头
        file_.seekg(0, std::ios::beg);

        // 从文件中读取数据到字符串中
        file_.read(&content[0], content.size());
    }

    // 返回读取到的文件内容
    return content;
}

} // namespace FileSystem