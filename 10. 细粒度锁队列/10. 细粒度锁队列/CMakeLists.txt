# CMakeList.txt: 10. 细粒度锁队列 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 将源代码添加到此项目的可执行文件。
add_executable (CMakeTarget "细粒度锁数组操作.hpp" "细粒度锁队列.hpp" "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。

set(VLD_INCLUDE_DIRS "F:\\vld\\Visual Leak Detector\\include")
set(VLD_LIBRARIES "F:\\vld\\Visual Leak Detector\\lib\\Win64\\vld.lib")
include_directories(${VLD_INCLUDE_DIRS})
target_link_libraries(CMakeTarget PRIVATE  ${VLD_LIBRARIES})