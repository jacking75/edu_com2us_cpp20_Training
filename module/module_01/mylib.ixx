export module mylib;  // mylib 모듈 인터페이스 단위 선언

namespace mylib { 

    // 클래스 export, 암묵적으로 모든 멤버가 export 된다
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