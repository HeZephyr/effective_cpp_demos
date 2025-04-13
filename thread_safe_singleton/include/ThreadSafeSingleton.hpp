// ThreadSafeSingleton.hpp
#ifndef THREAD_SAFE_SINGLETON_HPP
#define THREAD_SAFE_SINGLETON_HPP

#include <iostream>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

namespace ThreadSafety {

// 前置声明
class ThreadUtil;

// 条款27: 尽量少做转型
class BaseConfig {
public:
    virtual ~BaseConfig() = default;
    virtual std::string getName() const = 0;
    
protected:
    // 条款4: 确定对象被使用前已先被初始化
    BaseConfig() {
        std::cout << "BaseConfig 构造" << std::endl;
    }
};

// 应用配置类
class AppConfig : public BaseConfig {
public:
    // 添加静态工厂方法，直接使用 new 创建实例
    static std::shared_ptr<AppConfig> createInstance() {
        return std::shared_ptr<AppConfig>(new AppConfig());
    }

    std::string getName() const override {
        return "AppConfig";
    }
    
    void setConfigValue(const std::string& key, const std::string& value) {
        std::cout << "设置配置: " << key << " = " << value << std::endl;
        config_[key] = value;
    }
    
    std::string getConfigValue(const std::string& key) const {
        auto it = config_.find(key);
        if (it != config_.end()) {
            return it->second;
        }
        return "";
    }
    
    void loadDefaults() {
        setConfigValue("app.name", "线程安全单例示例");
        setConfigValue("app.version", "1.0.0");
        setConfigValue("app.maxThreads", "10");
    }
    
    friend class AppConfigSingleton; // 允许单例访问私有构造函数
    
private:
    // 条款4: 确定对象被使用前已先被初始化
    AppConfig() {
        std::cout << "AppConfig 构造" << std::endl;
        loadDefaults();
    }
    
    std::unordered_map<std::string, std::string> config_;
};

// 线程安全工具类
class ThreadUtil {
public:
    // 模拟一个耗时操作
    static void simulateWork(int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
    
    // 获取当前线程ID的字符串表示
    static std::string getThreadId() {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        return oss.str();
    }
    
    // 并行执行多个函数
    template<typename Func>
    static void runInParallel(Func func, int numThreads) {
        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(func, i);
        }
        
        for (auto& t : threads) {
            t.join();
        }
    }
};

// ========================
// 线程安全的单例实现 - 版本1：双重检查锁定 (DCLP)
// ========================
class DCLPSingleton {
public:
    static DCLPSingleton& getInstance() {
        if (instance_ == nullptr) {  // 第一次检查
            std::lock_guard<std::mutex> lock(mutex_);
            if (instance_ == nullptr) {  // 第二次检查
                instance_ = new DCLPSingleton();
            }
        }
        return *instance_;
    }

    // 不推荐使用，仅为示例
    void unsafe() {
        std::cout << "警告：此单例实现在某些平台上可能不是线程安全的！" << std::endl;
    }
    
private:
    DCLPSingleton() {
        std::cout << "DCLPSingleton 构造 (不推荐使用此模式)" << std::endl;
    }
    
    // 条款6: 若不想使用编译器自动生成的函数，就该明确拒绝
    DCLPSingleton(const DCLPSingleton&) = delete;
    DCLPSingleton& operator=(const DCLPSingleton&) = delete;
    
    static DCLPSingleton* instance_;  // 不再使用volatile，现代C++应使用std::atomic或其他同步机制
    static std::mutex mutex_;
};

// 静态成员初始化，移除volatile关键字
DCLPSingleton* DCLPSingleton::instance_ = nullptr;
std::mutex DCLPSingleton::mutex_;

// ========================
// 线程安全的单例实现 - 版本2：Meyers单例 (C++11及以后推荐)
// ========================
class MeyersSingleton {
public:
    // 条款4: 确定对象被使用前已先被初始化
    static MeyersSingleton& getInstance() {
        // 静态局部变量保证了线程安全的延迟初始化
        static MeyersSingleton instance;
        return instance;
    }
    
    void safeMethod() {
        std::cout << "MeyersSingleton: 此方法是线程安全的" << std::endl;
    }
    
private:
    MeyersSingleton() {
        std::cout << "MeyersSingleton 构造 (推荐使用此模式)" << std::endl;
        ThreadUtil::simulateWork(100); // 模拟一些初始化工作
    }
    
    // 条款6: 若不想使用编译器自动生成的函数，就该明确拒绝
    MeyersSingleton(const MeyersSingleton&) = delete;
    MeyersSingleton& operator=(const MeyersSingleton&) = delete;
};

// ========================
// 线程安全的单例实现 - 版本3：使用std::call_once和std::once_flag
// ========================
class CallOnceSingleton {
public:
    static CallOnceSingleton& getInstance() {
        // std::call_once函数确保initSingleton函数只会被调用一次
        // initFlag_是一个std::once_flag类型的静态变量，用于标记initSingleton函数是否已经被调用过
        std::call_once(initFlag_, &CallOnceSingleton::initSingleton);
        // 返回单例实例
        return *instance_;
    }
    
    void safeMethod() {
        std::cout << "CallOnceSingleton: 此方法是线程安全的" << std::endl;
    }
    
private:
    CallOnceSingleton() {
        std::cout << "CallOnceSingleton 构造" << std::endl;
        ThreadUtil::simulateWork(100); // 模拟一些初始化工作
    }
    
    // 条款6: 若不想使用编译器自动生成的函数，就该明确拒绝
    CallOnceSingleton(const CallOnceSingleton&) = delete;
    CallOnceSingleton& operator=(const CallOnceSingleton&) = delete;
    
    static void initSingleton() {
        instance_ = new CallOnceSingleton();
    }
    
    static std::once_flag initFlag_;
    static CallOnceSingleton* instance_;
};

// 静态成员初始化
std::once_flag CallOnceSingleton::initFlag_;
CallOnceSingleton* CallOnceSingleton::instance_ = nullptr;

// ========================
// 线程安全的单例实现 - 版本4：使用std::atomic (C++11)
// ========================
class AtomicSingleton {
public:
    static AtomicSingleton& getInstance() {
        // 使用内存顺序 memory_order_acquire
        AtomicSingleton* tmp = instance_.load(std::memory_order_acquire);
        // memory_order_acquire 确保在读取该值之后的所有内存操作都不会被重排到读取操作之前
        // 这里先尝试非锁定方式读取单例实例指针
        if (tmp == nullptr) {
            // 使用互斥锁 mutex_ 来保证在多线程环境下只有一个线程能进入临界区
            // std::lock_guard 是一个RAII（资源获取即初始化）类型，会在其生命周期结束时自动释放锁
            std::lock_guard<std::mutex> lock(mutex_);
            // 再次从原子变量 instance_ 中加载值，使用 memory_order_relaxed 内存顺序
            // memory_order_relaxed 只保证原子性，不保证内存顺序，这里是因为已经加锁，不需要更强的内存顺序
            tmp = instance_.load(std::memory_order_relaxed);
            // 再次检查指针是否为空，这是为了防止在加锁之前已经有其他线程创建了单例实例
            if (tmp == nullptr) {
                tmp = new AtomicSingleton();
                // 使用内存顺序 memory_order_release 将新创建的实例指针存储到原子变量 instance_ 中
                // memory_order_release 确保在存储操作之前的所有内存操作都不会被重排到存储操作之后
                // 这样其他线程在使用 memory_order_acquire 读取该值时能看到完整的初始化结果
                instance_.store(tmp, std::memory_order_release);
            }
        }
        return *tmp;
    }
    
    void safeMethod() {
        std::cout << "AtomicSingleton: 此方法是线程安全的" << std::endl;
    }
    
private:
    AtomicSingleton() {
        std::cout << "AtomicSingleton 构造" << std::endl;
        ThreadUtil::simulateWork(100); // 模拟一些初始化工作
    }
    
    // 条款6: 若不想使用编译器自动生成的函数，就该明确拒绝
    AtomicSingleton(const AtomicSingleton&) = delete;
    AtomicSingleton& operator=(const AtomicSingleton&) = delete;
    
    static std::atomic<AtomicSingleton*> instance_;
    static std::mutex mutex_;
};

// 静态成员初始化
std::atomic<AtomicSingleton*> AtomicSingleton::instance_(nullptr);
std::mutex AtomicSingleton::mutex_;

// ========================
// 实际应用：线程安全的配置单例
// ========================
class AppConfigSingleton {
public:
    // 条款29: 为"异常安全"努力是值得的
    static std::shared_ptr<AppConfig> getInstance() {
        // 使用 AppConfig::createInstance() 代替 std::make_shared<AppConfig>()
        static std::shared_ptr<AppConfig> instance = AppConfig::createInstance();
        return instance;
    }
    
    // 提供一个类型安全的静态转换方法
    // 条款27: 尽量少做转型
    static std::shared_ptr<BaseConfig> getBaseConfig() {
        // 安全地向上转型
        return std::static_pointer_cast<BaseConfig>(getInstance());
    }
    
private:
    AppConfigSingleton() = delete; // 防止实例化
};

// ========================
// 线程安全单例管理器
// ========================
class SingletonManager {
public:
    // 条款11: 在operator=中处理"自我赋值"
    static void cleanup() {
        std::cout << "SingletonManager: 清理单例资源..." << std::endl;
        // 在这里可以添加清理代码，例如释放全局资源
        // 注意：此方法不会释放使用静态局部变量实现的单例
    }
    
    static void demoAllSingletons() {
        std::cout << "\n-- 演示所有单例类型 --" << std::endl;
        
        // 不推荐的双重检查锁定模式
        DCLPSingleton::getInstance().unsafe();
        
        // Meyers单例模式
        MeyersSingleton::getInstance().safeMethod();
        
        // 使用std::call_once的单例模式
        CallOnceSingleton::getInstance().safeMethod();
        
        // 使用std::atomic的单例模式
        AtomicSingleton::getInstance().safeMethod();
        
        // 应用配置单例
        auto config = AppConfigSingleton::getInstance();
        std::cout << "应用名称: " << config->getConfigValue("app.name") << std::endl;
        std::cout << "应用版本: " << config->getConfigValue("app.version") << std::endl;
    }
};

} // namespace ThreadSafety

#endif // THREAD_SAFE_SINGLETON_HPP