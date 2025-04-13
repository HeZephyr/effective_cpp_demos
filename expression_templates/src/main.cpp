// main.cpp
#include "ExpressionTemplates.hpp"
#include <iostream>
#include <iomanip>

using namespace ExpressionTemplates;

int main() {
    try {
        std::cout << "===== 表达式模板示例开始 =====" << std::endl;
        
        // 创建一些测试向量
        Vector<double> a(5, 1.0);  // 5个元素，初始值全为1.0
        Vector<double> b(5, 2.0);  // 5个元素，初始值全为2.0
        Vector<double> c(5, 3.0);  // 5个元素，初始值全为3.0
        
        // 修改一些值，使示例更有趣
        a[1] = 1.5; a[3] = 1.7;
        b[2] = 2.5; b[4] = 2.8;
        c[0] = 3.2; c[2] = 3.6;
        
        // 打印初始向量
        std::cout << "\n-- 初始向量 --" << std::endl;
        printVector(a, "a");
        printVector(b, "b");
        printVector(c, "c");
        
        // 基本向量操作
        std::cout << "\n-- 基本向量操作 --" << std::endl;
        
        Vector<double> sum = a + b;
        printVector(sum, "a + b");
        
        Vector<double> diff = a - b;
        printVector(diff, "a - b");
        
        Vector<double> scaled = a * 2.5;
        printVector(scaled, "a * 2.5");
        
        Vector<double> scaled2 = 3.0 * b;
        printVector(scaled2, "3.0 * b");
        
        // 复杂表达式
        std::cout << "\n-- 复杂表达式 --" << std::endl;
        
        Vector<double> expr1 = a + b * 2.0 - c;
        printVector(expr1, "a + b * 2.0 - c");
        
        Vector<double> expr2 = (a + b) * 2.0;
        printVector(expr2, "(a + b) * 2.0");
        
        // 数学函数应用
        std::cout << "\n-- 数学函数应用 --" << std::endl;
        
        Vector<double> sqrtVec = sqrt(a);
        printVector(sqrtVec, "sqrt(a)");
        
        Vector<double> absVec = abs(diff);
        printVector(absVec, "abs(a - b)");
        
        Vector<double> squareVec = square(a);
        printVector(squareVec, "square(a)");
        
        // 链式数学函数
        Vector<double> complexMath = sqrt(square(a) + square(b));
        printVector(complexMath, "sqrt(square(a) + square(b))");
        
        // 表达式模板实际上只会在赋值时求值，即惰性求值
        std::cout << "\n-- 惰性求值示例 --" << std::endl;
        
        auto expression = a + b * 2.0 - c;
        std::cout << "表达式已创建，但尚未求值" << std::endl;
        
        // 只有在这里才会实际计算表达式
        Vector<double> result = expression;
        std::cout << "表达式已求值并存储到结果向量中" << std::endl;
        printVector(result, "result");
        
        // 性能对比
        std::cout << "\n-- 性能对比 (小向量) --" << std::endl;
        comparePerformance<1000>();
        
        std::cout << "\n-- 性能对比 (中等向量) --" << std::endl;
        comparePerformance<100000>();
        
        std::cout << "\n-- 性能对比 (大向量) --" << std::endl;
        comparePerformance<1000000>();
        
        std::cout << "\n===== 表达式模板示例结束 =====" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}