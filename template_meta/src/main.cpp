// main.cpp
#include "TypeTraits.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string>

using namespace MetaProgramming;

class UserType {
public:
    UserType() = default;
    
    // 没有size()方法
    // 也没有重载输出运算符
};

class BetterUserType {
public:
    BetterUserType() = default;
    
    size_t size() const { return 42; }
    // 但仍然没有重载输出运算符
};

// 为BetterUserType重载输出运算符
std::ostream& operator<<(std::ostream& os, const BetterUserType&) {
    return os << "BetterUserType实例";
}

int main() {
    try {
        std::cout << "===== 模板元编程与Traits示例开始 =====" << std::endl;
        
        // 条款48: 编译期计算示例
        std::cout << "\n-- 编译期计算 --" << std::endl;
        std::cout << "5的阶乘 (编译期计算): " << Factorial<5>::value << std::endl;
        std::cout << "10的斐波那契数 (编译期计算): " << Fibonacci<10>::value << std::endl;
        
        // 条款47: Traits类使用示例
        std::cout << "\n-- 容器Traits演示 --" << std::endl;
        
        // 创建不同类型的容器
        std::vector<int> vec = {1, 2, 3, 4, 5};
        std::list<double> lst = {1.1, 2.2, 3.3};
        std::map<std::string, int> mp = {{"one", 1}, {"two", 2}};
        
        // 使用Traits输出容器信息
        std::cout << "\nVector信息:" << std::endl;
        ContainerTraits<decltype(vec)>::print_info();
        
        std::cout << "\nList信息:" << std::endl;
        ContainerTraits<decltype(lst)>::print_info();
        
        std::cout << "\nMap信息:" << std::endl;
        ContainerTraits<decltype(mp)>::print_info();
        
        // 使用编译期条件选择最优实现
        std::cout << "\n-- 根据容器特性优化处理 --" << std::endl;
        {
            Timer t("处理vector");
            optimize_process(vec);
        }
        
        {
            Timer t("处理list");
            optimize_process(lst);
        }
        
        {
            Timer t("处理map");
            optimize_process(mp);
        }
        
        // 检测接口存在性示例
        std::cout << "\n-- SFINAE与接口检测 --" << std::endl;
        UserType user;
        BetterUserType betterUser;
        
        std::cout << "UserType has size(): " 
                  << (HasSizeMethod<UserType>::value ? "是" : "否") << std::endl;
        std::cout << "BetterUserType has size(): " 
                  << (HasSizeMethod<BetterUserType>::value ? "是" : "否") << std::endl;
        
        std::cout << "UserType is streamable: " 
                  << (IsStreamable<UserType>::value ? "是" : "否") << std::endl;
        std::cout << "BetterUserType is streamable: " 
                  << (IsStreamable<BetterUserType>::value ? "是" : "否") << std::endl;
        
        // 智能打印函数示例
        std::cout << "\n-- 智能打印函数 --" << std::endl;
        smart_print(42);                // 基本类型
        smart_print(std::string("你好"));  // 字符串
        smart_print(vec);               // 容器
        smart_print(user);              // 用户自定义类型
        smart_print(betterUser);        // 具有某些接口的用户自定义类型
        
        // 综合示例: 容器处理器
        std::cout << "\n-- 综合演示: 容器处理器 --" << std::endl;
        
        ContainerProcessor<std::vector<int>> vecProcessor;
        ContainerProcessor<std::list<double>> listProcessor;
        ContainerProcessor<std::map<std::string, int>> mapProcessor;
        
        std::cout << "\n处理Vector:" << std::endl;
        vecProcessor.process(vec);
        
        std::cout << "\n处理List:" << std::endl;
        listProcessor.process(lst);
        
        std::cout << "\n处理Map:" << std::endl;
        mapProcessor.process(mp);
        
        // 条款42: typename的使用
        std::cout << "\n-- typename的使用 --" << std::endl;
        process_value_type<std::vector<int>>();
        process_value_type<std::map<std::string, double>>();
        process_value_type<UserType>(); // 不是容器
        
        std::cout << "\n===== 模板元编程与Traits示例结束 =====" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}