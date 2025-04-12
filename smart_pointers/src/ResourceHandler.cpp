#include "ResourceHandler.hpp"
#include <iostream>

namespace MemoryManagement {

// Item 18: 构造函数演示默认初始化
// 初始化 trackedResource_ 并使用自定义删除器
ResourceHandler::ResourceHandler()
    : trackedResource_(new int(0), [](int* p) {
        // 在删除资源前打印日志，显示要删除的资源的值
        std::cout << "Deleting tracked resource: " << *p << "\n";
        // 释放资源
        delete p;
    }) {}

// Item 18: 从调用者处转移所有权
// 接收一个 std::unique_ptr<int> 类型的资源，使用 std::move 将所有权转移到 exclusiveResources_ 向量中
void ResourceHandler::addResource(std::unique_ptr<int> resource) {
    exclusiveResources_.push_back(std::move(resource));
}

// Item 18: 共享所有权演示
// 接收一个 std::shared_ptr<std::vector<int>> 类型的资源，将其赋值给 sharedResource_，实现资源共享
void ResourceHandler::shareResource(std::shared_ptr<std::vector<int>> sharedRes) {
    sharedResource_ = sharedRes;
}

// 打印管理的所有资源信息
void ResourceHandler::printResources() const {
    std::cout << "Exclusive resources:\n";
    // 遍历 exclusiveResources_ 向量，打印每个独占资源的值
    for (const auto& res : exclusiveResources_) {
        std::cout << *res << " ";
    }

    std::cout << "\nShared resources:\n";
    // 检查 sharedResource_ 是否有效
    if (sharedResource_) {
        // 遍历共享资源向量，打印每个元素的值
        for (int val : *sharedResource_) {
            std::cout << val << " ";
        }
    }
    std::cout << "\n";
}

} // namespace MemoryManagement