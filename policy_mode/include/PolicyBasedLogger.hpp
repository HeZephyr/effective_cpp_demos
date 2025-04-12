// PolicyBasedLogger.hpp
#ifndef POLICY_BASED_LOGGER_HPP
#define POLICY_BASED_LOGGER_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

namespace PolicyBased {

// 前置声明
class NullMutex;

// 条款35: 考虑virtual函数以外的其他选择
// 使用策略模式（Policy-Based Design）替代继承和虚函数

// ========================
// 日志格式化策略
// ========================

// 简单格式化策略
class SimpleFormatter {
public:
    static std::string format(const std::string& message) {
        return message;
    }
};

// 带时间戳的格式化策略
class TimestampFormatter {
public:
    static std::string format(const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "[%Y-%m-%d %H:%M:%S] ") << message;
        return ss.str();
    }
};

// 带线程ID的格式化策略
class ThreadFormatter {
public:
    static std::string format(const std::string& message) {
        std::stringstream ss;
        ss << "[线程 " << std::this_thread::get_id() << "] " << message;
        return ss.str();
    }
};

// ========================
// 日志输出策略
// ========================

// 控制台输出策略
class ConsoleOutput {
public:
    void write(const std::string& message) {
        std::cout << message << std::endl;
    }
};

// 文件输出策略
class FileOutput {
public:
    explicit FileOutput(const std::string& filename = "log.txt") 
        : filename_(filename) {
        file_.open(filename_, std::ios::app);
        if (!file_.is_open()) {
            throw std::runtime_error("无法打开日志文件: " + filename_);
        }
    }
    
    ~FileOutput() {
        if (file_.is_open()) {
            file_.close();
        }
    }
    
    void write(const std::string& message) {
        if (file_.is_open()) {
            file_ << message << std::endl;
            file_.flush();
        }
    }
    
private:
    std::string filename_;
    std::ofstream file_;
};

// 内存缓冲输出策略
class BufferedOutput {
public:
    void write(const std::string& message) {
        buffer_.push_back(message);
    }
    
    const std::vector<std::string>& getBuffer() const {
        return buffer_;
    }
    
    void clear() {
        buffer_.clear();
    }
    
    void dumpToConsole() {
        for (const auto& msg : buffer_) {
            std::cout << msg << std::endl;
        }
    }
    
    void dumpToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            for (const auto& msg : buffer_) {
                file << msg << std::endl;
            }
            file.close();
        }
    }
    
private:
    std::vector<std::string> buffer_;
};

// ========================
// 线程安全策略
// ========================

// 空互斥锁策略（无线程安全）
class NullMutex {
public:
    void lock() {}
    void unlock() {}
};

// 实际互斥锁策略（线程安全）
class StdMutex {
public:
    void lock() {
        mutex_.lock();
    }
    
    void unlock() {
        mutex_.unlock();
    }
    
private:
    std::mutex mutex_;
};

// ========================
// 日志级别策略
// ========================

// 简单日志级别策略
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

// 日志级别过滤策略
template<LogLevel MinLevel>
class LevelFilter {
public:
    static bool shouldLog(LogLevel level) {
        return level >= MinLevel;
    }
    
    static std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug:   return "[调试] ";
            case LogLevel::Info:    return "[信息] ";
            case LogLevel::Warning: return "[警告] ";
            case LogLevel::Error:   return "[错误] ";
            case LogLevel::Fatal:   return "[致命] ";
            default:                return "[未知] ";
        }
    }
};

// ========================
// 主日志类 - 使用策略模式组合功能
// ========================

// 条款38: 通过复合塑模出has-a或"根据某物实现出"
// 条款41: 了解隐式接口和编译期多态

template<
    typename FormatterPolicy = SimpleFormatter,
    typename OutputPolicy = ConsoleOutput,
    typename ThreadingPolicy = NullMutex,
    typename FilterPolicy = LevelFilter<LogLevel::Debug>
>
class Logger {
public:
    // 通过构造函数转发参数给策略类
    template<typename... Args>
    explicit Logger(Args&&... args) 
        : output_(std::forward<Args>(args)...) {}
    
    // 记录日志的主要方法
    void log(LogLevel level, const std::string& message) {
        // 使用过滤策略检查是否应该记录此级别
        if (!FilterPolicy::shouldLog(level)) {
            return;
        }
        
        // 使用线程安全策略
        threading_.lock();
        
        // 使用格式化策略
        std::string formatted = FormatterPolicy::format(
            FilterPolicy::levelToString(level) + message
        );
        
        // 使用输出策略
        output_.write(formatted);
        
        threading_.unlock();
    }
    
    // 便捷方法
    void debug(const std::string& message) {
        log(LogLevel::Debug, message);
    }
    
    void info(const std::string& message) {
        log(LogLevel::Info, message);
    }
    
    void warning(const std::string& message) {
        log(LogLevel::Warning, message);
    }
    
    void error(const std::string& message) {
        log(LogLevel::Error, message);
    }
    
    void fatal(const std::string& message) {
        log(LogLevel::Fatal, message);
    }
    
    // 获取输出策略引用
    OutputPolicy& getOutput() {
        return output_;
    }
    
    const OutputPolicy& getOutput() const {
        return output_;
    }
    
private:
    OutputPolicy output_;
    ThreadingPolicy threading_;
};

// 使用typedef/using简化常用组合
// 条款43：学习处理模板化基类内的名称
using ConsoleLogger = Logger<SimpleFormatter, ConsoleOutput, NullMutex>;
using FileLogger = Logger<TimestampFormatter, FileOutput, StdMutex>;
using BufferedLogger = Logger<ThreadFormatter, BufferedOutput, StdMutex>;

// 条款42：了解typename的双重意义
template<typename T>
class LoggerFactory {
public:
    // 这里的typename表示Container::value_type是一个类型而不是静态成员
    template<typename Container>
    static void logContainer(Logger<T, ConsoleOutput>& logger, const Container& container) {
        logger.info("容器内容:");
        
        // 使用SFINAE和if constexpr检测容器元素是否为std::pair类型(如map)
        if constexpr (is_pair_container<Container>::value) {
            // 特殊处理std::map等关联容器
            for (const auto& [key, value] : container) {
                std::ostringstream oss;
                oss << "  " << key << " -> " << value;
                logger.info(oss.str());
            }
        } else {
            // 处理普通容器
            for (const auto& value : container) {
                std::ostringstream oss;
                oss << "  " << value;
                logger.info(oss.str());
            }
        }
    }
    
private:
    // 辅助模板元编程：检测容器的value_type是否为std::pair
    template<typename C, typename = void>
    struct is_pair_container : std::false_type {};
    
    template<typename C>
    struct is_pair_container<C, 
        std::void_t<decltype(std::declval<typename C::value_type::first_type>(), 
                             std::declval<typename C::value_type::second_type>())>> 
        : std::true_type {};
};

} // namespace PolicyBased

#endif // POLICY_BASED_LOGGER_HPP