//https://zenn.dev/onihusube/articles/299ed7a3bc6062068fdd

// private ��� �÷��׸�Ʈ
// ����� 1���� ���Ϸ� ���� �� �ܺο� �����ϰ� ���� ���� �κ�(������ ��)�� private ��� �÷��׸�Ʈ�� ���μ� ���� �� �ִ�.

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

// Visual Studio 2022(2023.03.21)������ ���� private ��� �÷��׸�Ʈ�� �������� �ʴ´�
// https://developercommunity.visualstudio.com/t/c-modules-change-in-private-module-triggers-recomp/1651834#T-N1660981
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