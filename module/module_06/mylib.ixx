//https://zenn.dev/onihusube/articles/0202ca564da2c3f5f98a

// 인터페이스는 1개의 파일에 구현 부분은 복수 파일로 할 수 있음
// 구현 파티션을 사용하지 않는 형태


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