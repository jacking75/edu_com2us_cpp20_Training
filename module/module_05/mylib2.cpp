module; // �۷ι� ��� �÷��׸�Ʈ

#include <iostream>

// mylib ����� ���� ��Ƽ�� 2
module mylib:part2;

// �������̽� import
import mylib;

namespace mylib {

    void print_S(const S& s) {
        std::cout << s.get() << std::endl;
    }
}