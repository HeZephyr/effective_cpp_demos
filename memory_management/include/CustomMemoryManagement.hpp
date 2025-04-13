// CustomMemoryManagement.hpp
#ifndef CUSTOM_MEMORY_MANAGEMENT_HPP
#define CUSTOM_MEMORY_MANAGEMENT_HPP

#include <iostream>
#include <new>           // std::set_new_handler, std::bad_alloc
#include <cstddef>       // size_t, ptrdiff_t
#include <cstdlib>       // malloc, free
#include <memory>        // 智能指针
#include <mutex>         // std::mutex
#include <unordered_map> // 内存跟踪
#include <vector>        // 内存池实现
#include <algorithm>     // std::fill
#include <cassert>       // assert

namespace MemoryManagement {

// 前置声明
class MemoryPool;

// ========================
// 内存跟踪类 - 用于检测内存泄漏
// ========================
class MemoryTracker {
public:
    // 条款49: 了解new-handler的行为
    static void memoryExhausted() {
        std::cerr << "内存耗尽！尝试释放紧急内存池..." << std::endl;
        
        // 释放紧急内存池
        if (releaseEmergencyReserve()) {
            std::cout << "紧急内存池已释放，重试分配" << std::endl;
        } else {
            std::cerr << "无法恢复内存，即将抛出bad_alloc异常" << std::endl;
            // 重置new-handler使得下次分配直接失败并抛出异常
            std::set_new_handler(nullptr);
        }
    }
    
    // 获取单例
    static MemoryTracker& getInstance() {
        static MemoryTracker instance;
        return instance;
    }
    
    // 跟踪内存分配
    void recordAllocation(void* ptr, size_t size, const char* file, int line) {
        if (ptr == nullptr) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_[ptr] = AllocationInfo{size, file, line};
        totalAllocated_ += size;
        
        std::cout << "[内存] 分配 " << size << " 字节在 " << file 
                  << ":" << line << " 地址: " << ptr << std::endl;
    }
    
    // 跟踪内存释放
    void recordDeallocation(void* ptr) {
        if (ptr == nullptr) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = allocations_.find(ptr);
        if (it != allocations_.end()) {
            totalAllocated_ -= it->second.size;
            std::cout << "[内存] 释放 " << it->second.size 
                      << " 字节在地址: " << ptr << std::endl;
            allocations_.erase(it);
        } else {
            std::cerr << "[警告] 尝试释放未跟踪的内存: " << ptr << std::endl;
        }
    }
    
    // 检查内存是否被跟踪
    bool isTracked(void* ptr) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocations_.find(ptr) != allocations_.end();
    }
    
    // 打印内存泄漏报告
    void printLeakReport() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::cout << "\n====== 内存泄漏报告 ======" << std::endl;
        std::cout << "总分配: " << totalAllocated_ << " 字节" << std::endl;
        
        if (allocations_.empty()) {
            std::cout << "没有检测到内存泄漏" << std::endl;
        } else {
            std::cout << "检测到 " << allocations_.size() << " 处内存泄漏:" << std::endl;
            for (const auto& [ptr, info] : allocations_) {
                std::cout << "  泄漏: " << info.size << " 字节在 " 
                          << info.file << ":" << info.line 
                          << " 地址: " << ptr << std::endl;
            }
        }
        std::cout << "===========================" << std::endl;
    }
    
    // 获取当前分配总量
    size_t getTotalAllocated() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalAllocated_;
    }
    
private:
    // 条款49: 紧急内存池
    static bool createEmergencyReserve() {
        static bool created = false;
        if (!created) {
            try {
                // 分配1MB的紧急内存池
                emergencyReserve_ = malloc(EMERGENCY_POOL_SIZE);
                if (emergencyReserve_) {
                    created = true;
                    std::cout << "已创建紧急内存池" << std::endl;
                    return true;
                }
            } catch (...) {
                return false;
            }
        }
        return created;
    }
    
    static bool releaseEmergencyReserve() {
        if (emergencyReserve_) {
            free(emergencyReserve_);
            emergencyReserve_ = nullptr;
            return true;
        }
        return false;
    }

    // 禁止复制和赋值
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;
    
    // 分配信息结构
    struct AllocationInfo {
        size_t size;
        const char* file;
        int line;
    };
    
    MemoryTracker() : totalAllocated_(0) {
        // 初始化时创建紧急内存池
        createEmergencyReserve();
        // 设置new-handler
        std::set_new_handler(memoryExhausted);
    }
    
    ~MemoryTracker() {
        printLeakReport();
        
        // 释放紧急内存池
        releaseEmergencyReserve();
    }
    
    // 内存分配记录
    std::unordered_map<void*, AllocationInfo> allocations_;
    // 总分配内存
    size_t totalAllocated_;
    // 互斥锁保证线程安全
    mutable std::mutex mutex_;
    
    // 紧急内存池
    static void* emergencyReserve_;
    static constexpr size_t EMERGENCY_POOL_SIZE = 1024 * 1024; // 1MB
};

// 静态成员初始化
void* MemoryTracker::emergencyReserve_ = nullptr;

// ========================
// 内存池实现
// ========================
class MemoryPool {
public:
    // 构造函数，初始化内存池
    // blockSize 为每个内存块的大小，默认值是 1024 字节
    // poolSize 为内存池的总大小，默认值是 1024 * 1024 字节
    explicit MemoryPool(size_t blockSize = 1024, size_t poolSize = 1024 * 1024)
        : blockSize_(blockSize), poolSize_(poolSize), used_(0) {
        // 调用 malloc 函数分配指定大小的内存池
        memory_ = reinterpret_cast<char*>(malloc(poolSize_));
        // 若分配失败，抛出 std::bad_alloc 异常
        if (!memory_) {
            throw std::bad_alloc();
        }
        
        // 计算内存池能容纳的内存块数量
        size_t numBlocks = poolSize_ / blockSize_;
        // 为空闲块链表预分配空间，避免多次重新分配内存
        freeList_.reserve(numBlocks);
        
        // 初始化空闲块链表，将每个内存块的起始地址存入链表
        for (size_t i = 0; i < numBlocks; ++i) {
            freeList_.push_back(memory_ + i * blockSize_);
        }
        
        // 输出内存池创建信息，包含内存块数量和每个块的大小
        std::cout << "内存池已创建: " << numBlocks << " 块, 每块 " 
                    << blockSize_ << " 字节" << std::endl;
    }
    
    // 析构函数，释放内存池占用的内存
    ~MemoryPool() {
        // 若内存池存在，则释放内存并将指针置为 nullptr
        if (memory_) {
            free(memory_);
            memory_ = nullptr;
        }
        // 输出内存池销毁信息
        std::cout << "内存池已销毁" << std::endl;
    }
    
    // 从内存池分配一个内存块
    void* allocate() {
        // 使用互斥锁保证线程安全，防止多个线程同时访问空闲块链表
        std::lock_guard<std::mutex> lock(mutex_);
        
        // 若空闲块链表为空，说明内存池已满，返回 nullptr
        if (freeList_.empty()) {
            return nullptr; 
        }
        
        // 取出空闲块链表的最后一个元素作为要分配的内存块
        void* block = freeList_.back();
        // 从空闲块链表中移除该元素
        freeList_.pop_back();
        // 更新已使用的内存量
        used_ += blockSize_;
        
        return block;
    }
    
    // 将内存块释放回内存池
    void deallocate(void* ptr) {
        // 使用互斥锁保证线程安全，防止多个线程同时访问空闲块链表
        std::lock_guard<std::mutex> lock(mutex_);
        
        // 检查指针是否在内存池的地址范围内
        if (ptr >= memory_ && ptr < memory_ + poolSize_) {
            // 计算指针相对于内存池起始地址的偏移量
            uintptr_t offset = reinterpret_cast<char*>(ptr) - memory_;
            // 若偏移量不是块大小的整数倍，说明指针不是内存块的起始位置，输出警告信息并返回
            if (offset % blockSize_ != 0) {
                std::cerr << "[警告] 尝试释放非内存块起始位置: " << ptr << std::endl;
                return;
            }
            
            // 检查该指针是否已经在空闲块链表中，若存在则输出警告信息并返回
            if (std::find(freeList_.begin(), freeList_.end(), ptr) != freeList_.end()) {
                std::cerr << "[警告] 尝试多次释放同一块内存: " << ptr << std::endl;
                return;
            }
            
            // 将该指针添加到空闲块链表中
            freeList_.push_back(ptr);
            // 更新已使用的内存量
            used_ -= blockSize_;
            // 输出内存块回收信息
            std::cout << "[内存池] 回收块: " << ptr << std::endl;
        } else {
            // 若指针不在内存池地址范围内，输出警告信息
            std::cerr << "[警告] 尝试释放不属于此内存池的内存: " << ptr << std::endl;
        }
    }
    
    // 获取每个内存块的大小
    size_t getBlockSize() const {
        return blockSize_;
    }
    
    // 获取已使用的内存量
    size_t getUsed() const {
        // 使用互斥锁保证线程安全，防止多个线程同时访问已使用内存量
        std::lock_guard<std::mutex> lock(mutex_);
        return used_;
    }
    
    // 获取空闲块的数量
    size_t getFreeCount() const {
        // 使用互斥锁保证线程安全，防止多个线程同时访问空闲块链表
        std::lock_guard<std::mutex> lock(mutex_);
        return freeList_.size();
    }
    
    // 禁止复制构造函数，防止内存池被复制
    MemoryPool(const MemoryPool&) = delete;
    // 禁止赋值运算符，防止内存池被赋值
    MemoryPool& operator=(const MemoryPool&) = delete;
    
private:
    char* memory_;         // 内存池的起始地址
    size_t blockSize_;     // 每个内存块的大小
    size_t poolSize_;      // 内存池的总大小
    size_t used_;          // 已使用的内存字节数
    std::vector<void*> freeList_;  // 空闲块链表，存储可用的内存块地址
    mutable std::mutex mutex_;     // 互斥锁，用于保证线程安全
};

// ========================
// 可跟踪内存分配的类基类
// ========================

// 条款50: 了解new和delete的合理替换时机
// 条款51: 编写new和delete时需固守常规
class Trackable {
public:
    // 重载new运算符
    static void* operator new(size_t size) {
        if (size == 0) {
            size = 1; // 处理0大小请求
        }
        
        void* ptr = malloc(size);
        if (ptr == nullptr) {
            // 分配失败，会触发new-handler
            throw std::bad_alloc();
        }
        
        // 记录分配
        MemoryTracker::getInstance().recordAllocation(ptr, size, "Unknown", 0);
        return ptr;
    }
    
    // 重载delete运算符
    static void operator delete(void* ptr) noexcept {
        if (ptr == nullptr) return; // 处理空指针
        
        // 记录释放
        auto& tracker = MemoryTracker::getInstance();
        
        // 检查此指针是否在跟踪记录中
        if (tracker.isTracked(ptr)) {
            tracker.recordDeallocation(ptr);
            free(ptr);
        } else {
            std::cerr << "[警告] 尝试释放未跟踪的内存: " << ptr << std::endl;
            // 不调用free，因为这可能不是通过malloc分配的内存
        }
    }
    
    // 条款52: 写了placement new也要写placement delete
    // placement new
    static void* operator new(size_t size, const char* file, int line) {
        if (size == 0) size = 1;
        
        void* ptr = malloc(size);
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }
        
        // 记录分配，带文件和行号信息
        MemoryTracker::getInstance().recordAllocation(ptr, size, file, line);
        return ptr;
    }
    
    // 对应的placement delete - 当构造函数抛出异常时会被调用
    static void operator delete(void* ptr, const char* file, int line) noexcept {
        if (ptr == nullptr) return;
        
        std::cout << "[内存] 构造失败释放 " << file << ":" << line << std::endl;
        MemoryTracker::getInstance().recordDeallocation(ptr);
        free(ptr);
    }
    
    // 另一种placement new - 使用内存池
    static void* operator new(size_t size, MemoryPool& pool) {
        if (size > pool.getBlockSize()) {
            throw std::bad_alloc(); // 请求太大，内存池块不够大
        }
        
        void* ptr = pool.allocate();
        if (ptr == nullptr) {
            throw std::bad_alloc(); // 内存池已满
        }
        
        // 记录分配
        MemoryTracker::getInstance().recordAllocation(ptr, size, "MemoryPool", 0);
        return ptr;
    }
    
    // 对应的placement delete - 当构造函数抛出异常时会被调用
    static void operator delete(void* ptr, MemoryPool& pool) noexcept {
        if (ptr == nullptr) return;
        
        std::cout << "[内存] 构造失败释放回内存池" << std::endl;
        
        // 检查此指针是否在跟踪记录中
        auto& tracker = MemoryTracker::getInstance();
        if (tracker.isTracked(ptr)) {
            tracker.recordDeallocation(ptr);
        }
        
        // 将内存归还给内存池
        pool.deallocate(ptr);
    }
    
    // 数组分配版本
    static void* operator new[](size_t size) {
        void* ptr = malloc(size);
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }
        
        MemoryTracker::getInstance().recordAllocation(ptr, size, "Unknown[]", 0);
        return ptr;
    }
    
    // 数组释放版本
    static void operator delete[](void* ptr) noexcept {
        if (ptr == nullptr) return;
        
        MemoryTracker::getInstance().recordDeallocation(ptr);
        free(ptr);
    }
    
protected:
    // 虚析构函数使其成为多态基类
    virtual ~Trackable() = default;
};

// 替代malloc和free的宏，用于自动记录文件和行号
#define TRACKED_NEW new(__FILE__, __LINE__)
#define POOL_NEW(pool) new(pool)

// ========================
// 客户示例类
// ========================
class Widget : public Trackable {
public:
    Widget() : data_(nullptr) {
        std::cout << "Widget构造" << std::endl;
        
        // 在构造函数中分配内存
        data_ = new int[100];
    }
    
    // 可能抛出异常的构造函数，用于测试placement delete
    explicit Widget(bool throwException) : data_(nullptr) {
        std::cout << "Widget构造(可能抛出异常)" << std::endl;
        
        data_ = new int[100];
        
        if (throwException) {
            throw std::runtime_error("Widget构造故意失败");
        }
    }
    
    ~Widget() {
        std::cout << "Widget析构" << std::endl;
        
        // 释放在构造函数中分配的内存
        delete[] data_;
    }
    
    // 禁止复制和赋值
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;
    
private:
    int* data_;
};

// ========================
// 工具函数和测试函数
// ========================

// 打印内存统计信息
void printMemoryStats() {
    auto& tracker = MemoryTracker::getInstance();
    std::cout << "\n当前分配内存: " << tracker.getTotalAllocated() << " 字节" << std::endl;
}

// 测试基本内存跟踪
void testBasicTracking() {
    std::cout << "\n-- 测试基本内存跟踪 --" << std::endl;
    
    // 分配一个对象
    Widget* w1 = new Widget();
    printMemoryStats();
    
    // 释放对象
    delete w1;
    printMemoryStats();
    
    // 分配一个数组
    Widget* wArray = new Widget[3];
    printMemoryStats();
    
    // 释放数组
    delete[] wArray;
    printMemoryStats();
}

// 测试带位置信息的内存跟踪
void testTrackedNew() {
    std::cout << "\n-- 测试带位置信息的内存跟踪 --" << std::endl;
    
    // 使用带文件和行号信息的new
    Widget* w2 = TRACKED_NEW Widget();
    printMemoryStats();
    
    // 清理
    delete w2;
    printMemoryStats();
}

// 测试内存泄漏检测
void testMemoryLeak() {
    std::cout << "\n-- 测试内存泄漏检测 --" << std::endl;
    
    // 故意泄漏一个对象
    /*Widget* leak =*/ new Widget();
    std::cout << "故意泄漏了一个Widget对象" << std::endl;
    
    printMemoryStats();
}

// 测试内存池
void testMemoryPool() {
    std::cout << "\n-- 测试内存池 --" << std::endl;
    
    // 创建内存池
    MemoryPool pool(sizeof(Widget), 1024 * 10); // 10KB内存池
    
    std::cout << "初始空闲块数: " << pool.getFreeCount() << std::endl;
    
    // 从内存池分配对象 - 使用正确的构造方式
    try {
        Widget* w3 = POOL_NEW(pool) Widget();
        
        std::cout << "分配后空闲块数: " << pool.getFreeCount() << std::endl;
        std::cout << "已使用内存: " << pool.getUsed() << " 字节" << std::endl;
        
        // 注意：必须使用正确的析构方式，并直接将内存归还给内存池
        w3->~Widget(); // 显式调用析构函数
        pool.deallocate(w3); // 直接归还内存给内存池
        
        std::cout << "释放后空闲块数: " << pool.getFreeCount() << std::endl;
        std::cout << "已使用内存: " << pool.getUsed() << " 字节" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "内存池分配失败: " << e.what() << std::endl;
    }
}

// 测试构造函数异常与placement delete
void testConstructorException() {
    std::cout << "\n-- 测试构造函数异常与placement delete --" << std::endl;
    
    try {
        // 此构造函数会抛出异常
        Widget* w4 = TRACKED_NEW Widget(true);
        // 不会执行到这里
        delete w4;
    } catch (const std::exception& e) {
        std::cout << "捕获异常: " << e.what() << std::endl;
    }
    
    printMemoryStats();
    
    // 内存池版本
    MemoryPool pool(sizeof(Widget), 1024);
    
    try {
        // 此构造函数会抛出异常
        Widget* w5 = POOL_NEW(pool) Widget(true);
        // 不会执行到这里
        delete w5;
    } catch (const std::exception& e) {
        std::cout << "捕获异常(内存池版本): " << e.what() << std::endl;
    }
    
    printMemoryStats();
}

// 测试new-handler
void testNewHandler() {
    std::cout << "\n-- 测试new-handler --" << std::endl;
    
    try {
        // 尝试分配超大内存，可能触发new-handler
        std::cout << "尝试分配大量内存..." << std::endl;
        char* bigArray = new char[1024 * 1024 * 1024]; // 1GB
        delete[] bigArray;
    } catch (const std::bad_alloc& e) {
        std::cout << "捕获bad_alloc异常: 内存分配失败" << std::endl;
    }
}

} // namespace MemoryManagement

#endif // CUSTOM_MEMORY_MANAGEMENT_HPP