//https://zenn.dev/onihusube/articles/0202ca564da2c3f5f98a

// �������̽��� 1���� ���Ͽ� ���� �κ��� ���� ���Ϸ� �� �� ����
// ���� ��Ƽ���� ������� �ʴ� ����


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