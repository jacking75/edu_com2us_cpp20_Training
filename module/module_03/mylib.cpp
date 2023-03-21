module; // �۷ι� ��� �÷��׸�Ʈ

// #include �� �۷ι� ��� �÷��׸�Ʈ �������� �����ؾ� �Ѵ�
#include <iostream>

module mylib;  // mylib ����� ���� ���� ����
// mylib ��� �������̽��� �Ϲ������� import �ϰ� �ִ�

// ���̽����̽��� ����� �̸��� ���� �޶� ������
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