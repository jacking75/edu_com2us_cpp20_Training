// mylib ��� ���� ����
module mylib;

namespace mylib {

    S::S() = default;
    S::S(int n) : m_num{ n } {}

    int S::get() const {
        return this->m_num;
    }
}