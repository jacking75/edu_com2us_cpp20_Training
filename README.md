# 컴투스 서버 프로그래머 C++20 학습 및 실습

## 학습 자료
  
### Features Summary
- [C++20 Key Features Summary](https://www.slideshare.net/utilforever/c20-key-features-summary )
- [C++ Korea C++20 스터디 자료](https://github.com/CppKorea/Cpp20Study )
- [C++20 - 새로운 문법, 표준 라이브러리 기능 소개](https://blog.naver.com/kmc7468/221708166955 )
- [Marc Gregoire’s Blog - Slides of My Presentations at CppCon 2020](http://www.nuonsoft.com/blog/2020/10/22/slides-of-my-presentations-at-cppcon-2020/ )
- [Slides for Meeting C++ 2019: C++20: The small things](https://meetingcpp.com/mcpp/slides/2019/talk.pdf  )

    
  
  
### 작은 언어 기능들
- [Designated Initializer ( 지정된 초기화 )](https://openmynotepad.tistory.com/64 )
- [수학 상수](https://blog.naver.com/kmc7468/221843531555 )  
- [선택문 최적화를 위한 [[likely]], [[unlikely]] ](https://blog.naver.com/nsun527/221834414761 )
- [우주선 연산자에대해 연구를 해보았다](https://blog.naver.com/shk5646/222105348582 )
- [<=> 연산자 탐방](https://blog.naver.com/nsun527/221834414761)
- [(일어) 우주선 연산자의 폴백 처리](https://onihusube.hatenablog.com/entry/2019/11/17/014040 )
- [(일어) friend 지정된 함수 낸에서 구조화 속박을 상요하여 비 공개 멤버 변수에 접근 하는 것을 허용](http://secret-garden.hatenablog.com/entry/2019/12/04/194134 ) 
- [new 식에서 배열 요소 수 추론](https://wandbox.org/permlink/V3cM89ZFnejoQuBp )
  
(working) スコープ付き列挙型のusing宣言
https://cpprefjp.github.io/lang/cpp20/using_enum.html

(working) ソースコードの位置情報を返すライブラリを追加 
http://secret-garden.hatenablog.com/entry/2019/12/13/212815 

(working) C++20便利機能の紹介：文字列チェック三銃士(2/3) starts_with, ends_with
https://qiita.com/yohhoy/items/c86916ac9e86928eb147 

(working) 整数型の比較 in C++20
https://zenn.dev/onihusube/articles/bde8678e96918484e6d9 

(working) std::to_array
https://cpprefjp.github.io/reference/array/to_array.html

(working) std::ssize
https://cpprefjp.github.io/reference/iterator/ssize.html

(working) std::endian
https://cpprefjp.github.io/reference/bit/endian.html

(working) new式での配列要素数の推論
https://cpprefjp.github.io/lang/cpp20/array_size_deduction_in_new-expressions.html

(working) 定数式からの仮想関数の呼び出しを許可 
http://secret-garden.hatenablog.com/entry/2019/12/06/232434 

(working) autoパラメータによる関数テンプレートの簡易定義
https://cpprefjp.github.io/lang/cpp20/function_templates_with_auto_parameters.html

(working) 確率が高い分岐と低い分岐を伝える属性 [[likely]], [[unlikely]]
https://cpprefjp.github.io/lang/cpp20/likely_and_unlikely_attributes.html

(working) 範囲for文がカスタマイゼーションポイントを見つけるルールを緩和
https://cpprefjp.github.io/lang/cpp20/relaxing_the_range_for_loop_customization_point_finding_rules.html

(working) 初期化式をともなう範囲for文
https://cpprefjp.github.io/lang/cpp20/range-based_for_statements_with_initializer.html

(working) std::source_location
https://cpprefjp.github.io/reference/source_location/source_location.html
  
(working) 可変引数が空でない場合のトークン置換
https://cpprefjp.github.io/lang/cpp20/va_opt.html

(working) compare
https://cpprefjp.github.io/reference/compare.html
  
(working) UTF-8エンコーディングされた文字の型としてchar8_tを追加
https://cpprefjp.github.io/lang/cpp20/char8_t.html
    
(working) コンパイル時に評価されたかどうかを判別出来る関数を追加 
http://secret-garden.hatenablog.com/entry/2019/12/11/212905

(working) 連想コンテナにある要素が含まれているかを判定するメンバ関数を追加 
http://secret-garden.hatenablog.com/entry/2019/12/12/000143 
  
(working) 丸カッコの値リストからの集成体初期化を許可
https://cpprefjp.github.io/lang/cpp20/allow_initializing_aggregates_from_a_parenthesized_list_of_values.html
  

  
### Lambda
Template Parameter for Lambdas  http://www.modernescpp.com/index.php/more-powerful-lambdas-with-c-20   
状態を持たないラムダ式がデフォルト構築可能、代入可能となった  http://secret-garden.hatenablog.com/entry/2019/12/24/000739   
ジェネリックラムダのテンプレート構文  http://secret-garden.hatenablog.com/entry/2019/12/09/201235   
  
  
  
### Span 
https://cpprefjp.github.io/reference/span.html
C++20 span tutorial  https://solarianprogrammer.com/2019/11/03/cpp-20-span-tutorial/     
    
	
	
### std::format
- [printf 쓰지 마세요! std::format](https://blog.naver.com/nsun527/221834454953 )
- [std::format](https://blog.naver.com/kmc7468/221707892020 )
- [std::format 사용해보기(?)](https://blog.naver.com/nortul/222092094688 )
  
https://cpprefjp.github.io/reference/format.html
An Extraterrestrial Guide to C++20 Text Formatting  https://www.bfilipek.com/2020/02/extra-format-cpp20.html   
新しいフォーマットライブラリ <format> が追加   https://qiita.com/advent-calendar/2019/cpp20_pink_bangbi   
   
   
  
### [ Chrono ]
https://cpprefjp.github.io/reference/chrono.html

Calendar and Time-Zones in C++20: Time of Day
http://www.modernescpp.com/index.php/calendar-and-time-zone-in-c-20 

C++標準ライブラリの時計(Clock)
http://d.hatena.ne.jp/yohhoy/20180320/p1 

C++標準ライブラリのカレンダー(Calendar)
http://d.hatena.ne.jp/yohhoy/20180322/p1 

C++標準ライブラリのタイムゾーン(Time Zone)
http://d.hatena.ne.jp/yohhoy/20180326/p1 

C++20標準ライブラリ <chrono>ヘッダ Tips
https://qiita.com/yohhoy/items/8e751b019b369693f0f3 
  
  
  
### std::Bit 
https://cpprefjp.github.io/reference/bit.html  
  
<bit> in Visual Studio 2019 version 16.8 Preview 2  https://devblogs.microsoft.com/cppblog/bit-in-visual-studio-2019-version-16-8-preview-2/   
  
  
  
### constexpr, consteval, constinit
- [constexpr을 넘어선 consteval](https://blog.naver.com/nsun527/221834432741 )
- [constexpr, consteval, constinit](https://blog.naver.com/kmc7468/221705880457 )
(working) constinit consteval constexpr constの違い
https://gununu.hatenadiary.jp/entry/2019/10/15/020903   

  
### Concurrency
Concurrency in C++20 and beyond  https://github.com/CppCon/CppCon2019/blob/master/Presentations/concurrency_in_cpp20_and_beyond/concurrency_in_cpp20_and_beyond__anthony_williams__cppcon_2019.pdf   
C++20便利機能の紹介：自動joinスレッドと停止機構 std::jthread, stop_token  https://qiita.com/yohhoy/items/e493aeacef9a1f6315b9    
https://cpprefjp.github.io/reference/thread/jthread.html  
  
(working) std::latch
https://cpprefjp.github.io/reference/latch/latch.html

(working) std::barrier
https://cpprefjp.github.io/reference/barrier/barrier.html

(working) stop_token
https://cpprefjp.github.io/reference/stop_token.html
  
(working) C++ Synchronized Buffered Ostream
http://d.hatena.ne.jp/yohhoy/20171121/p1 

(working) C++20便利機能の紹介：同期出力ストリーム std::osyncstream
https://qiita.com/yohhoy/items/b0fa779176d5debcd09e 
  
  
    
### module
- [module 사용해보기](https://speedr00t.tistory.com/768 )
  
https://cpprefjp.github.io/lang/cpp20/modules.html  

C++ にモジュールがやってくる！ 
http://secret-garden.hatenablog.com/entry/2019/12/23/212339 

A Tour of C++ Modules in Visual Studio
https://devblogs.microsoft.com/cppblog/a-tour-of-cpp-modules-in-visual-studio/ 

Standard C++20 Modules support with MSVC in Visual Studio 2019 version 16.8
https://devblogs.microsoft.com/cppblog/standard-c20-modules-support-with-msvc-in-visual-studio-2019-version-16-8/ 

Visual C++ 2015 Update 1でC++にmoduleが試験的に実装された
http://nekko1119.hatenablog.com/entry/2015/12/02/070752 

モジュール理論 基礎編
https://onihusube.hatenablog.com/entry/2019/07/07/025446  

C++20 モジュールの概要 / Introduction to C++ modules (part 1)
https://www.slideshare.net/TetsuroMatsumura/c20-152189285 

続・モジュール / Introduction to C++ modules (part 2)
https://www.slideshare.net/TetsuroMatsumura/introduction-to-c-modules-part-2-225829715   

Modules in VC++ 2019 16.5
https://mariusbancila.ro/blog/2020/05/07/modules-in-vc-2019-16-5/ 

C++20: A Simple math Module
http://www.modernescpp.com/index.php/cpp20-a-first-module  

Practical C++ Modules 
https://github.com/CppCon/CppCon2019/blob/master/Presentations/practical_cpp_modules/practical_cpp_modules__boris_kolpackov__cppcon_2019.pdf 

モジュールへの移行を考える 1 - 単一ヘッダファイル+単一ソースファイル
https://zenn.dev/onihusube/articles/299ed7a3bc6062068fdd 
モジュールへの移行を考える 2 - 実装の隠蔽について
https://zenn.dev/onihusube/articles/7f1a6253b0683458b9d0 

C＋＋20のモジュールとincludeの違いについて詳しく調べてみた
https://logicalbeat.jp/blog/6223/ 
  
    
  
### Coroutine
- [Coroutine ( 코루틴 ) - 1](https://openmynotepad.tistory.com/65 )
- [Coroutine ( 코루틴 ) - 2](https://openmynotepad.tistory.com/66 )
- [Coroutine ( 코루틴 ) - 3](https://openmynotepad.tistory.com/67 )
  
  
  
### Concepts  
- [Concepts ( 콘셉트, 개념 ) - 1](https://openmynotepad.tistory.com/69 )
- [Concepts ( 콘셉트, 개념 ) - 2](https://openmynotepad.tistory.com/70 )
- [Concepts ( 콘셉트, 개념 ) - 3](https://openmynotepad.tistory.com/71 )
- [Concepts ( 콘셉트, 개념 ) - 4](https://openmynotepad.tistory.com/72 )
- [Concepts 알아보기 (1)](https://blog.naver.com/nsun527/221760493147 )
- [concept를 이용한 타입 오버플로우 체크](https://blog.naver.com/wkdghcjf1234/222183575154 )
- [Concepts를 이용한 Join, Concat 구현](https://blog.naver.com/wkdghcjf1234/222181356030 )
- [concepts를 이용한 String::Format](https://blog.naver.com/wkdghcjf1234/222176161232 )
  
https://cpprefjp.github.io/lang/cpp20/concepts.html

C++20コンセプト入門以前
https://qiita.com/yohhoy/items/f3d90c598348817cd29c 

【C++20】結局conceptは何が便利なのか
https://qiita.com/negi-drums/items/a527c05050781a5af523 

［C++］コンセプトの5景
https://onihusube.hatenablog.com/entry/2019/10/18/194328 

C++ にコンセプトがやってくる！ 
https://secret-garden.hatenablog.com/entry/2019/12/25/100312 

コンセプト制約式の包摂関係とオーバーロード解決
https://yohhoy.hatenadiary.jp/entry/20190903/p1  

コンセプトは滅びぬ！何度でもよみがえるさ！コンセプトの力こそC++erの夢だからだ！
https://spinor.hatenablog.com/entry/20111215/1323951052 

requires式から利用可能な宣言 
https://yohhoy.hatenadiary.jp/entry/20200721/p1 
C++20コンセプト時代のエラーメッセージとの付き合い方
https://qiita.com/yohhoy/items/9dffbe98497cf12483c6 

Slides for Meeting C++ 2019: Concepts in C++20: A Evolution or a Revolution?
https://meetingcpp.com/mcpp/slides/2019/conceptsBerlin.pdf 

複合要件とsame_as/convertible_toコンセプト
https://yohhoy.hatenadiary.jp/entry/20200825/p1

SFINAEとC++20のコンセプトを比較してみる。
https://kenkyu-note.hatenablog.com/entry/2020/08/07/073349 

Intro to C++20's Concepts
https://github.com/hniemeyer/IntroToConcepts 

コンセプトと短絡評価
https://yohhoy.hatenadiary.jp/entry/20201119/p1 

Concepts で外部ライブラリの template よりも優先されるオーバーロード関数を書く 
https://srz-zumix.blogspot.com/2020/03/wipc-concept-template.html 
  
    
  
  
  
   
### [C++20 std::Range ]
範囲ライブラリとして <ranges> が追加される
http://secret-garden.hatenablog.com/entry/2019/12/21/224438 

Visual Studio Community 2019 PreviewでC++20 Rangesの主要部分がサポートされた
https://qiita.com/katabamisan/items/407b472596c98fcc5226 

範囲ライブラリ (C++20)
https://ja.cppreference.com/w/cpp/ranges 

範囲ライブラリとして <ranges> が追加される
http://secret-garden.hatenablog.com/entry/2019/12/21/224438 

C++20のRangeライブラリの強力な機能、プロジェクション
https://cpplover.blogspot.com/2019/01/c20range.html  

http://becpp.org/blog/wp-content/uploads/2019/10/Arno-Schoedl-From-Iterators-To-Ranges.pdf   

C++20 Standard Library Beyond Ranges 
https://github.com/CppCon/CppCon2019/blob/master/Presentations/cpp20_standard_library_beyond_ranges/cpp20_standard_library_beyond_ranges__jeff_garland__cppcon_2019.pdf 

C++20 Ranges, Projections, std::invoke and if constexpr
https://www.bfilipek.com/2020/10/understanding-invoke.html 

C++20: The Ranges Library
http://www.modernescpp.com/index.php/c-20-the-ranges-library 

A custom C++20 range view
https://mariusbancila.ro/blog/2020/06/06/a-custom-cpp20-range-view/ 

C++ STL イテレーターのRange access（std::begin等）の概要と一覧
https://qiita.com/katabamisan/items/7bc5299f16d1afbfcb13 

std::views::splitで文字列分割
https://yohhoy.hatenadiary.jp/entry/20201027/p1 

From STL to Ranges 
https://github.com/CppCon/CppCon2019/blob/master/Presentations/from_stl_to_ranges/from_stl_to_ranges__jeff_garland__cppcon_2019.pdf 

Increased Complexity of C++20 Range Algorithms Declarations - Is It Worth?
https://www.bfilipek.com/2020/10/complex-ranges-algorithms.html 

<ranges>のviewを見る1 - empty_view
https://zenn.dev/onihusube/articles/6608a0185832dc51213c 
<ranges>のviewを見る2 - single_view
https://zenn.dev/onihusube/articles/5c922fe6856859ef8bf7 
<ranges>のviewを見る3 - iota_view
https://zenn.dev/onihusube/articles/87647047e094fe5f3b94 
<ranges>のviewを見る4 - istream_view
https://zenn.dev/onihusube/articles/ff891b851802813d73a1 
<ranges>のviewを見る5 - ref_view
https://zenn.dev/onihusube/articles/d900f52393b809f5300a 
<ranges>のviewを見る6 - filter_view
https://zenn.dev/onihusube/articles/d6ea9550bd0daf46394c 
<ranges>のviewを見る7 - transform_view
https://zenn.dev/onihusube/articles/6e053bfeb4fce1db0613 
<ranges>のviewを見る8 - take_view
https://zenn.dev/onihusube/articles/cadd871201d9ac0dd322 
<ranges>のviewを見る9 - take_while_view
https://zenn.dev/onihusube/articles/4c9df5ac12e042eb62a6 
<ranges>のviewを見る10 - drop_view
https://zenn.dev/onihusube/articles/a0b5207df9d587ce0973 
<ranges>のviewを見る11 - drop_while_view
https://zenn.dev/onihusube/articles/8fa73ccc945e6002ba5f 
<ranges>のviewを見る12 - join_view
https://zenn.dev/onihusube/articles/42b5465e778cee595f76 
<ranges>のviewを見る13 - split_view
https://zenn.dev/onihusube/articles/8accfa7e3e30239d7e91 
<ranges>のviewを見る14 - counted view
https://zenn.dev/onihusube/articles/5b857ad5bae3190d0d3b 
<ranges>のviewを見る15 - common_view
https://zenn.dev/onihusube/articles/ee74272e49fb3953fa3b 
<ranges>のviewを見る16 - reverse_view
https://zenn.dev/onihusube/articles/b91aa582d28ed869ec09 
<ranges>のviewを見る17 - elements_view
https://zenn.dev/onihusube/articles/9a13745c24b36b9bd753 
  
  
  
### Template
- [(일어) 타입 이름으로 있는 것이 확실한 문맥에서 typename을 생략 가능하다](
http://secret-garden.hatenablog.com/entry/2019/12/05/180904 )


### iterator
- [(일어) C++20에서 iterator 행동 조사 방법](https://onihusube.hatenablog.com/entry/2020/12/11/000123 )
- [(일어) C++20에서의 iterator_traits 사정](https://onihusube.hatenablog.com/entry/2020/12/14/002822  )
- [(일어) C++17 iterator <=> C++20 iterator != 0](https://onihusube.hatenablog.com/entry/2020/12/27/150400  )






