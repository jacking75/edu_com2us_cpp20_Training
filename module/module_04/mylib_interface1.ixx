
// mylib 모듈의 인터페이스 파티션 선언
export module mylib:interface1;

namespace mylib {

    // 클래스 export, 암무적으로 모든 멤버가 export 된다
    export class S {
        int m_num = 0;
    public:

        S();
        S(int n);

        int get() const;
    };
}