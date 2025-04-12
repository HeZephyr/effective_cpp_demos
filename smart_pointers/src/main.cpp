#include "ResourceHandler.hpp"
#include <memory>
#include <iostream>

int main() {
    // 创建一个 ResourceHandler 对象，用于管理资源
    MemoryManagement::ResourceHandler handler;

    // Item 18: 创建一个带有自定义数组删除器的 unique_ptr
    // 自定义删除器，用于释放动态分配的数组
    auto arrDeleter = [](int* p) { delete[] p; };
    // 创建一个 unique_ptr 来管理动态分配的整数数组
    // 数组初始化为 {1, 2, 3}，大小为 5
    std::unique_ptr<int[], decltype(arrDeleter)> arrayResource(new int[5]{1, 2, 3}, arrDeleter);

    // Item 18: 所有权转移演示
    // 使用 std::make_unique 创建一个指向整数 42 的 unique_ptr，并将其所有权转移给 handler
    handler.addResource(std::make_unique<int>(42));
    // 使用 std::make_unique 创建一个指向整数 99 的 unique_ptr，并将其所有权转移给 handler
    handler.addResource(std::make_unique<int>(99));

    // Item 18: 共享所有权示例
    // 使用 std::make_shared 创建一个指向 std::vector<int> 的 shared_ptr
    // 向量初始化为 {10, 20}
    auto sharedData = std::make_shared<std::vector<int>>(std::initializer_list<int>{10, 20});
    // 将 shared_ptr 传递给 handler，实现资源的共享所有权
    handler.shareResource(sharedData);

    // 调用 handler 的 printResources 方法，打印管理的资源信息
    handler.printResources();

    // Item 18: unique_ptr 会在离开作用域时自动释放资源
    // 程序正常结束，返回 0
    return 0;
}