module;

#include <iostream>

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