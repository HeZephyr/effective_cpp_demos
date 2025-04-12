#ifndef RESOURCE_HANDLER_HPP
#define RESOURCE_HANDLER_HPP

#include <memory>
#include <vector>

namespace MemoryManagement {

// Item 18: 使用 std::unique_ptr 实现独占所有权
// ResourceHandler 类用于管理不同类型的资源，展示智能指针的使用
class ResourceHandler {
public:
    // 构造函数
    ResourceHandler();

    // Item 18: 演示 unique_ptr 所有权转移
    // 该方法接收一个 std::unique_ptr<int> 类型的资源，将其所有权转移到类内部
    void addResource(std::unique_ptr<int> resource);

    // Item 18: 展示使用 shared_ptr 实现共享所有权
    // 该方法接收一个 std::shared_ptr<std::vector<int>> 类型的资源，实现资源的共享
    void shareResource(std::shared_ptr<std::vector<int>> sharedRes);

    // 打印管理的所有资源信息
    void printResources() const;

private:
    // Item 18: 带有自定义删除器的 unique_ptr（在删除前记录日志）
    // 自定义删除器会在资源被删除时打印信息
    std::unique_ptr<int, void(*)(int*)> trackedResource_;

    // 存储独占资源的向量，每个元素是一个 std::unique_ptr<int>
    std::vector<std::unique_ptr<int>> exclusiveResources_;

    // 存储共享资源的智能指针，使用 std::shared_ptr<std::vector<int>> 实现共享所有权
    std::shared_ptr<std::vector<int>> sharedResource_;
};

} // namespace MemoryManagement

#endif