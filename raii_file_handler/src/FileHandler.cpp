// FileHandler.cpp
#include "FileHandler.hpp"
#include <iostream>
#include <sstream>

namespace FileSystem {

// 转换FileMode为std::ios::openmode
std::ios::openmode convertFileMode(FileMode mode) {
    std::ios::openmode openMode = std::ios::binary; // 总是使用二进制模式
    
    switch (mode) {
        case FileMode::Read:
            openMode |= std::ios::in;
            break;
        case FileMode::Write:
            openMode |= std::ios::out | std::ios::trunc;
            break;
        case FileMode::ReadWrite:
            openMode |= std::ios::in | std::ios::out;
            break;
        case FileMode::Append:
            openMode |= std::ios::out | std::ios::app;
            break;
    }
    
    return openMode;
}

// ===== FileHandle 实现 =====

FileHandle::FileHandle(const std::string& filepath, FileMode mode)
    : filepath_(filepath) {
    
    // 条款13: 以对象管理资源 - 构造函数获取资源
    std::ios::openmode openMode = convertFileMode(mode);
    
    // 检查如果是只读模式但文件不存在，则抛出异常
    if ((openMode & std::ios::in) && !(openMode & std::ios::out)) {
        std::ifstream checkFile(filepath);
        if (!checkFile.good()) {
            throw FileException("无法打开文件进行读取: " + filepath);
        }
    }
    
    file_.open(filepath_, openMode);
    
    if (!file_.is_open()) {
        throw FileException("无法打开文件: " + filepath_);
    }
    
    std::cout << "文件已打开: " << filepath_ << std::endl;
}

FileHandle::~FileHandle() {
    // 条款13: 以对象管理资源 - 析构函数释放资源
    // 条款8: 别让异常逃离析构函数
    try {
        if (file_.is_open()) {
            file_.close();
            std::cout << "文件已关闭: " << filepath_ << std::endl;
        }
    } catch (...) {
        std::cerr << "警告: 关闭文件时发生异常: " << filepath_ << std::endl;
    }
}

void FileHandle::close() {
    if (file_.is_open()) {
        file_.close();
        std::cout << "文件已关闭: " << filepath_ << std::endl;
    }
}

// ===== FileHandler 实现 =====

FileHandler::FileHandler(const std::string& filepath, FileMode mode)
    : filepath_(filepath), mode_(mode) {
    // 条款17: 以独立语句将newed对象置入智能指针
    // 使用独立语句创建FileHandle，确保异常安全
    fileHandle_ = std::make_unique<FileHandle>(filepath, mode);
}

FileHandler::~FileHandler() {
    // 智能指针会自动管理资源释放
    std::cout << "FileHandler销毁，自动关闭文件" << std::endl;
}

// 移动构造函数
FileHandler::FileHandler(FileHandler&& other) noexcept
    : fileHandle_(std::move(other.fileHandle_)),
      filepath_(std::move(other.filepath_)),
      mode_(other.mode_) {
    std::cout << "FileHandler移动构造" << std::endl;
}

// 移动赋值操作符
FileHandler& FileHandler::operator=(FileHandler&& other) noexcept {
    // 条款11: 在operator=中处理"自我赋值" 
    if (this != &other) {
        fileHandle_ = std::move(other.fileHandle_);
        filepath_ = std::move(other.filepath_);
        mode_ = other.mode_;
    }
    std::cout << "FileHandler移动赋值" << std::endl;
    return *this;
}

void FileHandler::ensureOpen() const {
    if (!fileHandle_ || !fileHandle_->isOpen()) {
        throw FileException("文件未打开或已关闭: " + filepath_);
    }
}

void FileHandler::write(const std::string& content) {
    // 条款29: 为"异常安全"努力是值得的
    ensureOpen();
    
    auto& file = fileHandle_->getRawHandle();
    file << content;
    
    if (file.fail()) {
        throw FileException("写入文件失败: " + filepath_);
    }
    
    // 确保数据写入磁盘
    file.flush();
}

void FileHandler::writeLine(const std::string& line) {
    write(line + "\n");
}

std::string FileHandler::read() {
    ensureOpen();
    
    auto& file = fileHandle_->getRawHandle();
    
    // 保存当前文件位置
    auto currentPos = file.tellg();
    
    // 移动到文件末尾
    file.seekg(0, std::ios::end);
    // 获取文件大小
    auto size = file.tellg();
    // 回到文件开头
    file.seekg(0, std::ios::beg);
    
    // 预分配足够的内存
    std::string content;
    content.resize(static_cast<size_t>(size));
    
    // 读取整个文件
    file.read(&content[0], size);
    
    if (file.fail() && !file.eof()) {
        throw FileException("读取文件失败: " + filepath_);
    }
    
    // 恢复文件位置
    file.clear(); // 清除EOF标志
    file.seekg(currentPos);
    
    return content;
}

std::vector<std::string> FileHandler::readLines() {
    std::vector<std::string> lines;
    std::string content = read();
    std::istringstream iss(content);
    
    std::string line;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

// ===== LineIterator 实现 =====

LineIterator::LineIterator(FileHandler& fileHandler)
    : fileHandler_(fileHandler), currentLine_(0) {
    loadIfNeeded();
}

bool LineIterator::hasNext() const {
    return currentLine_ < lines_.size();
}

std::string LineIterator::next() {
    if (!hasNext()) {
        throw FileException("没有更多行可读");
    }
    
    return lines_[currentLine_++];
}

void LineIterator::loadIfNeeded() {
    if (lines_.empty()) {
        lines_ = fileHandler_.readLines();
    }
}

} // namespace FileSystem