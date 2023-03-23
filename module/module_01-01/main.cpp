// 모듈로된 STL 사용하기
// 프로젝트 설정에서 "C/C++" -> "언어" -> "실험용 C++ 표준 라이브러리 모듈 사용"을 "예"로한다

import std.core;
import std.memory;
import std.threading;

int main()
{
    std::thread t([] { std::cout << "func() called." << std::endl; });
    std::cout << "Main function" << std::endl;
    t.join();

    auto ptr = std::make_unique<int>(5);
    (*ptr)++;
    std::cout << "*ptr: " << *ptr << std::endl;
}