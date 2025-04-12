// main.cpp
#include "FileHandler.hpp"
#include <iostream>

using namespace FileSystem;

int main() {
    try {
        std::cout << "===== RAII文件处理器演示开始 =====" << std::endl;
        
        // 条款13: 以对象管理资源
        // 创建临时测试文件
        {
            std::cout << "\n-- 写入测试文件 --" << std::endl;
            FileHandler writer("test.txt", FileMode::Write);
            writer.writeLine("第一行: RAII演示");
            writer.writeLine("第二行: 资源获取即初始化");
            writer.writeLine("第三行: 通过析构函数自动释放资源");
            writer.writeLine("第四行: 确保异常安全");
            std::cout << "文件写入完成" << std::endl;
        } // writer在这里自动销毁，文件自动关闭
        
        // 读取文件并显示内容
        {
            std::cout << "\n-- 读取测试文件 --" << std::endl;
            FileHandler reader("test.txt", FileMode::Read);
            
            std::cout << "全部内容:" << std::endl;
            std::cout << reader.read() << std::endl;
            
            std::cout << "按行读取:" << std::endl;
            LineIterator it(reader);
            while (it.hasNext()) {
                std::cout << "  > " << it.next() << std::endl;
            }
        } // reader在这里自动销毁
        
        // 条款14: 在资源管理类中小心copying行为
        // 演示移动语义
        std::cout << "\n-- 移动语义演示 --" << std::endl;
        FileHandler original("test.txt", FileMode::ReadWrite);
        
        // 移动构造
        FileHandler moved(std::move(original));
        std::cout << "移动后的文件路径: " << moved.getFilepath() << std::endl;
        
        // 原始对象现在处于有效但未指定的状态
        std::cout << "原始对象仍然是有效的对象，但文件句柄已移动" << std::endl;
        
        // 条款29: 为"异常安全"努力是值得的
        std::cout << "\n-- 异常处理演示 --" << std::endl;
        try {
            FileHandler nonExistent("non_existent_file.txt", FileMode::Read);
        } catch (const FileException& e) {
            std::cout << "预期的异常: " << e.what() << std::endl;
        }
        
        std::cout << "\n===== RAII文件处理器演示结束 =====" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "未捕获的异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}