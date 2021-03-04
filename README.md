# 컴투스 서버 프로그래머 C++20 학습 및 실습
- [C++ compiler support](https://en.cppreference.com/w/cpp/compiler_support )
    - Visual C++ 19.28은 Visual Studio 16.9에 해당한다
    - [Visual C++ 버전 별 Visual Studio 버전 테이블](https://docs.microsoft.com/ko-kr/cpp/error-messages/compiler-warnings/compiler-warnings-by-compiler-version?view=msvc-160 )
    - [MS STL Changelog](https://github.com/microsoft/STL/wiki/Changelog )
- [modern-cpp-features](https://github.com/AnthonyCalandra/modern-cpp-features )
   
  
## 학습 자료
- 웹에서 공개된 자료를 참고하여 정리했다.
    - https://cpprefjp.github.io/
  
  
### Features Summary
- [C++20 Key Features Summary](https://www.slideshare.net/utilforever/c20-key-features-summary )
- [C++ Korea C++20 스터디 자료](https://github.com/CppKorea/Cpp20Study )
- [C++20 - 새로운 문법, 표준 라이브러리 기능 소개](https://blog.naver.com/kmc7468/221708166955 )
- [Marc Gregoire’s Blog - Slides of My Presentations at CppCon 2020](http://www.nuonsoft.com/blog/2020/10/22/slides-of-my-presentations-at-cppcon-2020/ )
- [Slides for Meeting C++ 2019: C++20: The small things](https://meetingcpp.com/mcpp/slides/2019/talk.pdf  )

    
  
  
### 작지만 유용한 기능들
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
  
  
  
### Lambda
- [제너릭 람다의 템플릿 구문](https://wandbox.org/permlink/8kcCAfMugsmqsyaF )
     - [Template Parameter for Lambdas](http://www.modernescpp.com/index.php/more-powerful-lambdas-with-c-20  ) 
- [상태를 가지지 않는 람다식은 default 생성, 대입 가능](https://wandbox.org/permlink/hdz8PseVtgk30BEP )
    - [예)std::map의 생성자에 비교 함수를 넘길 때 사용](https://wandbox.org/permlink/V6rSAIxF2rcoNaTq )
    
   
    
### Span 
- 작업 문서 링크할 예정    
	

### constexpr, consteval, constinit
- consteval: (2020.01.20) Visual C++은 Visual Studio 16.8에서 일부 구현
- constinit: (2020.01.20) 아직 미구현  
  
- [constexpr을 넘어선 consteval](https://blog.naver.com/nsun527/221834432741 )
- [constexpr, consteval, constinit](https://blog.naver.com/kmc7468/221705880457 )
- [(일어) constinit, consteval, constexpr, const 차이](https://gununu.hatenadiary.jp/entry/2019/10/15/020903   )  
- [정수식에서 가상함수 호출을 허가](https://wandbox.org/permlink/1fV60owHXyRWWk6Z )
  
	  
	  
### Chrono 
- [chrono Extensions in C++20](./chrono_extensionCpp20.md)
- [Calendar and Time-Zones in C++20: Time of Day](http://www.modernescpp.com/index.php/calendar-and-time-zone-in-c-20 )
  
  
  
### std::Bit 
- [bit](./bit.md)
      
    
	
### Concurrency
- Visual C++은 곧 출시 예정인 Visual Studio 16.9부터 지원 
  
- [Concurrency in C++20 and beyond (pdf)](https://github.com/CppCon/CppCon2019/blob/master/Presentations/concurrency_in_cpp20_and_beyond/concurrency_in_cpp20_and_beyond__anthony_williams__cppcon_2019.pdf  ) 
- [thread Extensions in C++20](./thread_extensionCpp20.md)
- [C++ Synchronized Buffered Ostream std::osyncstream](https://docs.google.com/document/d/1-XIeNmpZhY2fGl2L8ySQit0HlkmtnuH-hyr_FTDlD04/edit?usp=sharing  )
  
     
### std::Range
- (2020.01.20) Visual C++은 Visual Studio 16.8에서 대부분 구현   
  
- [Ranges](./ranges.md)


### Concepts  
- (2020.01.20) Visual C++은 Visual Studio 16.3에서 일부 구현
  
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
  
      
      
### module
- (working)C++ にモジュールがやってくる！   http://secret-garden.hatenablog.com/entry/2019/12/23/212339 
- [module 사용해보기](https://speedr00t.tistory.com/768 )
- [A Tour of C++ Modules in Visual Studio](https://devblogs.microsoft.com/cppblog/a-tour-of-cpp-modules-in-visual-studio/ )
- [Standard C++20 Modules support with MSVC in Visual Studio 2019 version 16.8](https://devblogs.microsoft.com/cppblog/standard-c20-modules-support-with-msvc-in-visual-studio-2019-version-16-8/ )
- Practical C++ Modules [pdf](https://github.com/CppCon/CppCon2019/blob/master/Presentations/practical_cpp_modules/practical_cpp_modules__boris_kolpackov__cppcon_2019.pdf )
- [Modules in VC++ 2019 16.5](https://mariusbancila.ro/blog/2020/05/07/modules-in-vc-2019-16-5/ )
- [C++20: A Simple math Module](http://www.modernescpp.com/index.php/cpp20-a-first-module )  
  
- (working) https://cpprefjp.github.io/lang/cpp20/modules.html  
- (working)モジュール理論 基礎編  https://onihusube.hatenablog.com/entry/2019/07/07/025446  
- (working)C++20 モジュールの概要 / Introduction to C++ modules (part 1) https://www.slideshare.net/TetsuroMatsumura/c20-152189285 
- (working)続・モジュール / Introduction to C++ modules (part 2) https://www.slideshare.net/TetsuroMatsumura/introduction-to-c-modules-part-2-225829715   
- (working)モジュールへの移行を考える 3 - 複数ヘッダファイル+単一ソースファイル  https://zenn.dev/onihusube/articles/98a0fce9827fa87a88d3    
- (working)C＋＋20のモジュールとincludeの違いについて詳しく調べてみた  https://logicalbeat.jp/blog/6223/ 
  
  
  
### std::format
(2021.01.14) 아직 컴파일러에서 구현하지 않았음  
  
- [printf 쓰지 마세요! std::format](https://karupro.tistory.com/12 )
- [std::format](https://blog.naver.com/kmc7468/221707892020 )
- [std::format 사용해보기(?)](https://blog.naver.com/nortul/222092094688 )
- [An Extraterrestrial Guide to C++20 Text Formatting](https://www.bfilipek.com/2020/02/extra-format-cpp20.html  )  
    
	
  	 
### Template
- [auto 파라메터에 의한 함ㅅ후 템플릿 간이 정의](https://wandbox.org/permlink/L4rPlrtsT6aK1d25 )
    - C++14에서 도입된 제너릭 람다처럼 보통 함수도 파라메터 타입을 auto로 하여 간단하게 함수 템플릿을 정의할 수 있다.
    - 아래의 제한이 있다.
        - 파라메터 타입에 decltype(auto)은 사용할 수 없다
        - `std::vector<auto>` 처럼 타입의 일부를 auto로 할 수 없다
    - auto로 정의한 변수의 타입을 취하고 싶을 때는 decltype을 사용한다.
- [(일어) 타입 이름으로 있는 것이 확실한 문맥에서 typename을 생략 가능하다](http://secret-garden.hatenablog.com/entry/2019/12/05/180904 )
  
  

### iterator
- [(일어) C++20에서 iterator 행동 조사 방법](https://onihusube.hatenablog.com/entry/2020/12/11/000123 )
- [(일어) C++20에서의 iterator_traits 사정](https://onihusube.hatenablog.com/entry/2020/12/14/002822  )
- [(일어) C++17 iterator <=> C++20 iterator != 0](https://onihusube.hatenablog.com/entry/2020/12/27/150400  )
  
  
  
### Coroutine
- [Coroutine ( 코루틴 ) - 1](https://openmynotepad.tistory.com/65 )
- [Coroutine ( 코루틴 ) - 2](https://openmynotepad.tistory.com/66 )
- [Coroutine ( 코루틴 ) - 3](https://openmynotepad.tistory.com/67 )
   

<br/>  
<br/>  
   
## 실습 
- [Modernizing legacy code](https://mariusbancila.ro/blog/2021/01/15/modernizing-legacy-code/ )  
- IOCP 네트워크 라이브러리: codes/NaveNet-ReCpp20
- 머드 게임: codes/mudmuhan-ReCpp20