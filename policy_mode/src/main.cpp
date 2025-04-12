// main.cpp
#include "PolicyBasedLogger.hpp"
#include <vector>
#include <map>
#include <thread>

using namespace PolicyBased;

// 线程函数：使用日志器记录消息
void workerThread(Logger<ThreadFormatter, ConsoleOutput, StdMutex>& logger, int id) {
    logger.info("线程 " + std::to_string(id) + " 启动");
    
    // 模拟一些工作
    std::this_thread::sleep_for(std::chrono::milliseconds(id * 100));
    
    logger.info("线程 " + std::to_string(id) + " 完成工作");
}

int main() {
    try {
        std::cout << "===== 策略模式设计示例开始 =====" << std::endl;
        
        // 创建不同类型的日志器
        std::cout << "\n-- 控制台日志器 --" << std::endl;
        ConsoleLogger consoleLogger;
        consoleLogger.debug("这是一条调试消息");
        consoleLogger.info("这是一条信息消息");
        consoleLogger.warning("这是一条警告消息");
        consoleLogger.error("这是一条错误消息");
        
        // 文件日志器
        std::cout << "\n-- 文件日志器 --" << std::endl;
        FileLogger fileLogger("example.log");
        fileLogger.info("日志已写入文件");
        fileLogger.warning("这条警告也会写入文件");
        std::cout << "消息已写入example.log文件" << std::endl;
        
        // 带缓冲的日志器
        std::cout << "\n-- 缓冲日志器 --" << std::endl;
        BufferedLogger bufferedLogger;
        bufferedLogger.info("这条消息会被缓存");
        bufferedLogger.warning("这条警告也会被缓存");
        bufferedLogger.error("严重错误！");
        
        // 显示缓冲内容
        std::cout << "缓冲区内容:" << std::endl;
        const auto& buffer = bufferedLogger.getOutput().getBuffer();
        for (const auto& msg : buffer) {
            std::cout << "  " << msg << std::endl;
        }
        
        // 将缓冲区内容写入文件
        bufferedLogger.getOutput().dumpToFile("buffer_dump.log");
        std::cout << "缓冲内容已写入buffer_dump.log文件" << std::endl;
        
        // 使用自定义组合的日志器
        std::cout << "\n-- 自定义日志器 --" << std::endl;
        Logger<TimestampFormatter, ConsoleOutput, NullMutex, LevelFilter<LogLevel::Warning>> 
            warningLogger;
        warningLogger.debug("这条调试消息不会显示"); // 会被过滤
        warningLogger.info("这条信息也不会显示");    // 会被过滤
        warningLogger.warning("这条警告会显示");
        warningLogger.error("这条错误也会显示");
        
        // 多线程日志示例
        std::cout << "\n-- 多线程日志示例 --" << std::endl;
        Logger<ThreadFormatter, ConsoleOutput, StdMutex> threadLogger;
        
        std::vector<std::thread> threads;
        for (int i = 1; i <= 3; ++i) {
            threads.emplace_back(workerThread, std::ref(threadLogger), i);
        }
        
        // 等待所有线程完成
        for (auto& t : threads) {
            t.join();
        }
        
        // 使用LoggerFactory记录容器内容
        std::cout << "\n-- 容器日志记录 --" << std::endl;
        std::vector<int> numbers = {1, 2, 3, 4, 5};
        std::map<std::string, int> scores = {{"张三", 85}, {"李四", 92}, {"王五", 78}};
        
        LoggerFactory<SimpleFormatter>::logContainer(consoleLogger, numbers);
        LoggerFactory<SimpleFormatter>::logContainer(consoleLogger, scores);
        
        std::cout << "\n===== 策略模式设计示例结束 =====" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}