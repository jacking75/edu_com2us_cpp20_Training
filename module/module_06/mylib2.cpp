
module; // �۷ι� ��� �÷��׸�Ʈ

// #include �� �۷ι� ��� �÷��׸�Ʈ �ȿ��� �Ѵ�
#include <iostream>

// mylib ��� ���� ����
module mylib;

namespace mylib {

    void print_S(const S& s) {
        std::cout << s.get() << std::endl;
    }
}