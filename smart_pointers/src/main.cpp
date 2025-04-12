// main.cpp
#include "ResourceHandler.hpp"
#include <iostream>

using namespace MemoryManagement;

int main() {
    try {
        std::cout << "===== 智能指针演示开始 =====" << std::endl;
        
        // 创建资源处理器
        ResourceHandler handler;
        
        // 条款17: 以独立语句将newed对象置入智能指针
        // 创建独占资源
        {
            auto memResource = std::make_unique<MemoryResource>(1, "内存缓存", 1024);
            handler.addExclusiveResource(std::move(memResource));
        }
        
        // 创建另一个独占资源
        handler.addExclusiveResource(std::make_unique<FileResource>(2, "配置文件", "/etc/config.json"));
        
        // 条款14: 在资源管理类中小心copying行为
        // 创建共享资源
        auto logFile = std::make_shared<FileResource>(3, "日志文件", "/var/log/app.log");
        handler.addSharedResource(logFile);
        
        // 创建另一个引用到同一个共享资源
        auto backupHandler = [logFile] {
            std::cout << "备份处理器也使用同一个日志文件，引用计数=" << logFile.use_count() << std::endl;
        };
        backupHandler();
        
        // 打印所有资源
        handler.printResources();
        
        // 使用资源
        handler.useResource(1);
        handler.useResource(2);
        handler.useResource(3);
        
        // 尝试使用不存在的资源
        handler.useResource(99);
        
        // 条款13: 以对象管理资源
        // 程序结束时，ResourceHandler的析构函数会自动释放所有资源
        std::cout << "\n===== 智能指针演示结束 =====" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}