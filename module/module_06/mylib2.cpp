
module; // 글로벌 모듈 플래그먼트

// #include 는 글로벌 모듈 플래그먼트 안에서 한다
#include <iostream>

// mylib 모듈 구현 단위
module mylib;

namespace mylib {

    void print_S(const S& s) {
        std::cout << s.get() << std::endl;
    }
}