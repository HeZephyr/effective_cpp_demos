// ResourceHandler.cpp
#include "ResourceHandler.hpp"
#include <iostream>
#include <algorithm>

namespace MemoryManagement {

// Resource 实现
Resource::Resource(int id, std::string name)
    : id_(id), name_(std::move(name)) {
    std::cout << "资源创建: ID=" << id_ << ", 名称=" << name_ << std::endl;
}

Resource::~Resource() {
    std::cout << "资源销毁: ID=" << id_ << ", 名称=" << name_ << std::endl;
}

// MemoryResource 实现
MemoryResource::MemoryResource(int id, std::string name, size_t size)
    : Resource(id, std::move(name)), size_(size) {
    // 条款17: 以独立语句将newed对象置入智能指针
    buffer_ = std::make_unique<char[]>(size);
    std::cout << "内存资源初始化: 大小=" << size_ << "字节" << std::endl;
}

MemoryResource::~MemoryResource() {
    std::cout << "内存资源释放: 大小=" << size_ << "字节" << std::endl;
}

void MemoryResource::use() {
    std::cout << "使用内存资源: ID=" << getId() << ", 名称=" << getName() 
              << ", 大小=" << size_ << "字节" << std::endl;
}

// FileResource 实现
FileResource::FileResource(int id, std::string name, std::string path)
    : Resource(id, std::move(name)), path_(std::move(path)) {
    std::cout << "文件资源初始化: 路径=" << path_ << std::endl;
}

FileResource::~FileResource() {
    std::cout << "文件资源释放: 路径=" << path_ << std::endl;
}

void FileResource::use() {
    std::cout << "使用文件资源: ID=" << getId() << ", 名称=" << getName() 
              << ", 路径=" << path_ << std::endl;
}

// ResourceHandler 实现
ResourceHandler::ResourceHandler()
    // 条款13: 以对象管理资源
    // 自定义删除器，用于在资源释放前记录日志
    : trackedResource_(new int(0), [](int* p) {
          std::cout << "释放跟踪资源，值为: " << *p << std::endl;
          delete p;
      }) {
    std::cout << "资源处理器初始化" << std::endl;
    // 调整容器初始大小，减少重新分配
    exclusiveResources_.reserve(10);
    sharedResources_.reserve(10);
}

ResourceHandler::~ResourceHandler() {
    // 条款8: 别让异常逃离析构函数
    try {
        std::cout << "资源处理器销毁，自动释放所有管理的资源" << std::endl;
        // unique_ptr和shared_ptr会自动管理资源释放
    } catch (...) {
        std::cerr << "警告: 资源释放过程中发生异常" << std::endl;
    }
}

ResourceHandler::ResourceHandler(ResourceHandler&& other) noexcept
    : exclusiveResources_(std::move(other.exclusiveResources_)),
      sharedResources_(std::move(other.sharedResources_)),
      trackedResource_(std::move(other.trackedResource_)) {
    std::cout << "资源处理器移动构造" << std::endl;
}

ResourceHandler& ResourceHandler::operator=(ResourceHandler&& other) noexcept {
    // 条款11: 在operator=中处理"自我赋值"
    if (this != &other) {
        exclusiveResources_ = std::move(other.exclusiveResources_);
        sharedResources_ = std::move(other.sharedResources_);
        trackedResource_ = std::move(other.trackedResource_);
    }
    std::cout << "资源处理器移动赋值" << std::endl;
    return *this;
}

void ResourceHandler::addExclusiveResource(std::unique_ptr<Resource> resource) {
    // 条款13: 以对象管理资源
    if (resource) {
        std::cout << "添加独占资源: ID=" << resource->getId() << std::endl;
        exclusiveResources_.push_back(std::move(resource));
    }
}

void ResourceHandler::addSharedResource(std::shared_ptr<Resource> resource) {
    // 条款14: 在资源管理类中小心copying行为
    if (resource) {
        std::cout << "添加共享资源: ID=" << resource->getId() << std::endl;
        sharedResources_.push_back(resource);
    }
}

Resource* ResourceHandler::getExclusiveResource(int id) const {
    // 条款15: 在资源管理类中提供对原始资源的访问
    auto it = std::find_if(exclusiveResources_.begin(), exclusiveResources_.end(),
                          [id](const auto& res) { return res->getId() == id; });
    
    return (it != exclusiveResources_.end()) ? it->get() : nullptr;
}

std::shared_ptr<Resource> ResourceHandler::getSharedResource(int id) const {
    auto it = std::find_if(sharedResources_.begin(), sharedResources_.end(),
                          [id](const auto& res) { return res->getId() == id; });
    
    return (it != sharedResources_.end()) ? *it : nullptr;
}

void ResourceHandler::useResource(int id) const {
    // 先检查独占资源
    Resource* res = getExclusiveResource(id);
    if (res) {
        res->use();
        return;
    }
    
    // 再检查共享资源
    auto sharedRes = getSharedResource(id);
    if (sharedRes) {
        sharedRes->use();
        return;
    }
    
    std::cout << "找不到ID为" << id << "的资源" << std::endl;
}

void ResourceHandler::printResources() const {
    std::cout << "\n====== 资源列表 ======" << std::endl;
    
    std::cout << "独占资源:" << std::endl;
    for (const auto& res : exclusiveResources_) {
        std::cout << "  ID=" << res->getId() << ", 名称=" << res->getName() << std::endl;
    }
    
    std::cout << "共享资源:" << std::endl;
    for (const auto& res : sharedResources_) {
        std::cout << "  ID=" << res->getId() << ", 名称=" << res->getName() 
                  << ", 引用计数=" << res.use_count() << std::endl;
    }
    
    std::cout << "=====================\n" << std::endl;
}

} // namespace MemoryManagement