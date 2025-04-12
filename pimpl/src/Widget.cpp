// Widget.cpp
#include "Widget.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

namespace Interface {

// 条款31: 将文件间的编译依存关系降至最低
// 这里实现WidgetImpl类，客户不需要知道其细节
class Widget::WidgetImpl {
public:
    explicit WidgetImpl(const std::string& name = "")
        : name_(name) {}
    
    void setName(const std::string& name) { name_ = name; }
    const std::string& getName() const { return name_; }
    
    void addFeature(const std::string& feature) { 
        features_.push_back(feature);
    }
    
    size_t getFeatureCount() const { 
        return features_.size();
    }
    
    std::string getFeature(size_t index) const {
        if (index < features_.size()) {
            return features_[index];
        }
        return "";
    }
    
    void display() const {
        std::cout << "Widget名称: " << name_ << std::endl;
        std::cout << "特性列表 (" << features_.size() << "):" << std::endl;
        for (size_t i = 0; i < features_.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << features_[i] << std::endl;
        }
    }
    
private:
    std::string name_;
    std::vector<std::string> features_;
};

// 构造函数
Widget::Widget() 
    : pImpl_(std::make_unique<WidgetImpl>()) {
    std::cout << "创建默认Widget" << std::endl;
}

Widget::Widget(const std::string& name) 
    : pImpl_(std::make_unique<WidgetImpl>(name)) {
    std::cout << "创建Widget: " << name << std::endl;
}

// 析构函数
Widget::~Widget() {
    std::cout << "销毁Widget: " << pImpl_->getName() << std::endl;
    // unique_ptr会自动释放WidgetImpl
}

// 拷贝构造函数
Widget::Widget(const Widget& other) 
    : pImpl_(std::make_unique<WidgetImpl>(*other.pImpl_)) {
    std::cout << "拷贝构造Widget: " << pImpl_->getName() << std::endl;
}

// 拷贝赋值操作符
Widget& Widget::operator=(const Widget& other) {
    // 条款11: 在operator=中处理"自我赋值"
    if (this != &other) {
        // 条款12: 复制对象时勿忘其每一个成分
        // 创建副本，然后交换 - 复制并交换惯用法
        Widget temp(other);
        swap(temp);
    }
    std::cout << "拷贝赋值Widget: " << pImpl_->getName() << std::endl;
    return *this;
}

// 移动构造函数
Widget::Widget(Widget&& other) noexcept
    : pImpl_(std::move(other.pImpl_)) {
    std::cout << "移动构造Widget" << std::endl;
}

// 移动赋值操作符
Widget& Widget::operator=(Widget&& other) noexcept {
    if (this != &other) {
        pImpl_ = std::move(other.pImpl_);
    }
    std::cout << "移动赋值Widget" << std::endl;
    return *this;
}

// 条款25: 考虑写出一个不抛异常的swap函数
void Widget::swap(Widget& other) noexcept {
    using std::swap;
    swap(pImpl_, other.pImpl_);
}

// 转发到实现类的公共方法
void Widget::setName(const std::string& name) {
    pImpl_->setName(name);
}

const std::string& Widget::getName() const {
    return pImpl_->getName();
}

void Widget::addFeature(const std::string& feature) {
    pImpl_->addFeature(feature);
}

size_t Widget::getFeatureCount() const {
    return pImpl_->getFeatureCount();
}

std::string Widget::getFeature(size_t index) const {
    return pImpl_->getFeature(index);
}

void Widget::display() const {
    pImpl_->display();
}

} // namespace Interface