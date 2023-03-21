//https://zenn.dev/onihusube/articles/299ed7a3bc6062068fdd

// ��� �ѹ��� export �ϱ�

export module mylib;  // mylib ��� �������̽� ���� ����

export namespace mylib { 

    // Ŭ���� export, �Ϲ������� ��� ����� export �ȴ�
    class S {
        int m_num = 0;

    public:
        S();
        S(int n);

        int get() const;
    };

    // ���� �Լ� export
    void print_S(const S& s);
}

// �Ʒ�ó�� export ������ ����� ����� ����
/*
export module mylib;  

namespace mylib {

  export {
     class S {
       int m_num = 0;
     public:

       S();
       S(int n);

       int get() const;
     };

     void print_S(const S& s);
  }

  // �� �Լ��� export ���� �ʴ´�
  void private_func(int);
}
*/