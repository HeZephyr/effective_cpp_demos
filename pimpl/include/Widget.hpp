// Widget.hpp
#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <memory>
#include <string>

namespace Interface {

// 条款31: 将文件间的编译依存关系降至最低
// 使用Pimpl（指向实现的指针）技术
class Widget {
public:
    // 条款4: 确定对象被使用前已先被初始化
    Widget();
    explicit Widget(const std::string& name);
    
    // 条款7: 为多态基类声明virtual析构函数 (虽然不是多态基类，但这里需要正确释放WidgetImpl)
    ~Widget();
    
    // 条款12: 复制对象时勿忘其每一个成分
    Widget(const Widget& other);
    Widget& operator=(const Widget& other);
    
    // 条款25: 考虑写出一个不抛异常的swap函数
    void swap(Widget& other) noexcept;
    
    // 移动语义
    Widget(Widget&& other) noexcept;
    Widget& operator=(Widget&& other) noexcept;
    
    // 公共接口
    void setName(const std::string& name);
    const std::string& getName() const;
    
    void addFeature(const std::string& feature);
    size_t getFeatureCount() const;
    std::string getFeature(size_t index) const;
    
    void display() const;
    
private:
    // 前置声明WidgetImpl类
    class WidgetImpl;
    // 使用智能指针管理实现类实例
    std::unique_ptr<WidgetImpl> pImpl_;
};

// 非成员函数
// 条款2: 尽量以const, enum, inline替换#define
inline void swap(Widget& a, Widget& b) noexcept {
    a.swap(b);
}

} // namespace Interface

#endif // WIDGET_HPP