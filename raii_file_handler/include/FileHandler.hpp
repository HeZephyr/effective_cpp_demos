// FileHandler.hpp
#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <stdexcept>

namespace FileSystem {

// 自定义异常类
class FileException : public std::runtime_error {
public:
    explicit FileException(const std::string& message) : std::runtime_error(message) {}
};

// 文件模式枚举
enum class FileMode {
    Read,       // 只读模式
    Write,      // 只写模式
    ReadWrite,  // 读写模式
    Append      // 追加模式
};

// 文件句柄类
class FileHandle {
public:
    // 条款13: 以对象管理资源 - 构造函数获取资源
    explicit FileHandle(const std::string& filepath, FileMode mode);
    
    // 条款13: 以对象管理资源 - 析构函数释放资源
    ~FileHandle();
    
    // 条款6: 若不想使用编译器自动生成的函数，就该明确拒绝
    // 禁止拷贝构造和拷贝赋值
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
    
    // 条款15: 在资源管理类中提供对原始资源的访问
    std::fstream& getRawHandle() { return file_; }
    const std::fstream& getRawHandle() const { return file_; }
    
    // 文件操作方法
    bool isOpen() const { return file_.is_open(); }
    void close();
    void flush() { file_.flush(); }

private:
    std::fstream file_;
    std::string filepath_;
};

// 文件处理器类
class FileHandler {
public:
    // 条款4: 确定对象被使用前已先被初始化
    explicit FileHandler(const std::string& filepath, FileMode mode = FileMode::ReadWrite);
    
    // 条款7: 为多态基类声明virtual析构函数 (虽然这不是多态基类，但养成良好习惯)
    ~FileHandler();
    
    // 条款6: 若不想使用编译器自动生成的函数，就该明确拒绝
    FileHandler(const FileHandler&) = delete;
    FileHandler& operator=(const FileHandler&) = delete;
    
    // 条款29: 为"异常安全"努力是值得的
    // 实现移动语义
    FileHandler(FileHandler&& other) noexcept;
    FileHandler& operator=(FileHandler&& other) noexcept;
    
    // 文件操作接口
    void write(const std::string& content);
    void writeLine(const std::string& line);
    std::string read();
    std::vector<std::string> readLines();
    
    // 条款3: 尽可能使用const
    bool isOpen() const { return fileHandle_ && fileHandle_->isOpen(); }
    const std::string& getFilepath() const { return filepath_; }
    
    // 条款15: 在资源管理类中提供对原始资源的访问
    FileHandle* getFileHandle() { return fileHandle_.get(); }
    const FileHandle* getFileHandle() const { return fileHandle_.get(); }

private:
    // 条款13: 以对象管理资源 (通过智能指针)
    std::unique_ptr<FileHandle> fileHandle_;
    std::string filepath_;
    FileMode mode_;
    
    // 辅助方法
    void ensureOpen() const;
};

// 文件读取迭代器 - 演示更高级的RAII用法
class LineIterator {
public:
    explicit LineIterator(FileHandler& fileHandler);
    ~LineIterator() = default;
    
    bool hasNext() const;
    std::string next();
    
private:
    FileHandler& fileHandler_;
    std::vector<std::string> lines_;
    size_t currentLine_;
    
    void loadIfNeeded();
};

} // namespace FileSystem

#endif // FILE_HANDLER_HPP