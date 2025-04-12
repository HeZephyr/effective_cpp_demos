#include "Widget.hpp"
#include <iostream>

using namespace Interface;

int main() {
    try {
        std::cout << "===== Pimpl模式演示开始 =====" << std::endl;
        
        // 创建Widget对象
        Widget widget("智能手机");
        
        // 添加特性
        widget.addFeature("触摸屏");
        widget.addFeature("摄像头");
        widget.addFeature("GPS");
        
        // 显示Widget信息
        std::cout << "\n原始Widget:" << std::endl;
        widget.display();
        
        // 条款12: 复制对象时勿忘其每一个成分
        std::cout << "\n拷贝Widget:" << std::endl;
        Widget copyWidget = widget;
        copyWidget.setName("平板电脑");
        copyWidget.addFeature("大屏幕");
        copyWidget.display();
        
        // 原始Widget保持不变
        std::cout << "\n原始Widget保持不变:" << std::endl;
        widget.display();
        
        // 条款25: 考虑写出一个不抛异常的swap函数
        std::cout << "\n交换两个Widget:" << std::endl;
        swap(widget, copyWidget);
        std::cout << "交换后的原始Widget:" << std::endl;
        widget.display();
        std::cout << "交换后的拷贝Widget:" << std::endl;
        copyWidget.display();
        
        // 创建使用移动构造的Widget
        std::cout << "\n移动构造:" << std::endl;
        Widget movedWidget(std::move(copyWidget));
        movedWidget.display();
        
        std::cout << "\n===== Pimpl模式演示结束 =====" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}