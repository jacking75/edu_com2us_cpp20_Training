module;

#include <iostream>

export module mylib.mylib2;

// 클래스 S의 선언을 참조하기 위해 import 한다
import mylib.mylib1;

namespace mylib {

	// 프리 함수 export
	export void print_S(const S& s)
	{
		std::cout << s.get() << std::endl;
	}
}