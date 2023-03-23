//https://zenn.dev/onihusube/articles/0202ca564da2c3f5f98a

// 인터페이스는 1개의 파일에 구현 부분은 복수 파일로 할 수 있음
// 구현 파티션을 사용하지 않는 형태


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