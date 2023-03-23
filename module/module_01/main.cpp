//https://zenn.dev/onihusube/articles/299ed7a3bc6062068fdd
// 간단한 모듈 사용 예

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