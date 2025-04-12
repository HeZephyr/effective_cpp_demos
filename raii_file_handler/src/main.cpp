#include "FileHandler.hpp"
#include <iostream>

int main() {
    try {
        // RAII对象自动管理文件生命周期
        FileSystem::FileHandler fh("test.txt");
        
        fh.write("Hello Effective C++!\n");
        std::cout << "File content: \n" << fh.read() << std::endl;
        
        // 不需要显式调用close，析构函数自动处理
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}