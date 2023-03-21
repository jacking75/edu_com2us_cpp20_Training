//https://zenn.dev/onihusube/articles/299ed7a3bc6062068fdd

// private 모듈 플래그먼트
// 모듈의 1개의 파일로 했을 때 외부에 공개하고 싶지 않은 부분(구현부 등)은 private 모듈 플래그먼트로 감싸서 숨길 수 있다.

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

// Visual Studio 2022(2023.03.21)에서는 아직 private 모듈 플래그먼트를 지원하지 않는다
// https://developercommunity.visualstudio.com/t/c-modules-change-in-private-module-triggers-recomp/1651834#T-N1660981
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