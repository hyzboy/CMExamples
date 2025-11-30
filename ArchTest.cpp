#include <iostream>
#include <string>

int main() {
    std::cout << "Testing CPU architecture detection..." << std::endl;

    // 简单的编译测试
    std::cout << "Architecture separation compilation test passed!" << std::endl;

    // 检查CMAKE_SYSTEM_PROCESSOR
    std::cout << "CMAKE_SYSTEM_PROCESSOR: ";
#ifdef CMAKE_SYSTEM_PROCESSOR
    std::cout << CMAKE_SYSTEM_PROCESSOR << std::endl;
#else
    std::cout << "Not defined" << std::endl;
#endif

    return 0;
}