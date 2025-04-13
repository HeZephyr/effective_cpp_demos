// main.cpp
#include "ThreadSafeSingleton.hpp"
#include <thread>
#include <vector>
#include <iostream>

using namespace ThreadSafety;

// 演示Meyers单例的线程安全性
void testMeyersSingleton(int id) {
    std::cout << "线程 " << id << " 尝试访问Meyers单例..." << std::endl;
    auto& singleton = MeyersSingleton::getInstance();
    singleton.safeMethod();
    std::cout << "线程 " << id << " 成功访问Meyers单例" << std::endl;
}

// 演示CallOnce单例的线程安全性
void testCallOnceSingleton(int id) {
    std::cout << "线程 " << id << " 尝试访问CallOnce单例..." << std::endl;
    auto& singleton = CallOnceSingleton::getInstance();
    singleton.safeMethod();
    std::cout << "线程 " << id << " 成功访问CallOnce单例" << std::endl;
}

// 演示Atomic单例的线程安全性
void testAtomicSingleton(int id) {
    std::cout << "线程 " << id << " 尝试访问Atomic单例..." << std::endl;
    auto& singleton = AtomicSingleton::getInstance();
    singleton.safeMethod();
    std::cout << "线程 " << id << " 成功访问Atomic单例" << std::endl;
}

// 演示应用配置单例
void testAppConfigSingleton(int id) {
    std::cout << "线程 " << id << " 尝试访问AppConfig单例..." << std::endl;
    
    // 获取配置单例
    auto config = AppConfigSingleton::getInstance();
    
    // 读取或修改配置
    if (id % 2 == 0) {
        // 偶数线程读取配置
        std::string appName = config->getConfigValue("app.name");
        std::string threadKey = "thread." + std::to_string(id);
        std::cout << "线程 " << id << " 读取配置: app.name = " << appName << std::endl;
        
        // 检查是否有当前线程的配置
        std::string threadValue = config->getConfigValue(threadKey);
        if (!threadValue.empty()) {
            std::cout << "线程 " << id << " 读取配置: " << threadKey << " = " << threadValue << std::endl;
        }
    } else {
        // 奇数线程写入配置
        std::string threadKey = "thread." + std::to_string(id);
        std::string threadValue = "Thread " + std::to_string(id) + " was here";
        config->setConfigValue(threadKey, threadValue);
    }
    
    // 模拟一些工作
    ThreadUtil::simulateWork(50);
    
    std::cout << "线程 " << id << " 完成访问AppConfig单例" << std::endl;
}

// 使用BaseConfig接口
void testBaseConfigInterface() {
    std::cout << "\n-- 测试BaseConfig接口 --" << std::endl;
    
    // 通过基类引用访问配置
    std::shared_ptr<BaseConfig> baseConfig = AppConfigSingleton::getBaseConfig();
    std::cout << "基类接口可以访问名称: " << baseConfig->getName() << std::endl;
    
    // 条款27: 尽量少做转型
    // 必要时进行动态类型转换
    if (auto appConfig = std::dynamic_pointer_cast<AppConfig>(baseConfig)) {
        std::cout << "成功转换回AppConfig，访问app.version = " 
                  << appConfig->getConfigValue("app.version") << std::endl;
    } else {
        std::cout << "转换失败，不是AppConfig类型" << std::endl;
    }
}

int main() {
    try {
        std::cout << "===== 线程安全单例模式示例开始 =====" << std::endl;
        
        // 演示所有单例类型
        SingletonManager::demoAllSingletons();
        
        // 测试基类接口
        testBaseConfigInterface();
        
        // 多线程测试Meyers单例
        std::cout << "\n-- 多线程测试Meyers单例 --" << std::endl;
        ThreadUtil::runInParallel(testMeyersSingleton, 3);
        
        // 多线程测试CallOnce单例
        std::cout << "\n-- 多线程测试CallOnce单例 --" << std::endl;
        ThreadUtil::runInParallel(testCallOnceSingleton, 3);
        
        // 多线程测试Atomic单例
        std::cout << "\n-- 多线程测试Atomic单例 --" << std::endl;
        ThreadUtil::runInParallel(testAtomicSingleton, 3);
        
        // 多线程测试应用配置单例
        std::cout << "\n-- 多线程测试应用配置单例 --" << std::endl;
        ThreadUtil::runInParallel(testAppConfigSingleton, 5);
        
        // 程序结束前清理
        SingletonManager::cleanup();
        
        std::cout << "\n===== 线程安全单例模式示例结束 =====" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}