//https://zenn.dev/onihusube/articles/299ed7a3bc6062068fdd


export module mylib;  // mylib ��� �������̽� ���� ����

namespace mylib { 

    // Ŭ���� export, �Ϲ������� ��� ����� export �ȴ�
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