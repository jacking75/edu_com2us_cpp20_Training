

export module mylib;  // mylib 모듈 인터페이스 단위 선언

export namespace mylib { 

    // 클래스 export, 암묵적으로 모든 멤버가 export 된다
    class S {
        int m_num = 0;

    public:
        S();
        S(int n);

        int get() const;
    };

    // 프리 함수 export
    void print_S(const S& s);
}

// 아래처럼 export 구역을 만드는 방법도 있음
/*
export module mylib;  

namespace mylib {

  export {
     class S {
       int m_num = 0;
     public:

       S();
       S(int n);

       int get() const;
     };

     void print_S(const S& s);
  }

  // 이 함수는 export 되지 않는다
  void private_func(int);
}
*/