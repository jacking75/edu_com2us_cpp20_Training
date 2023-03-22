module; // 글로벌 모듈 플래그먼트

#include <iostream>

// mylib 모듈의 구현 파티션 2
module mylib:part2;

// 인터페이스 import
import mylib;

namespace mylib {

    void print_S(const S& s) {
        std::cout << s.get() << std::endl;
    }
}