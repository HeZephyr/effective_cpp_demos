// main.cpp
#include "CustomMemoryManagement.hpp"
#include <iostream>

using namespace MemoryManagement;

int main() {
    try {
        std::cout << "===== 自定义内存管理示例开始 =====" << std::endl;
        
        // 测试基本内存跟踪
        testBasicTracking();
        
        // 测试跟踪位置信息
        testTrackedNew();
        
        // 测试内存池
        testMemoryPool();
        
        // 测试构造函数异常处理
        testConstructorException();
        
        // 此测试可能导致程序崩溃，取消注释以测试
        // testNewHandler();
        
        // 测试内存泄漏检测
        testMemoryLeak();
        
        std::cout << "\n===== 自定义内存管理示例结束 =====" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "未捕获的异常: " << e.what() << std::endl;
        return 1;
    }
    
    // 程序结束时，MemoryTracker析构函数会打印内存泄漏报告
    return 0;
}