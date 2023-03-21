module; // 글로벌 모듈 플래그먼트

// #include 는 글로버 모듈 플래그먼트 내에서만 선언해야 한다
#include <iostream>

module mylib;  // mylib 모듈의 구현 단위 선언
// mylib 모듈 인터페이스를 암묵적으로 import 하고 있다

// 네이스페이스와 모듈의 이름을 서로 달라도 괜찮다
namespace mylib {

    S::S() = default;
    S::S(int n) : m_num{ n } {}

    inline int S::get() const {
        return this->m_num;
    }

    void print_S(const S& s) {
        std::cout << s.get() << std::endl;
    }
}