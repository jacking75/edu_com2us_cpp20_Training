# 컴투스 서버 프로그래머 C++20 학습 및 실습
- [C++ compiler support](https://en.cppreference.com/w/cpp/compiler_support )
    - Visual C++ 19.28은 Visual Studio 16.9에 해당한다
    - [Visual C++ 버전 별 Visual Studio 버전 테이블](https://docs.microsoft.com/ko-kr/cpp/error-messages/compiler-warnings/compiler-warnings-by-compiler-version?view=msvc-160 )
    - [MS STL Changelog](https://github.com/microsoft/STL/wiki/Changelog )
- [modern-cpp-features](https://github.com/AnthonyCalandra/modern-cpp-features )
   
  
## 학습 자료
- 웹에서 공개된 자료를 참고하여 정리했다.
    - https://cpprefjp.github.io/
  
  
## Features Summary  
- [C++20 Key Features Summary](https://www.slideshare.net/utilforever/c20-key-features-summary )
- [C++ Korea C++20 스터디 자료](https://github.com/CppKorea/Cpp20Study )
- [C++20 - 새로운 문법, 표준 라이브러리 기능 소개](https://blog.naver.com/kmc7468/221708166955 )
- [Marc Gregoire’s Blog - Slides of My Presentations at CppCon 2020](http://www.nuonsoft.com/blog/2020/10/22/slides-of-my-presentations-at-cppcon-2020/ )
- [Slides for Meeting C++ 2019: C++20: The small things](https://meetingcpp.com/mcpp/slides/2019/talk.pdf  )
- [All C++20 core language features with examples](https://oleksandrkvl.github.io/2021/04/02/cpp-20-overview.html )

    
  
  
## 작지만 유용한 기능들
- [Designated Initializer ( 지정된 초기화 )](https://openmynotepad.tistory.com/64 )
- [수학 상수](https://blog.naver.com/kmc7468/221843531555 )  
- [선택문 최적화를 위한 [[likely]], [[unlikely]] ](https://blog.naver.com/nsun527/221834414761 )
- [우주선 연산자에 대해 연구를 해 보았다](https://blog.naver.com/shk5646/222105348582 )
- [<=> 연산자 탐방](https://blog.naver.com/nsun527/221834414761)

- [new 식에서 배열 요소 수 추론](https://wandbox.org/permlink/V3cM89ZFnejoQuBp )
- [scope 있는 열거 타입의 using 선언](https://wandbox.org/permlink/CgjcBMhZEsoCWft5 ) 
    - namespace 처럼 `using enum EnumType;` 혹은 `using EnumType::enumerator` 로 하면 열거 값의 scpoe 지정을 생략 할 수 있다  
- [소스 코드의 위치 정보를 반환하는 `std::source_location`](https://wandbox.org/permlink/FdbTYOnyvTERIkql )  
    - `source_location`의 멤버 함수는 constexpr로 선언 되어 있음. 즉 컴파일 시점에 계산된다.
- [안전한 정수 타입 비교](https://wandbox.org/permlink/GVE2g4GWu7TCVTHw )
    - `std::cmp_equal`, `std::cmp_not_equal`, `std::cmp_less`, `std::cmp_greater`, `std::cmp_less_equal`, `std::cmp_greater_equal`, `std::in_range`
- [starts_with: 지정한 부분 문자열로 시작하는지 조사](https://wandbox.org/permlink/A4mesAibDhcBCpHf ) 
- [ends_with: 자종헌 부분 뮨자열로 끝나는지 조사](https://wandbox.org/permlink/A4mesAibDhcBCpHf )  
- [`std::to_array`: 1차원 배열에서 std::array를 만든다](https://wandbox.org/permlink/4pshkvbAFkqECYRu )
- [`std::ssize`: 컨테이너(STL 컨테이너와 일반 배열) 요소 수를 부호 있는 정수 타입으로 얻는다](https://wandbox.org/permlink/XZ4rjECx5Z4FeQ2T )
    - `size_t`는 구현에 의존하지만 C++11 부터는 부호 없는 정수 타입으로 선언되어 있음
- [`std::endian`: 바이트 순서(littel, big)을 나타내는 열거 타입](https://wandbox.org/permlink/lR5OtazkOcrFxgY6 )
- [range based for 문에서 초기화 식 사용](https://wandbox.org/permlink/hZ7YpiRvrDyx5Utq )
    - `for (auto thing = foo(); auto& x : thing.items()) { } `
- [UTF-8 엔코딩된 문자열 타입으로 `char8_t` 추가](https://wandbox.org/permlink/V4xPCQvgJNPIk1Nk)
    - UTF-8로 엔코딩된 문자여을 저장하는 것을 생각한 타입으로 부호 없는 문자열 타입 char8_t 타입이다
    - `char8_t` 타입은 `unsigned char` 타입과 같은 크기, 얼라이먼트, 정수 변환 순위이지만 독립된 타입이고, `char` 나 `unsigned char` 와 오버로드로 구별된다
    - `u8` 플레이스 홀드가 붙은 문자열/(raw) 문자열 리터럴 타입도 `char/const char[n]`에서 `char8_t/const char8_t [n]`로 변경된다.
- [`std::is_constant_evaluated()`: 컴파일 시에 평가 되는 여부를 판별하는 함수](https://wandbox.org/permlink/VhaiQ30YCcav5Zj1 )
    - 컴파일 시에 평가 되는 경우 true를 반환, 실행 시에 평가 되는 경우는 false를 반환.
- [`contains` 연상 컨테이너에 어떤 요소가 있는가를 판정하는 멤버 함수](https://wandbox.org/permlink/t73hAgOVsbJEV0wp )
- [소괄호의 값 리스트에서 집성체(구조체, 배열) 초기화 허가](https://wandbox.org/permlink/BTJJE1m5GQ1omOCi )  
- [(일어) 우주선 연산자의 폴백 처리](https://onihusube.hatenablog.com/entry/2019/11/17/014040 )
- [(일어) friend 지정된 함수 낸에서 구조화 속박을 상요하여 비 공개 멤버 변수에 접근 하는 것을 허용](http://secret-garden.hatenablog.com/entry/2019/12/04/194134 )  
- [(일어) range based for이 커스텀 마이션 포인트를 발견하는 rule을 완화](https://cpprefjp.github.io/lang/cpp20/relaxing_the_range_for_loop_customization_point_finding_rules.html )
- [C++20: Heterogeneous Lookup in (Un)ordered Containers](https://www.cppstories.com/2021/heterogeneous-access-cpp20 )
  
  
  
## Lambda
- [제너릭 람다의 템플릿 구문](https://wandbox.org/permlink/8kcCAfMugsmqsyaF )
     - [Template Parameter for Lambdas](http://www.modernescpp.com/index.php/more-powerful-lambdas-with-c-20  ) 
- [상태를 가지지 않는 람다식은 default 생성, 대입 가능](https://wandbox.org/permlink/hdz8PseVtgk30BEP )
    - [예)std::map의 생성자에 비교 함수를 넘길 때 사용](https://wandbox.org/permlink/V6rSAIxF2rcoNaTq )
    
   
    
## Span 
- 메모리의 일부분을 소유권은 없이, 참조하는 클래스
    - 배열이나 std::vector의 일부 연속적인 요소를 추출하고, 이 요소들에만 처리를 적용하는 목적으로 사용한다.
- 비슷한 것으로 std::basic_string_view 가 있지만 이것은 문자열에 특화된 것이고, span은 메모리 연속성을 가진 모든 컨테이너에 사용할 수 있다.
    - 메모리 연속성이 없는 list, unordered_map 등은 사용 불가
- [설명 문서](https://docs.google.com/presentation/d/1bC_V1_wQsRL-N7igEc_JH_pWqfXMqmZkUMEYF9WKwZ0/edit?usp=sharing) 
	 
  
## constexpr, consteval, constinit
- [constexpr을 넘어선 consteval](https://blog.naver.com/nsun527/221834432741 )
- [constexpr, consteval, constinit](https://blog.naver.com/kmc7468/221705880457 )
- [(일어) constinit, consteval, constexpr, const 차이](https://gununu.hatenadiary.jp/entry/2019/10/15/020903   )  
- [정수식에서 가상함수 호출을 허가](https://wandbox.org/permlink/1fV60owHXyRWWk6Z )
  
	  
	  
## Chrono 
- [chrono Extensions in C++20](./chrono_extensionCpp20.md)
- [Calendar and Time-Zones in C++20: Time of Day](http://www.modernescpp.com/index.php/calendar-and-time-zone-in-c-20 )
- [(일어) 시간 계산과 달력 계산은 다르다! 완전 다르다!](https://qiita.com/yohhoy/items/d1f9004c66cc6ff15774 )
  
  
  
## std::Bit 
- [bit](./bit.md)
      
    
	
## Concurrency
- [Concurrency in C++20 and beyond (pdf)](https://github.com/CppCon/CppCon2019/blob/master/Presentations/concurrency_in_cpp20_and_beyond/concurrency_in_cpp20_and_beyond__anthony_williams__cppcon_2019.pdf  ) 
- [thread Extensions in C++20](./thread_extensionCpp20.md)
- [C++ Synchronized Buffered Ostream std::osyncstream](https://docs.google.com/document/d/1-XIeNmpZhY2fGl2L8ySQit0HlkmtnuH-hyr_FTDlD04/edit?usp=sharing  )
  
     
## [std::Range](./ranges.md)
 
  
  
## Concepts  
- [Concepts ( 콘셉트, 개념 ) - 1](https://openmynotepad.tistory.com/69 )
- [Concepts ( 콘셉트, 개념 ) - 2](https://openmynotepad.tistory.com/70 )
- [Concepts ( 콘셉트, 개념 ) - 3](https://openmynotepad.tistory.com/71 )
- [Concepts ( 콘셉트, 개념 ) - 4](https://openmynotepad.tistory.com/72 )
- [Concepts 알아보기 (1)](https://blog.naver.com/nsun527/221760493147 )
- [concept를 이용한 타입 오버플로우 체크](https://blog.naver.com/wkdghcjf1234/222183575154 )
- [(일어) concept 개념과 사용법](https://cpprefjp.github.io/lang/cpp20/concepts.html )
- (일어)C++20 concept 이전 입문]  https://qiita.com/yohhoy/items/f3d90c598348817cd29c 
- Meeting C++ 2019: Concepts in C++20: A Evolution or a Revolution? [pdf](https://meetingcpp.com/mcpp/slides/2019/conceptsBerlin.pdf )
- [Intro to C++20's Concepts](https://github.com/hniemeyer/IntroToConcepts )
- [C++템플릿 프로그래밍 SFINAE을 이해하기](https://docs.google.com/document/d/12D1cAFHhgXipn20Gg2WVlZiZN8Rl_2qELiwESABXgVM/edit?usp=sharing )
        
  
<br>    
<br>    
  
## std::format
  
### 도입
- Visual Studio 2019 16.10.0 이상 필요
- C++ 프로젝트의 속성에서 "C++ 표준"을 "미리보기 - 최신 C++ Working Draft의 기능 (/std:c++ latest)"로 설정
  
  
### 기본
라이브러리 `format`를 포함하면 사용할 수 있다.   
```
#include <iostream>
#include <format>

int main()
{
    using namespace std;
    cout << format("Hello {}!", "std::format") << endl;
}
```  
<pre>  
Hello std::format!
</pre>    
    
  
## 예외
`std::format`은 형식이나 인수가 잘못된 경우 런타임 예외 `std::format_error`를 발생시킨다.  
```
try {
    // 1개만 인수가 있는데 두 번째를 사용하려고 하면
    cout << format("{1}", 0) << endl;
}
catch (const format_error& e)
{
    cout << e.what() << endl;
}
```
↓  
Argument not found.
  
  
- 서식 문자열이 잘못된 경우에도 예외가 발생한다.
    ```
    format("{$324i0-g}", 0)
    ```  
    Invalid format string.
- 서식에서 지정한 형식과 인수의 타입이 다른 경우도 예외가 발생한다.
    ```
    format("{:d}", 4.0f)
    ```  
    invalid floating point type   
    
    ```  
    format("{:f}", 42)
    ```  
    invalid integral type  
- fill 지정에 `{` 나 `}` 를 사용하면 오류가 발생한다 (다른 문자는 사용 가능).
    ```
    format("{:{>10}", 42)
    ```  
    invalid fill character '{'   
    
    ```  
    format("{:}>10}", 42)
    ```  
    Unmatched '}' in format string.  
- 부동 소수점 숫자 이외에서 precision 지정은 할 수 없다.  
    ```
    format("{:6.3}", 42)
    ```  
    Precision not allowed for this argument type.
    

## 와이드 문자열
물론 wchar_t 및 wstring에 대응하고 있기 때문에, 영어 이외도 처리할 수 있다   
```
#include <iostream>
#include <format>
#include <locale>

int main()
{
    using namespace std;
    setlocale(LC_CTYPE, "");
    wcout << format(L"{}", L"안녕！") << endl;
}
```  
안녕！

  
fill 지정에도 와이드 문자는 사용할 수 있지만, 기대한 동작은 되지 않는 것 같다.  
```
wcout << format(L"{:あ^20}", L"中央寄せ") << endl;
```  
ああああああ中央寄せああああああ  // 'あ'が幅1として扱われている？(2*4+12=20)

<br>  
      
## 서식 지정 치트 시트
|     설명                                     |     기술                                |     결과            |
|----------------------------------------------|-----------------------------------------|---------------------|
|     1개                                      |     format("{}",   1)                   |     1               |
|     2개                                      |     format("{}   {}", 1, 2)             |     1 2             |
|     3개                                      |     format("{}   {} {}", 1, 2, 3)       |     1 2   3         |
|     순서 지정                                |     format("{2}   {1} {0}", 1, 2, 3)    |     3 2   1         |
|     문자(c는 생략 가능)                       |     format("{:c}",   'C')               |     C               |
|     문자열(s는 생략 가능)                        |     format("{:s}",   "C++")             |     C++             |
|     bool                                     |     format("{}",   true)                |     true            |
|     bool                                     |     format("{}",   false)               |     false           |
|     정수(10진수,d생략 가능)                   |     format("{:d}",   42)                |     42              |
|     정수(8진수)                              |     format("{:o}",   042)               |     42              |
|     정수(16진수)                             |     format("{:x}",   0xab)              |     ab              |
|     정수(16진수) 대문자                      |     format("{:X}",   0xab)              |     AB              |
|     정수(16진수)   0x 붙음                    |     format("{:#x}",   0xab)             |     0xab            |
|     정수(16진수)   0X 붙음                    |     format("{:#X}",   0xab)             |     0XAB            |
|     정수(2진수)                              |     format("{:b}",   0b10101010)        |     10101010        |
|     정수(2진수)   0b 붙음                     |     format("{:#b}",   0b10101010)       |     0b10101010      |
|     정수(2진수)   0B 붙음                     |     format("{:#B}",   0b10101010)       |     0B10101010      |
|     정수(정수에+)                            |     format("{:+d}",   42)               |     +42             |
|     부동소수점수                             |     format("{:f}",   123.456789)        |     123.456789      |
|     정도 지정                                 |     format("{:6.3f}",   123.456789)     |     123.457         |
|     지수 표기(e)                              |     format("{:e}",   123.456789)        |     1.234568e+02    |
|     지수 표기(E)                              |     format("{:E}",   123.456789)        |     1.234568E+02    |
|     최적의 표기를 자동 판정  (g는 생략 가능)    |     format("{:g}",   123.456789)        |     123.457         |
|     왼쪽 정렬                                   |     format("{:<12}",   "left")          |     left            |
|     오른쪽 정렬                                   |     format("{:>12}",   "right")         |            right    |
|     중앙 정렬                                 |     format("{:^12}",   "center")        |        center       |
|     묻힐 문자를 지정한다                    |     format("{:!^12}",   "hello")        |     !!!hello!!!!    |
     	
        
- [fmtlib과 std::format](https://docs.google.com/presentation/d/1F2B55zB34rswqFDMBWCKxLazdkEPhDdN9qmyD4PcbbU/edit?usp=sharing )
- [printf 쓰지 마세요! std::format](https://karupro.tistory.com/12 )
- [std::format](https://blog.naver.com/kmc7468/221707892020 )
- [std::format 사용해보기(?)](https://blog.naver.com/nortul/222092094688 )
- [An Extraterrestrial Guide to C++20 Text Formatting](https://www.bfilipek.com/2020/02/extra-format-cpp20.html  )  
- [(일어) C++20 문자열 포맷 라이브러리 std::format](https://qiita.com/tetsurom/items/e660c7aaf008d8a1e904)
      
  
<br>    
<br>    
    	 
## Template
- [auto 파라메터에 의한 함수 템플릿 간이 정의](https://wandbox.org/permlink/L4rPlrtsT6aK1d25 )
    - C++14에서 도입된 제너릭 람다처럼 보통 함수도 파라메터 타입을 auto로 하여 간단하게 함수 템플릿을 정의할 수 있다.
    - 아래의 제한이 있다.
        - 파라메터 타입에 decltype(auto)은 사용할 수 없다
        - `std::vector<auto>` 처럼 타입의 일부를 auto로 할 수 없다
    - auto로 정의한 변수의 타입을 취하고 싶을 때는 decltype을 사용한다.
- [(일어) 타입 이름으로 있는 것이 확실한 문맥에서 typename을 생략 가능하다](http://secret-garden.hatenablog.com/entry/2019/12/05/180904 )
  
<br>    
<br>            

## iterator
- [(일어) C++20에서 iterator 행동 조사 방법](https://onihusube.hatenablog.com/entry/2020/12/11/000123 )
- [(일어) C++20에서의 iterator_traits 사정](https://onihusube.hatenablog.com/entry/2020/12/14/002822  )
- [(일어) C++17 iterator <=> C++20 iterator != 0](https://onihusube.hatenablog.com/entry/2020/12/27/150400  )
  
<br>    
<br>            
  
## Coroutine
- [Coroutine ( 코루틴 ) - 1](https://openmynotepad.tistory.com/65 )
- [Coroutine ( 코루틴 ) - 2](https://openmynotepad.tistory.com/66 )
- [Coroutine ( 코루틴 ) - 3](https://openmynotepad.tistory.com/67 )
     

<br>    
<br>          
      
## [module](./READEME.md)
    
<br/>  
<br/>  
   
## 활용 
- [Writing a simple logging function in C++20](https://mariusbancila.ro/blog/2021/07/03/writing-a-simple-logging-function-in-c20/  )
  
      
<br/>  
<br/>  
   
## 실습 
- [Modernizing legacy code](https://mariusbancila.ro/blog/2021/01/15/modernizing-legacy-code/ )  
- IOCP 네트워크 라이브러리: codes/NaveNet-ReCpp20
- 머드 게임: codes/mudmuhan-ReCpp20