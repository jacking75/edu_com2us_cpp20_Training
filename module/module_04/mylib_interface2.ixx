
export module mylib:interface2;

// 클래스 S의 선언을 참조하기 위해 import 한다
import :interface1;

namespace mylib {

	// 프리 함수 export
	export void print_S(const S& s);
}