# module
  
## 모듈의 도입 이유  
프리프로세서(preprocessor, 전처리기)인 include는 헤더 파일의 내용을 그 장소에 전개한다. 이것은 아래와 같은 문제가 지적된다.    
  
1. 컴파일 시간이 길어진다.  
    헤더 파일 내용이 재귀적으로 전개되어, 프로그램이 길어진다(Hello world 만으로도 수만행에 달한다)  
    또 전개가 번역 단위마다 행하므로 전체로 보면 같은 헤더 파일을 몇 번이나 해석된다  
2. 프리프로세서의 상태에 의해 include 결과가 달라진다  
    include 순번에 따라서 에러가 발생할 수 있다  
3. 헤더 파일 내의 기술 영향을 너무 받는다   
    영향이 너무 커서 헤더 파일 내에 쓰는 것이 주저되는 것이 있다  
    using namespace 이나 매크로(예를들면 Windows 에서의 max) 등  
      
출처: https://cpprefjp.github.io/lang/cpp20/modules.html  
  
<br>       
  
## 얻는 혜택  
모듈은 전처리기를 사용하지 않고 프로그램을 분할할 수 있다. 즉 모듈은 `#include`와 같이 프리프로세서에서 헤더를 전개하여 종속성을 해결하는 것이 아니라 컴파일시 자동으로 종속성을 해결해 주는 것이다. 이렇게하면 `#include` 문제가 아래와 같이 해결된다.    
    
1. 컴파일 시간이 길어진다  
    번역 단위마다 헤더 파일을 전개하지 않기 때문에 어구 해석이 빨라진다. 그 결과, 일반적으로 모듈을 사용하는 것이 컴파일 시간이 짧아진다.  
2. 전 처리기의 상태에 따라 포함 결과가 변경된다.
3. 헤더 파일 내의 기술 영향을 너무 받는다   
    모듈에 기술한 `using namespace`나 매크로가 그 모듈을 사용하는 소스 파일에 누설되지 않게 된다. 이에 따라, 프리프로세서의 처리를 따르지 않으면 쉽게 발견할 수 없었던 버그가 발생하지 않게 된다.    
  
<br>  
   
## 추가 BMI 디렉터리
지시문에 전달된 import 이름을 확인하기 위해 검색할 하나 이상의 디렉터리를 지정한다.  
둘 이상이 있는 경우 세미콜론('';)을 사용하여 디렉터리를 구분한다.   
명령어로 할 때는 `/ifcSearchDir[path]` 컴파일러 옵션을 설정한다.  
    
![BMI(./images/001.png)    
   
    
## 참조
- [모듈에 대해서](https://docs.google.com/document/d/18USNv9vlIvuzURRjS44QzqdgM9mEicLm06FevA3FpfU/edit?usp=sharing )
- [모듈(module)](https://kukuta.tistory.com/389 )
- C++20 Modules 소개와 사용 - [1](https://openmynotepad.tistory.com/78)   [2](https://openmynotepad.tistory.com/79)
- [당신이 module을 써야만 하는 이유](https://kukuta.tistory.com/393 )
- [MS Docs C++ 모듈 개요](https://docs.microsoft.com/ko-kr/cpp/cpp/modules-cpp?view=msvc-160)
- [A Tour of C++ Modules in Visual Studio](https://devblogs.microsoft.com/cppblog/a-tour-of-cpp-modules-in-visual-studio/ ) 
    - 다른 디렉토리에 있는, 정적 라이브러리에 있는 모듈을 참조할 수 있는 방법 설명  
- [Standard C++20 Modules support with MSVC in Visual Studio 2019 version 16.8](https://devblogs.microsoft.com/cppblog/standard-c20-modules-support-with-msvc-in-visual-studio-2019-version-16-8/ )
- Practical C++ Modules [pdf](https://github.com/CppCon/CppCon2019/blob/master/Presentations/practical_cpp_modules/practical_cpp_modules__boris_kolpackov__cppcon_2019.pdf )
- [Modules in VC++ 2019 16.5](https://mariusbancila.ro/blog/2020/05/07/modules-in-vc-2019-16-5/ )
- [C++20: A Simple math Module](http://www.modernescpp.com/index.php/cpp20-a-first-module )  
- [(일어)C++20의 모듈과 include의 차이에 대해서 자세히 조사해 보았다](https://logicalbeat.jp/blog/6223/) 
- [(일어) 모듈 이론 기초편](https://onihusube.hatenablog.com/entry/2019/07/07/025446  )  
- (일어) C++ 모듈 인터페이스 파티션 사용처: [1](https://zenn.dev/uyamae/articles/0acaee53777832 )  [2](https://zenn.dev/uyamae/articles/51074bc5cff43b )
- [Example project for C++20 module support in CMake](https://github.com/calgray/cmake-cpp-modules-template )
- [Moving a project to C++ named Modules](https://devblogs.microsoft.com/cppblog/moving-a-project-to-cpp-named-modules/ )
   
