// Type Traits
#ifndef TYPE_TRAITS_HPP
#define TYPE_TRAITS_HPP

#include <type_traits>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <chrono>

namespace MetaProgramming {

// 条款47: 使用traits classes表现类型信息
// 容器特性traits类
template<typename Container>
struct ContainerTraits {
    // 默认实现，未知容器类型
    static constexpr bool is_container = false;
    static constexpr bool is_sequential = false;
    static constexpr bool is_associative = false;
    static constexpr bool has_random_access = false;
    
    // 打印容器类型信息
    static void print_info() {
        std::cout << "未知的容器类型" << std::endl;
    }
};

// vector特化
template<typename T, typename Alloc>
struct ContainerTraits<std::vector<T, Alloc>> {
    static constexpr bool is_container = true;
    static constexpr bool is_sequential = true;
    static constexpr bool is_associative = false;
    static constexpr bool has_random_access = true;
    
    using value_type = T;
    
    static void print_info() {
        std::cout << "容器类型: std::vector" << std::endl;
        std::cout << "特性: 连续存储, 随机访问, 动态数组" << std::endl;
    }
};

// list特化
template<typename T, typename Alloc>
struct ContainerTraits<std::list<T, Alloc>> {
    static constexpr bool is_container = true;
    static constexpr bool is_sequential = true;
    static constexpr bool is_associative = false;
    static constexpr bool has_random_access = false;
    
    using value_type = T;
    
    static void print_info() {
        std::cout << "容器类型: std::list" << std::endl;
        std::cout << "特性: 双向链表, 非连续存储, 快速插入和删除" << std::endl;
    }
};

// map特化
template<typename Key, typename T, typename Compare, typename Alloc>
struct ContainerTraits<std::map<Key, T, Compare, Alloc>> {
    static constexpr bool is_container = true;
    static constexpr bool is_sequential = false;
    static constexpr bool is_associative = true;
    static constexpr bool has_random_access = false;
    
    using key_type = Key;
    using mapped_type = T;
    
    static void print_info() {
        std::cout << "容器类型: std::map" << std::endl;
        std::cout << "特性: 关联容器, 键值对, 自动排序(红黑树)" << std::endl;
    }
};

// 条款48: 认识template元编程
// 编译期阶乘计算
template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N-1>::value;
};

// 特化处理终止条件
template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

// 编译期斐波那契数列
template<int N>
struct Fibonacci {
    static constexpr int value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
};

template<>
struct Fibonacci<0> {
    static constexpr int value = 0;
};

template<>
struct Fibonacci<1> {
    static constexpr int value = 1;
};

// 条款48: 使用SFINAE (Substitution Failure Is Not An Error)
// 检测类型是否有特定方法的traits

// 检测类型是否有size()方法
template<typename T, typename = void>
struct HasSizeMethod : std::false_type {};

template<typename T>
struct HasSizeMethod<T, 
    std::void_t<decltype(std::declval<T>().size())>> 
    : std::true_type {};

// 检测类型是否可以输出到流
template<typename T, typename = void>
struct IsStreamable : std::false_type {};

template<typename T>
struct IsStreamable<T, 
    std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> 
    : std::true_type {};

// 泛型算法示例，根据容器特性选择最优实现
template<typename Container>
void optimize_process(Container& container) {
    if constexpr (ContainerTraits<Container>::has_random_access) {
        std::cout << "使用随机访问优化算法" << std::endl;
        // 随机访问容器优化算法实现
        auto size = container.size();
        if (size > 0) {
            std::cout << "  访问第一个元素: " << container[0] << std::endl;
            if (size > 1) {
                std::cout << "  访问最后一个元素: " << container[size-1] << std::endl;
            }
        }
    } else if constexpr (ContainerTraits<Container>::is_sequential) {
        std::cout << "使用顺序访问算法" << std::endl;
        // 顺序容器算法实现
        if (!container.empty()) {
            std::cout << "  访问第一个元素: " << container.front() << std::endl;
            std::cout << "  访问最后一个元素: " << container.back() << std::endl;
        }
    } else if constexpr (ContainerTraits<Container>::is_associative) {
        std::cout << "使用关联容器算法" << std::endl;
        // 关联容器算法实现
        if (!container.empty()) {
            std::cout << "  遍历键值对:" << std::endl;
            int count = 0;
            for (const auto& [key, value] : container) {
                std::cout << "    " << key << " -> " << value << std::endl;
                if (++count >= 3) break; // 只显示前几个
            }
        }
    } else {
        std::cout << "使用通用算法" << std::endl;
        // 通用算法实现 - 使用迭代器遍历
        if (HasSizeMethod<Container>::value && !container.empty()) {
            std::cout << "  容器大小: " << container.size() << std::endl;
            std::cout << "  遍历内容:" << std::endl;
            for (const auto& item : container) {
                if constexpr (IsStreamable<decltype(item)>::value) {
                    std::cout << "    " << item << std::endl;
                } else {
                    std::cout << "    (无法打印的元素类型)" << std::endl;
                }
            }
        }
    }
}

// 使用SFINAE优化函数重载
// 针对拥有size()方法的容器
template<typename Container>
typename std::enable_if<HasSizeMethod<Container>::value, void>::type
print_size(const Container& c) {
    std::cout << "容器大小: " << c.size() << std::endl;
}

// 针对没有size()方法的类型
template<typename Container>
typename std::enable_if<!HasSizeMethod<Container>::value, void>::type
print_size(const Container&) {
    std::cout << "该类型没有size()方法" << std::endl;
}

// 条款41: 了解隐式接口和编译期多态
// 泛型打印函数，根据类型特性选择打印方式
template<typename T>
void smart_print(const T& value) {
    if constexpr (IsStreamable<T>::value) {
        std::cout << "值: " << value << std::endl;
    } else if constexpr (HasSizeMethod<T>::value) {
        std::cout << "对象具有size()方法，大小为: " << value.size() << std::endl;
    } else {
        std::cout << "无法直接打印此类型" << std::endl;
    }
}

// 条款42: 了解typename的双重意义
// 使用typename指定嵌套依赖类型
template<typename Container>
void process_value_type() {
    if constexpr (ContainerTraits<Container>::is_container) {
        // 这里的typename告诉编译器，value_type是一个类型而不是静态成员
        using value_type = typename Container::value_type;
        std::cout << "容器的值类型: " << typeid(value_type).name() << std::endl;
    } else {
        std::cout << "不是一个已知的容器类型" << std::endl;
    }
}

// 编译期类型特性检测与优化
// 条款44: 将与参数无关的代码抽离templates
template<typename Container>
class ContainerProcessor {
public:
    void process(Container& container) {
        // 打印容器信息
        ContainerTraits<Container>::print_info();
        
        // 输出容器大小
        print_size(container);
        
        // 根据容器特性选择优化算法
        optimize_process(container);
    }
};

// 性能计时器（使用RAII模式）
class Timer {
public:
    Timer(const std::string& operation) 
        : operation_(operation), start_(std::chrono::high_resolution_clock::now()) {
        std::cout << "开始 " << operation_ << std::endl;
    }
    
    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
        std::cout << operation_ << " 完成，耗时: " << duration << " 微秒" << std::endl;
    }
    
private:
    std::string operation_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

} // namespace MetaProgramming

#endif // TYPE_TRAITS_HPP