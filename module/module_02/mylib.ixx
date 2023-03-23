module; // 글로벌 모듈 플래그먼트

// #include 는 글로벌 모듈 플래그먼트 내에서 한다
#include <iostream>

export module mylib;
// 여기부터는 인터페이스

namespace mylib {

    // 클래스 export,  암묵적으로 모든 멤버가 export 된다
    export class S {
        int m_num = 0;
    public:

        S();
        S(int n);

        int get() const;
    };

    // 프리 함수 export
    export void print_S(const S& s);
}
// 인터페이스는 여기까지


private : module;
// 여기부터는 구현
       
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