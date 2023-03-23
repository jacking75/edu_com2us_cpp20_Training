module;

#include <iostream>

// mylib. 을 붙이지 않아도 괜찮다.  export module mylib1; 해도 main.cpp에서 접근할 수 있다
export module mylib.mylib1;

namespace mylib {

    export class S {
        int m_num = 0;
    public:

        S() = default;
        S(int n) : m_num{ n } {}

        int get() const
        {
            return this->m_num;
        }
    };
}