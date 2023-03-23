//https://zenn.dev/onihusube/articles/299ed7a3bc6062068fdd

// private 모듈 플래그먼트
// 모듈의 1개의 파일로 했을 때 외부에 공개하고 싶지 않은 부분(구현부 등)은 private 모듈 플래그먼트로 감싸서 숨길 수 있다.

// 이 예제는 빌드되지 않는다
// Visual Studio 2022(2023.03.21)에서는 아직 private 모듈 플래그먼트를 지원하지 않는다
// https://developercommunity.visualstudio.com/t/c-modules-change-in-private-module-triggers-recomp/1651834#T-N1660981

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