// ResourceHandler.hpp
#ifndef RESOURCE_HANDLER_HPP
#define RESOURCE_HANDLER_HPP

#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace MemoryManagement {

// 前向声明
class Resource;

// 资源基类
class Resource {
public:
    explicit Resource(int id, std::string name);
    virtual ~Resource() = 0; // 条款7: 为多态基类声明virtual析构函数
    
    // 常量成员函数，获取资源ID和名称
    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    virtual void use() = 0;

private:
    int id_;
    std::string name_;
};

// 内存资源类型
class MemoryResource : public Resource {
public:
    MemoryResource(int id, std::string name, size_t size);
    ~MemoryResource() override;
    
    void use() override;
    
private:
    size_t size_;
    std::unique_ptr<char[]> buffer_;
};

// 文件资源类型
class FileResource : public Resource {
public:
    FileResource(int id, std::string name, std::string path);
    ~FileResource() override;
    
    void use() override;
    
private:
    std::string path_;
};

// 资源处理器类
class ResourceHandler {
public:
    // 条款4: 确定对象被使用前已先被初始化
    ResourceHandler();
    
    // 条款13: 以对象管理资源 (RAII)
    // 析构函数中自动释放所有资源
    ~ResourceHandler();
    
    // 条款6: 若不想使用编译器自动生成的函数，就该明确拒绝
    // 禁止拷贝和赋值
    ResourceHandler(const ResourceHandler&) = delete;
    ResourceHandler& operator=(const ResourceHandler&) = delete;
    
    // 条款17: 以独立语句将newed对象置入智能指针
    // 添加独占资源，所有权转移
    void addExclusiveResource(std::unique_ptr<Resource> resource);
    
    // 条款14: 在资源管理类中小心copying行为
    // 添加共享资源，资源可以被多个对象共享
    void addSharedResource(std::shared_ptr<Resource> resource);
    
    // 条款15: 在资源管理类中提供对原始资源的访问
    // 获取资源但不转移所有权
    Resource* getExclusiveResource(int id) const;
    std::shared_ptr<Resource> getSharedResource(int id) const;
    
    // 使用指定ID的资源
    void useResource(int id) const;
    
    // 打印所有资源信息
    void printResources() const;
    
    // 条款10: 令operator=返回一个reference to *this
    // 移动赋值操作符
    ResourceHandler& operator=(ResourceHandler&& other) noexcept;
    
    // 移动构造函数
    ResourceHandler(ResourceHandler&& other) noexcept;

private:
    // 独占资源列表，使用unique_ptr确保资源自动释放
    std::vector<std::unique_ptr<Resource>> exclusiveResources_;
    
    // 共享资源列表，使用shared_ptr允许资源共享
    std::vector<std::shared_ptr<Resource>> sharedResources_;
    
    // 条款16: 成对使用new和delete时要采取相同形式
    // 带有自定义删除器的unique_ptr，用于特殊资源管理
    std::unique_ptr<int, std::function<void(int*)>> trackedResource_;
};

} // namespace MemoryManagement

#endif // RESOURCE_HANDLER_HPP
