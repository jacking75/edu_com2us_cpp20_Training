//https://zenn.dev/onihusube/articles/98a0fce9827fa87a88d3

// 인터페이스 파일은 1개만 있을 수 있다
// 다만 인터페이스를 몇개의 파일로 나누고 싶을 때는 파티션 으로 분할 할 수 있다.
// 같은 모듈에 속해 있는 인터페이스 파티션은 모두 프라이머리 인터페이스 단위에서 export 해야 한다


import mylib; // mylib 모듈 import 선언

int main() 
{
	// mylib은 모듈의 이름이 아니고 import한 모듈의 네임스페이스 이름이다.
	mylib::S s1{};
	mylib::S s2{ 20 };

	mylib::print_S(s1); // 0
	mylib::print_S(s2); // 20

	return 0;
}