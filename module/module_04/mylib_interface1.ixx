
// mylib ����� �������̽� ��Ƽ�� ����
export module mylib:interface1;

namespace mylib {

    // Ŭ���� export, �Ϲ������� ��� ����� export �ȴ�
    export class S {
        int m_num = 0;
    public:

        S();
        S(int n);

        int get() const;
    };
}