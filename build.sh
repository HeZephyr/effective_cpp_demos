#!/bin/bash

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 函数：打印带颜色的消息
print_message() {
  echo -e "${BLUE}[BUILD]${NC} $1"
}

print_success() {
  echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
  echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
  echo -e "${RED}[ERROR]${NC} $1"
}

# 函数：构建单个项目
build_project() {
  local project=$1
  
  print_message "开始构建项目: ${project}"
  
  # 检查项目目录是否存在
  if [ ! -d "${project}" ]; then
    print_error "找不到项目目录: ${project}"
    return 1
  fi
  
  # 创建构建目录
  mkdir -p "${project}/build"
  
  # 进入项目目录并执行构建
  (
    cd "${project}" && \
    print_message "执行 make clean..." && \
    make clean &>/dev/null && \
    print_message "执行 make..." && \
    make
  )
  
  # 检查构建结果
  if [ $? -eq 0 ]; then
    print_success "${project} 构建成功"
    return 0
  else
    print_error "${project} 构建失败"
    return 1
  fi
}

# 主函数
main() {
  echo "======================================"
  echo "Effective C++ 示例项目构建脚本"
  echo "======================================"
  
  # 定义所有项目
  local projects=(
    "smart_pointers"
    "raii_file_handler"
    "pimpl"
    "template_meta"
    "policy_mode"
    "thread_safe_singleton"
    "expression_templates"
  )
  
  # 计数器
  local success_count=0
  local fail_count=0
  
  # 构建所有项目
  for project in "${projects[@]}"; do
    echo ""
    echo "--------------------------------------"
    if build_project "${project}"; then
      ((success_count++))
    else
      ((fail_count++))
    fi
    echo "--------------------------------------"
  done
  
  # 打印构建摘要
  echo ""
  echo "======================================"
  echo "构建摘要:"
  echo "--------------------------------------"
  echo "总计: $((success_count + fail_count)) 个项目"
  echo -e "${GREEN}成功:${NC} ${success_count} 个项目"
  if [ ${fail_count} -gt 0 ]; then
    echo -e "${RED}失败:${NC} ${fail_count} 个项目"
  else
    echo -e "失败: 0 个项目"
  fi
  echo "======================================"
  
  # 返回状态码
  return ${fail_count}
}

# 执行主函数
main "$@"