module; // �۷ι� ��� �÷��׸�Ʈ

// #include �� �۷ι� ��� �÷��׸�Ʈ ������ �Ѵ�
#include <iostream>

export module mylib;
// ������ʹ� �������̽�

namespace mylib {

    // Ŭ���� export,  �Ϲ������� ��� ����� export �ȴ�
    export class S {
        int m_num = 0;
    public:

        S();
        S(int n);

        int get() const;
    };

    // ���� �Լ� export
    export void print_S(const S& s);
}
// �������̽��� �������


private : module;
// ������ʹ� ����
       
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