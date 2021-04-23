# std::ranged
Ranges는 아래의 range와 같이 "컨셉"을 만족하는 타입과 이것을 지지하는 기능의 통칭이라고 할 수 있다.  
range 컨셉 이외에도 common_range, viewable_range가 있다.  
```
template<class T>
concept range = requires(T& t) {
  ranges::begin(t);
  ranges::end(t);
};
```  
   
range에는 STL 컨테이너인 std::vector, std::list 등을 사용 가능하고, begin과 end를 구현하는 자작 클래스나 구조체도 사용할 수 있다.    
예를 들면 std::views::transform은 range에서 파생된 컨셉 viewable_range에 따른 타입에 적용할 수 있다.       
```
#include <iostream>
#include <ranges>
#include <vector>
 
int main() 
{
    std::vector<int> ints{0, 1, 2, 3, 4, 5};
    auto even = [](int i) { return 0 == i % 2; };
    auto square = [](int i) { return i * i; };
 
    for (int i : ints | std::views::filter(even) | std::views::transform(square)) {
        std::cout << i << ' ';
    }
}
```   
  
결과   
<pre>
0 4 16
</pre>  
  
```
#include <iostream>
#include <ranges>
#include <vector>

class test {
public:
    test() : v{ 0, 1, 2, 3 } {}
    auto begin() { return v.begin(); }
    auto end() { return v.end(); }
private:
    std::vector<int> v;
};

int main()
{
    test t;
    auto mul2 = [](int x) { return x * 2; };
    for (auto x : t | std::views::transform(mul2))
        std::cout << x << std::endl;
    // 0, 2, 4, 6
    return 0;
}
```

`std::views::transform`의 호출 순서는 아래와 같다   
1. `std::views::transform`는 `std::views` 이름 공간에서 정의된 `_Transform_fn` 타입의 변수이다.  
2. `_Transform_fn` 타입은 `_Copy_constructible_object` 컨셉을 만족하는 타입 `_Fn`에 대한 `operator(_Fn _Fun)`을 정의해 두고, `transform(...)`은 이것을 호출한다.  
3. 위의 `operator(_Fn _Fun)`은 `_Transform_fn` 타입에서 private로 정의된 `_Partial<_Fn>` 타입의 오브젝트를 반환한다.  
4. `_Partial<_Fn>` 타입은 `_Pipe::_Base<_Partial<_Fn>>` 타입에서 파생하고, 이 `_Pipe::_Base<_Partial<_Fn>>` 타입이 정의하는 우측 용 | 연산자가 호출된다(| std::views::transform(...) |).  
5. `_Partial<_Fn>` 타입은 `transform_view`를 반환하는 `()` 연산자를 정의해 두고, 위의 `|` 연산자가 이것을 호출한다.   
  


## std::views의 함수 리스트
- std::all_view, std::views::all               // takes all elements
- std::ref_view                                // takes all elements of another view
- std::filter_view, std::views::filter         // takes the elements which satisfies the predicate
- std::transform_view, std::views::transform   // transforms each element
- std::take_view, std::views::take             // takes the first N elements of another view
- std::take_while_view, std::views::take_while // takes the elements of another view as long as the predicate returns true
- std::drop_view, std::views::drop             // skips the first N elements of another view
- std::drop_while_view, std::views::drop_while // skips the initial elements of another view until the predicate returns false
- std::join_view, std::views::join             // joins a view of ranges
- std::split_view, std::views::split           // splits a view by using a delimiter
- std::common_view, std::views::common         // converts a view into a std::common_range
- std::reverse_view, std::views::reverse       // iterates in reverse order
- std::basic_istream_view, std::istream_view   // applies operator>> on the view
- std::elements_view, std::views::elements     // creates a view on the N-th element of tuples
- std::keys_view, std::views::keys             // creates a view on the first element of a pair-like values
- std::values_view, std::views::values         // creates a view on the second elements of a pair-like values
   
    
## View에 대해서
range 라이브러리에서 View는 다른 곳(언어, 라이브러리, 개념 ...)에서 임의의 순서에 대한 View 라고 하는 것과 같은 의미이다.  
원래의 순서에 대해서 뭔가 작업을 적용한 결과 얻어진 **원래의 시퀀스를 복사하지 않고 참조**하며 지연 평가를 통해 시퀀스 조작을 적용하는 것이다.  
또한 View 자신도 순서이므로 View에 또 다른 처리를 적용 해 나갈 수 있도록 되어 있다.  
  
range 라이브러리에서 View 개념에 의해 구문 · 의미 론적 방향에서 아래와 같이 정의한다.  
```
template<class T>
concept view =
  range<T> &&                 // begin()/end()에 의한 반복자 쌍을 취득 가능
  movable<T> &&               // move 가동
  default_­initializable<T> && // default 구축 가능
  enable_view<T>;             // view 컨셉도 유효화 하는 변수 템플릿
```  
    
- 무브 구축 / 대입은 상수 시간
- 소멸은 상수 시간
- 복사 불가 또는 복사 구축/대입은 상수 시간  
  
이 정의에 따라서 타입이 range 라이브러리에서 View 로 처리된다.  
알기 어려울지도 모르지만 의미하는 바는 곧 모든 순서를 소유하지 않고 참조하여 View 자신의 구축 · 복사 · 무브 파기 참조하는 범위와는 무관하다는 것입니다.
  
실제 구현은 거의 확실히 반복자 쌍(range)를 유지하는 클래스 타입이 되고, View 관련된 작업은 begin() 호출 시 또는 이 반복기에 대해 ++, * 등의 작업 시간이 실행됨에 따라 지연 평가 될 것이다.  
  
표준 라이브러리에 있는 View인 클래스 타입은 예를 들어 `std::string_view` 이다. `std::string_view` 자신도 `range` 이며, 무브와 기본 구축이 가능하며, 단순히 문자열의 시작 포인터와 길이를 유지하는 것이므로, 문법적으로도 의미론적으로도 이 정의에 따라 있다.  
그러나 `std::ranges::enable_view<std::string_view>`(위 개념 정의의 맨 마지막 조건)이 false가 되므로 `std::string_view`는 view 컨셉을 충족하지 않습니다. `enable_view`는 view 컨셉을 활성화하기 위한 마지막 슬쩍이다.  
   

### view_interface
<ranges>에 있는 View가 되는 클래스는 공통 부분의 구현을 단순화하기 위해 view_interface라는 클래스를 상속하고 있습니다. view_interface는 CRTP 의해 파생 클래스 타입을 받고 파생된 View 에 대해 컨테이너 인터페이스를 대비하기 위한 것입니다.
이에 따라 `empty()/data()/size()/front()/back()/operator[]` ㅌ고 말했다 ㅌ요소를 참조하는 작업이 가능합니다 (단, View 의 참조 range 유형 (즉, 반복자 카테고리)에 따라).
```
template<class D>
  requires is_class_v<D> && same_as<D, remove_cv_t<D>>
class view_interface : public view_base {
  // 略
};
```
   
view_base라는 것은 단순한 태그 타입에서 View가 되는 클래스를 식별하기 위한 것이다. View의 타입 D에 요구하는 것은 클래스 타입이며 CV 수식 되지 않는 것 뿐이다. 직접 View를 정의 할 때도 이를 이용하면 좋을 것이다. 덧붙여서, 이것을 상속 해두면 std::ranges::enable_view<T>자동으로 true되도록되어 있습니다.

View 명명 규칙 및 작업
<ranges>에있는 View 는 操作名_view라는 클래스로 std::ranges네임 스페이스에 정의되어 있으며, range 객체를 전달 구축하여 그 작업을 수행 View 객체를 얻을 수 있습니다. 그리고 그 작업에 대해 view를 만들기위한 함수 객체가 std::ranges::views이름 공간에 操作名_view대한 操作名것이 정의되어 있습니다. 여기를 이용하면 View 를 얻는 작업을 간결하게 쓸 수 있습니다.

네임 스페이스는 성실하게 작성하고 긴하지만 std::views 네임 스페이스 별칭이 준비되어 있으며, 이것을 이용하면 조금 짧게 쓸 수 있다.  
이런 함수 객체는 range factories 와 range adaptor objects 2 종류가 있다.
  

### 참고 
- empty_view  https://zenn.dev/onihusube/articles/6608a0185832dc51213c 
- single_view  https://zenn.dev/onihusube/articles/5c922fe6856859ef8bf7 
- iota_view  https://zenn.dev/onihusube/articles/87647047e094fe5f3b94 
- istream_view  https://zenn.dev/onihusube/articles/ff891b851802813d73a1 
- ref_view   https://zenn.dev/onihusube/articles/d900f52393b809f5300a 
- filter_view  https://zenn.dev/onihusube/articles/d6ea9550bd0daf46394c 
- transform_view  https://zenn.dev/onihusube/articles/6e053bfeb4fce1db0613 
- take_view  https://zenn.dev/onihusube/articles/cadd871201d9ac0dd322 
- take_while_view  https://zenn.dev/onihusube/articles/4c9df5ac12e042eb62a6 
- drop_view  https://zenn.dev/onihusube/articles/a0b5207df9d587ce0973 
- drop_while_view  https://zenn.dev/onihusube/articles/8fa73ccc945e6002ba5f 
- join_view  https://zenn.dev/onihusube/articles/42b5465e778cee595f76 
- split_view  https://zenn.dev/onihusube/articles/8accfa7e3e30239d7e91 
- counted view  https://zenn.dev/onihusube/articles/5b857ad5bae3190d0d3b 
- common_view  https://zenn.dev/onihusube/articles/ee74272e49fb3953fa3b 
- reverse_view  https://zenn.dev/onihusube/articles/b91aa582d28ed869ec09 
- elements_view  https://zenn.dev/onihusube/articles/9a13745c24b36b9bd753 
  


## 예제 모음
문자열 구분 [WandBox](https://wandbox.org/permlink/GCstTOXAOtUi0q8r )  [출처](https://yohhoy.hatenadiary.jp/entry/20201027/p1 )    
```
#include <iomanip>
#include <iostream>
#include <string>
#include <ranges>

auto split2str(char delim) {
  return std::views::split(std::views::single(delim))
       | std::views::transform([](auto v) {
           auto cv = v | std::views::common;
           return std::string{cv.begin(), cv.end()};
         });
}

int main() {
  std::string s = "apple,banana,cinnamon";
  for (const auto& word : s | split2str(',')) {
    std::cout << std::quoted(word) << "\n";
  }
}
```  
결과  
<pre>
"apple"
"banana"
"cinnamon"
</pre>  
   
  
## 참고 사이트
- [(일어) 범위 라이브러리](https://ja.cppreference.com/w/cpp/ranges )
- [(일어)C++20 Range 라이브러리의 강력한 기능, 프로젝션](https://cpplover.blogspot.com/2019/01/c20range.html  )
- [(pdf) From Iterators To Ranges](http://becpp.org/blog/wp-content/uploads/2019/10/Arno-Schoedl-From-Iterators-To-Ranges.pdf )
- [(pdf) From STL to Ranges](https://github.com/CppCon/CppCon2019/blob/master/Presentations/from_stl_to_ranges/from_stl_to_ranges__jeff_garland__cppcon_2019.pdf )
- [C++20 Ranges, Projections, std::invoke and if constexpr](https://www.bfilipek.com/2020/10/understanding-invoke.html )
- [C++20: The Ranges Library](http://www.modernescpp.com/index.php/c-20-the-ranges-library )
- [A custom C++20 range view](https://mariusbancila.ro/blog/2020/06/06/a-custom-cpp20-range-view/ )
- [range: Express one of multiple options in a nice way](https://mariusbancila.ro/blog/2021/03/18/express-one-of-multiple-options-in-a-nice-way/ )


  