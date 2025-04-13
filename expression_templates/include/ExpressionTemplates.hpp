// ExpressionTemplates.hpp
#ifndef EXPRESSION_TEMPLATES_HPP
#define EXPRESSION_TEMPLATES_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <functional>

namespace ExpressionTemplates {

// 条款48: 认识template元编程
// 表达式模板是模板元编程的一种高级应用

// 前置声明
template<typename T>
class Vector;

// ========================
// 表达式模板基类，用于表示向量表达式
// ========================
template<typename Derived>
class VectorExpression {
public:
    // 重载operator[]，从派生类获取对应位置的元素
    double operator[](size_t i) const {
        return static_cast<const Derived&>(*this)[i];
    }
    
    // 获取表达式的大小
    size_t size() const {
        return static_cast<const Derived&>(*this).size();
    }
    
    // 将表达式转换为实际的向量
    operator Vector<double>() const;
};

// ========================
// 向量类，用于存储实际数据
// ========================
template<typename T>
class Vector : public VectorExpression<Vector<T>> {
public:
    // 默认构造函数
    Vector() = default;
    
    // 带大小的构造函数
    explicit Vector(size_t size) : data_(size) {}
    
    // 带初始值的构造函数
    Vector(size_t size, T value) : data_(size, value) {}
    
    // 从表达式构造向量
    template<typename Expr>
    Vector(const VectorExpression<Expr>& expr) : data_(expr.size()) {
        for (size_t i = 0; i < data_.size(); ++i) {
            data_[i] = expr[i];
        }
    }
    
    // 从表达式赋值
    template<typename Expr>
    Vector& operator=(const VectorExpression<Expr>& expr) {
        data_.resize(expr.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            data_[i] = expr[i];
        }
        return *this;
    }
    
    // 访问元素
    T& operator[](size_t i) {
        return data_[i];
    }
    
    // 常量访问元素
    const T& operator[](size_t i) const {
        return data_[i];
    }
    
    // 获取大小
    size_t size() const {
        return data_.size();
    }
    
    // 迭代器支持
    typename std::vector<T>::iterator begin() {
        return data_.begin();
    }
    
    typename std::vector<T>::iterator end() {
        return data_.end();
    }
    
    typename std::vector<T>::const_iterator begin() const {
        return data_.begin();
    }
    
    typename std::vector<T>::const_iterator end() const {
        return data_.end();
    }
    
private:
    std::vector<T> data_;
};

// 实现 VectorExpression::operator Vector<double>()
template<typename Derived>
VectorExpression<Derived>::operator Vector<double>() const {
    const Derived& derived = static_cast<const Derived&>(*this);
    Vector<double> result(derived.size());
    for (size_t i = 0; i < derived.size(); ++i) {
        result[i] = derived[i];
    }
    return result;
}

// ========================
// 向量加法表达式
// ========================
template<typename LhsExpr, typename RhsExpr>
class VectorSum : public VectorExpression<VectorSum<LhsExpr, RhsExpr>> {
public:
    VectorSum(const VectorExpression<LhsExpr>& lhs, 
             const VectorExpression<RhsExpr>& rhs)
        : lhs_(static_cast<const LhsExpr&>(lhs)),
          rhs_(static_cast<const RhsExpr&>(rhs)) {
        // 检查两个表达式的大小是否匹配
        if (lhs_.size() != rhs_.size()) {
            throw std::invalid_argument("向量大小不匹配");
        }
    }
    
    // 获取位置i的元素：左操作数[i] + 右操作数[i]
    double operator[](size_t i) const {
        return lhs_[i] + rhs_[i];
    }
    
    // 获取表达式的大小
    size_t size() const {
        return lhs_.size();
    }
    
private:
    const LhsExpr& lhs_;
    const RhsExpr& rhs_;
};

// 重载加法运算符
template<typename LhsExpr, typename RhsExpr>
VectorSum<LhsExpr, RhsExpr> operator+(
    const VectorExpression<LhsExpr>& lhs,
    const VectorExpression<RhsExpr>& rhs) {
    return VectorSum<LhsExpr, RhsExpr>(lhs, rhs);
}

// ========================
// 向量减法表达式
// ========================
template<typename LhsExpr, typename RhsExpr>
class VectorDifference : public VectorExpression<VectorDifference<LhsExpr, RhsExpr>> {
public:
    VectorDifference(const VectorExpression<LhsExpr>& lhs, 
                    const VectorExpression<RhsExpr>& rhs)
        : lhs_(static_cast<const LhsExpr&>(lhs)),
          rhs_(static_cast<const RhsExpr&>(rhs)) {
        // 检查两个表达式的大小是否匹配
        if (lhs_.size() != rhs_.size()) {
            throw std::invalid_argument("向量大小不匹配");
        }
    }
    
    // 获取位置i的元素：左操作数[i] - 右操作数[i]
    double operator[](size_t i) const {
        return lhs_[i] - rhs_[i];
    }
    
    // 获取表达式的大小
    size_t size() const {
        return lhs_.size();
    }
    
private:
    const LhsExpr& lhs_;
    const RhsExpr& rhs_;
};

// 重载减法运算符
template<typename LhsExpr, typename RhsExpr>
VectorDifference<LhsExpr, RhsExpr> operator-(
    const VectorExpression<LhsExpr>& lhs,
    const VectorExpression<RhsExpr>& rhs) {
    return VectorDifference<LhsExpr, RhsExpr>(lhs, rhs);
}

// ========================
// 向量与标量乘法表达式
// ========================
template<typename Expr>
class VectorScaled : public VectorExpression<VectorScaled<Expr>> {
public:
    VectorScaled(const VectorExpression<Expr>& expr, double scalar)
        : expr_(static_cast<const Expr&>(expr)), scalar_(scalar) {}
    
    // 获取位置i的元素：表达式[i] * 标量
    double operator[](size_t i) const {
        return expr_[i] * scalar_;
    }
    
    // 获取表达式的大小
    size_t size() const {
        return expr_.size();
    }
    
private:
    const Expr& expr_;
    double scalar_;
};

// 重载标量乘法运算符（向量 * 标量）
template<typename Expr>
VectorScaled<Expr> operator*(
    const VectorExpression<Expr>& expr,
    double scalar) {
    return VectorScaled<Expr>(expr, scalar);
}

// 重载标量乘法运算符（标量 * 向量）
template<typename Expr>
VectorScaled<Expr> operator*(
    double scalar,
    const VectorExpression<Expr>& expr) {
    return VectorScaled<Expr>(expr, scalar);
}

// ========================
// 向量应用函数表达式
// ========================
template<typename Expr, typename Func>
class VectorApply : public VectorExpression<VectorApply<Expr, Func>> {
public:
    VectorApply(const VectorExpression<Expr>& expr, Func func)
        : expr_(static_cast<const Expr&>(expr)), func_(func) {}
    
    // 获取位置i的元素：应用函数到表达式[i]
    double operator[](size_t i) const {
        return func_(expr_[i]);
    }
    
    // 获取表达式的大小
    size_t size() const {
        return expr_.size();
    }
    
private:
    const Expr& expr_;
    Func func_;
};

// 辅助函数，用于创建向量应用表达式
template<typename Expr, typename Func>
VectorApply<Expr, Func> apply(const VectorExpression<Expr>& expr, Func func) {
    return VectorApply<Expr, Func>(expr, func);
}

// 方便使用的常见数学函数
template<typename Expr>
auto sqrt(const VectorExpression<Expr>& expr) {
    return apply(expr, [](double x) { return std::sqrt(x); });
}

template<typename Expr>
auto abs(const VectorExpression<Expr>& expr) {
    return apply(expr, [](double x) { return std::abs(x); });
}

template<typename Expr>
auto square(const VectorExpression<Expr>& expr) {
    return apply(expr, [](double x) { return x * x; });
}

// ========================
// 性能计时器
// ========================
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

// ========================
// 辅助函数
// ========================

// 打印向量
template<typename T>
void printVector(const Vector<T>& vec, const std::string& name, size_t maxDisplay = 10) {
    std::cout << name << " = [";
    size_t count = 0;
    for (size_t i = 0; i < vec.size() && count < maxDisplay; ++i, ++count) {
        if (i > 0) std::cout << ", ";
        std::cout << vec[i];
    }
    if (vec.size() > maxDisplay) {
        std::cout << ", ...";
    }
    std::cout << "] (size: " << vec.size() << ")" << std::endl;
}

// 使用传统方法计算向量表达式
template<typename T>
Vector<T> traditionalAdd(const Vector<T>& a, const Vector<T>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("向量大小不匹配");
    }
    Vector<T> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}

template<typename T>
Vector<T> traditionalComplex(const Vector<T>& a, const Vector<T>& b, const Vector<T>& c, T scalar) {
    if (a.size() != b.size() || a.size() != c.size()) {
        throw std::invalid_argument("向量大小不匹配");
    }
    Vector<T> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] + b[i] * scalar - c[i];
    }
    return result;
}

// 条款44: 将与参数无关的代码抽离templates
// 测试函数，比较表达式模板与传统方法的性能
template<size_t Size>
void comparePerformance() {
    // 创建测试向量
    Vector<double> a(Size, 1.0);
    Vector<double> b(Size, 2.0);
    Vector<double> c(Size, 3.0);
    double scalar = 2.5;
    
    Vector<double> result1, result2;
    
    // 使用表达式模板
    {
        Timer t("表达式模板计算 (a + b * scalar - c)");
        result1 = a + b * scalar - c;
    }
    
    // 使用传统方法
    {
        Timer t("传统方法计算 (a + b * scalar - c)");
        result2 = traditionalComplex(a, b, c, scalar);
    }
    
    // 验证结果
    bool correct = true;
    for (size_t i = 0; i < Size && i < 10; ++i) {
        if (std::abs(result1[i] - result2[i]) > 1e-10) {
            correct = false;
            break;
        }
    }
    
    std::cout << "结果匹配: " << (correct ? "是" : "否") << std::endl;
}

} // namespace ExpressionTemplates

#endif // EXPRESSION_TEMPLATES_HPP