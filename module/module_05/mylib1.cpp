// mylib ����� ���� ��Ƽ�� 1
module mylib:part1;

// �������̽� import
import mylib;

namespace mylib {

    S::S() = default;
    S::S(int n) : m_num{ n } {}

    inline int S::get() const {
        return this->m_num;
    }
}