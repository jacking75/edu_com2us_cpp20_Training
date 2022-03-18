# concept

## concept은 무엇이 편리한가?
[출처](https://qiita.com/negi-drums/items/a527c05050781a5af523 )  
    
### requires 사용 1
concept은 템플릿 매개 변수를 하나 이상 가지고, 템플릿 매개 변수의 첫 번째 인수에 대한 제한을 나타낸다.   
bool로 평가 되는 메타 함수의 일종이기도 하다.  
concept을 정의하려면 컴파일 시 bool로 평가되는 표현식을 사용한다.  
concept을 정의하는데 특히 자주 사용하는 것이 requires 식으로 식에 제약을 걸고, 이것은 제약을 충족하는지 여부를 bool 값으로 반환하는 식이다.  
```
#include <cassert>

template<typename T> constexpr bool b = requires { // 제약을 만족하면 true, 만족하지 않으면 false 
  typename T::type;           // 타입 T가 멤버 타입에 type을 가져야 한다는 제약
};

template<typename T> concept C1 = b<T>;  // "타입 T가 b<T> == true 를 만족" 한다는 제약

template<typename T> concept C2 = requires {  // 직접 requires를 가져도 좋다. C1과 같다.
  typename T::type;
};

struct S { using type = int; };

int main() {
  static_assert(b<S>);      // S는 type을 멤버 타입으로 가지고 있으므로 b<S> == true
  static_assert(!b<int>);   // int는 type을 멤버 타입으로 가지고 있지 않으므로 b<int> == false

  static_assert(C1<S>);     // concept도 bool로 평가 할 수 있다
  static_assert(!C1<int>);

  static_assert(C2<S>); 
  static_assert(!C2<int>);
  return 0;
}
```  
  
또한 requires 식은 인수의 종류를 취할 수 있다. 이 인수는 타입 정보만 사용되고, 실제로 평가되지 않는다.  
이것을 사용해서 멤버 변수와 멤버 함수, 비 멤버 함수에 대한 제한이 쓰기 쉬워진다. 또한 함수의 반환 값도 제한 할 수 있고, 이것은 concept을 이용하여 쓸 수 있다.   
concept의 첫 번째 템플릿 인수가 반환 값에 충당되는 점에 유의하자.   
```
#include <type_traits>
#include <vector>
#include <list>

template<typename T, typename U> concept C1 
  = std::is_convertible_v<T, U>; // "타입 T가 타입 U로 타입 캐스트 가능" 이라는 제약. 타입 T에 대한 제약인 것에 주의

template<typename T> concept C2 = requires (T a, int b) {
  a.push_back(b);      // "타입 T가 멤버 함수에 push_back을 가진다"라는 제약
  { a[b] } -> C1<int>; // "타입 T에 operator[]가 있고, C1<decltype(a[b]), int> == true" 라는 제약
  std::size(a);        // "비 멤버 함수 std::size가 인수로 타입 T의 오브젝트를 받을 수 있다" 라는 제약
};

int main() {
  static_assert(C2<std::vector<int>>);  // 제약을 만족한다
  static_assert(!C2<std::list<int>>);   // 제약을 만족하지 않는다(operator[] 가 없다)
  return 0;
}
```   
       
  
### requires 사용 2
requires는 bool을 반환하는 메타 함수를 정의하는 것 외에 템플릿 매개 변수를 제한하는 방법도 있다. requires 후에 bool로 계산 되는 식을 두는 것으로, 템플릿 파라미터를 제한한다.  
```
#include <type_traits>
#include <utility>

// 함수 템플릿 제한
template<typename T> requires std::is_convertible_v<T, int> int cast_to_int(T a) {
  return static_cast<int>(a);
}

template<typename T> concept C = std::is_pointer_v<T>;
template<typename T> requires C<T> bool nullcheck(T ptr) {
  return ptr != nullptr;
}

// 처음의 requires 식은 템플릿 파라메터를 제한하고, 두 번째 requires는 bool을 반환하는 메타 함수를 정의한다
template<typename T> requires requires(T t) { t.first; }  // "멤버 함수에 t.first를 가진다" 라는 제약
auto first(T x) {
  return x.first;
} 

// 템플릿 클래스/구조체도 제한 가능
template<typename T> requires std::is_integral_v<T> struct S {
  T t = static_cast<T>(0);
};

int main() {
  cast_to_int('C');         // char는 제약을 만족하므로 OK
  // cast_to_int(nullptr);  // 이것은 제약을 만족하지 않으므로 컴파일 에러

  nullcheck("C++");       // const char*는 제약을 만족하므로 OK
  // nullcheck('C');      // char는 제약을 만족하지 않으므로 에러

  first(std::make_pair(42, 'C'));  // OK
  // first('C');                   // 에러

  S<unsinged> s1;    // OK
  // S<float> s2;    // 에러
  return 0;
}
```  
  
  
### 더 간결하게
requires 사용 이외에도 concept을 이용하여 템플릿 매개 변수를 제한하는 방법이 있다. 이쪽이 일반적으로 깔끔하게 쓸 수 있다.  
```
#include <type_traits>

template<typename T> concept integral_c = std::is_integral_v<T>;
template<integral_c T, integral_c U>    // integral_c<T> == integral_c<U> == true 가 되는 T, U 만 허가
auto add(T t, U u) {
  return t + u;
}

template<typename T, typename U> concept convertible_to_c = std::is_convertible_v<T, U>;
template<convertible_to_c<int> T>       // convertible_to_c<T, int> == true 가 되는 T만 허가
int cast_to_int(T a) {
  return a;
}

template<integral_c T> struct S {    // 템플릿 클래스/구조체도 같이 제한 가능
  T t = static_cast<T>(0);
};

int main() {
  add(42, 420LL);      // OK
  // add(42, 4.2);     // NG

  cast_to_int('a');         // OK
  // cast_to_int(nullptr);  // NG

  S<unsigned> s1;   // OK
  // S<float> s2;   // NG
  return 0;
}
```   
  
또한 템플릿 함수는 auto를 사용하여 더 간결하게 제한 할 수 있다.  
```
#include <type_traits>

template<typename T> concept integral_c = std::is_integral_v<T>;
auto add(integral_c auto t, integral_c auto u) {  // 앞의 add 함수와 완전 같다. 인수의 auto는 템플릿 파라메터로 된다
  return t + u;
}

int main() {
  add(42, 420LL);     // OK
  // add(42, 4.2);    // NG
  return 0;
}
```  
  
  
### 어떤 도움이 되나?
그런데, 긴 오프닝이 끝나고 이제부터가 본론이지만, 도대체 어떤 도움이 되는지를 보여주고 싶다.  
사람들이 말하는 장점 중 하나는 오류 메시지가 읽기 쉬운 것이 있다. 확실히 오류 메시지는 가독성이 좋게 된다. 그러나, 이 이상의 혜택이 concept에 있다.   
그것은 오버로드 해결과 constexpr if와 결합했을 때 발휘된다. 아래의 코드 예제를 살펴 보자.  
```
#include <vector>
#include <list>

template<typename T> concept forward_iterator_c = requires(T it1, T it2) {
  // 본래는 반환 값에 관한 제약도 걸 수 있지만 일단 생략. 기타 자잘한 것도 생략
  it1++;                ++it1;                     // 반복자
  *it1;                                            // 참조
  it1 = it2;            it1 = std::move(it2);      // 대입
  T(it1);               T(std::move(it1));         // 생성자
  it1 == it2;           it1 != it2;                // 등치 비교
};

template<typename T> concept random_access_iterator_c = forward_iterator_c<T>
  && requires (T it1, T it2, int a) {
  it1 + a;              it1 - a;                   // 임의 반복자
  it1 - it2;                                       // 차분
  it1 < it2;            it1 > it2;                 // 비교
  it1 <= it2;           it1 >= it2;                // 비교
};

template<forward_iterator_c FwdIt> int count(FwdIt first, FwdIt last) {        // 1
  int sum = 0;
  for (FwdIt it = first; it != last; ++it) { sum++; }
  return sum;
}

template<random_access_iterator_c RndIt> int count(RndIt first, RndIt last) {  // 2
  return last - first;
}

int main() {
  std::vector<int> vec(10);
  std::list<int> li(10);
  count(std::begin(vec), std::end(vec));   // 2 の count が呼ばれる
  count(std::begin(li), std::end(li));     // 1 の count が呼ばれる
  return 0;
}
```  
    
위의 코드는 2의 count는 1의 count 보다 엄격한 제약이 부과 되고 있는 것을 알 수 있다. 그러면 2가 1의 부분 특수화된 것으로 간주되고, 제약 조건을 충족하는지 여부로 오버로드 해결이 이루어진다.   따라서 반복자가 임의 접근 반복자 또는 포워드 반복자 인가로 분기 시킬 수 있게 되는 구조이다. 물론 constexpr if를 사용하여 아래와 같은 코드를 쓸 수 있다. 나는 아래 작성을 선호한다.  
```
template<forward_iterator_c FwdIt> int count(FwdIt first, FwdIt last) {
  if constexpr (random_access_iterator_c<FwdIt>) {
    return last - first;
  }
  else {
    int sum = 0;
    for (FwdIt it = first; it != last; ++it) { sum++; }
    return sum;
  }
}
```  
  
이와 같이 반복자 같이 비슷한 인터페이스를 가진 것에 다른 행동을 시키고 싶을 때 concept은 매우 유용하다. 이것은 상속과 다형성의 개념과 유사함을 느낀다. 그래서, 상속 및 다형성 코드와 concept을 사용한 코드를 비교해 보자.   
```
#include <iostream>

/* 다양성과 상속 */
struct PolymorphicCharacter {    // 베이스가 되는 인터페이스
  virtual ~PolymorphicCharacter() noexcept = default;
};
struct PolymorphicNPC : PolymorphicCharacter {  // 인터페이스 1
  virtual void action() const = 0;
  ~PolymorphicNPC() noexcept override = default;
};
struct PolymorphicEnemy : PolymorphicCharacter {  // 인터페이스 2
  virtual void attack() const = 0;
  ~PolymorphicEnemy() noexcept override = default;
};
struct PolymorphicCat : PolymorphicNPC {  // NPC만 된다
  void action() const override { std::cout << "침대에서 잔다" << std::endl; }
  ~PolymorphicCat() noexcept override = default;
};
struct PolymorphicDog : PolymorphicNPC, PolymorphicEnemy {  // NPC도 Enemy도 될 수 있는 Character
  void action() const override { std::cout << "棒にポリモーフィックに当たる" << std::endl; }
  void attack() const override { std::cout << "棒でポリモーフィックに叩く" << std::endl; }
  ~PolymorphicDog() noexcept override = default;
};
struct PolymorphicMonkey : PolymorphicEnemy {  // Enemy만 된다
  void attack() const override { std::cout << "木からポリモーフィックに落とす" << std::endl; }
  ~PolymorphicMonkey() noexcept override = default;
};
void polymorphism_action_n_times(const PolymorphicNPC& npc, int n) {
  for (int i = 0; i < n; i++) { npc.action(); }
}
void polymorphism_attack_n_times(const PolymorphicEnemy& enemy, int n) {
  for (int i = 0; i < n; i++) { enemy.attack(); }
}

/* concept */
template<typename T> concept ConceptCharacter = true;   // 베이스가 되는 인터페이스
template<typename T> concept ConceptNPC = ConceptCharacter<T> && requires(T npc) { npc.action(); };  // 인터페이스 1
template<typename T> concept ConceptEnemy = ConceptCharacter<T> && requires(T enemy) { enemy.attack(); };  // 인터페이스 2
struct ConceptCat {   // NPC만 된다
  void action() const { std::cout << "こたつでコンセプチュアルに寝る" << std::endl; }
};
struct ConceptDog {   // NPC도 Enemy도 되는 Character
  void action() const { std::cout << "棒にコンセプチュアルに当たる" << std::endl; }
  void attack() const { std::cout << "棒でコンセプチュアルに叩く" << std::endl; }
};
struct ConceptMonkey {  // Enemy만 된다
  void attack() const { std::cout << "木からコンセプチュアルに落とす" << std::endl; }
};
void concept_action_n_times(const ConceptNPC auto& npc, int n) {
  for (int i = 0; i < n; i++) { npc.action(); }
}
void concept_attack_n_times(const ConceptEnemy auto& enemy, int n) {
  for (int i = 0; i < n; i++) { enemy.attack(); }
} 

int main() {
  PolymorphicCat pcat;
  PolymorphicDog pdog;
  PolymorphicMonkey pmon;
  polymorphism_action_n_times(pcat, 3);
  polymorphism_action_n_times(pdog, 3);
  polymorphism_attack_n_times(pdog, 3);
  polymorphism_attack_n_times(pmon, 3);

  ConceptCat ccat;
  ConceptDog cdog;
  ConceptMonkey cmon;
  concept_action_n_times(ccat, 3);
  concept_action_n_times(cdog, 3);
  concept_attack_n_times(cdog, 3);
  concept_attack_n_times(cmon, 3);
  return 0;
}
```    
  
긴 예제가 되어 버렸지만 모두 비슷한 것을 구현하고 있다. 그러나 이 두개는 결정적인 차이가 있다. 다형성은 호출하는 가상 함수를 동적으로(런타임에) 결정하기 때문에 런타임 시간 비용이 소요된다. 반면 concept은 함수를 정적으로 (컴파일 시에) 결정하기 때문에 런타임 비용은 최소화 할 수 있다. 그 대신 컴파일 할 때 이진 파일의 크기는 커진다. 또한 concept을 이용하면 다형성에서 말하는 객체의 포인터 나 참조에 자식 클래스의 객체를 넣을 수 없게 되므로 주의가 필요하다. 문제가 없는 경우는 적극적으로 concept을 사용하는 것이 좋다.  
또한 concept을 사용하면 성가신 다중 상속을 방지 할 수도 있다고 생각한다. 물론 다중 상속을 사용하는 것은 나쁜 것은 아니지만 가능하면 사용하고 싶지는 않을 것이다.  
  
  
  
## 복합 요건과 same_as / convertible_to 개념
[출처](https://yohhoy.hatenadiary.jp/entry/20200825/p1 )   
C++20의 컨셉 requires 식에 기술하는 복합 요구 사항(compound-requirement)은 "어떤 식의 평가 결과가 특정 개념을 충족"하는 것을을 제약하는데, 이 때 표준 개념 std::same_as/ std::convertible_to를 적절하게 맞게 사용해야 한다. 특히 "데이터 멤버 타입 제약 조건"에 주의 할 것.  
  
아래 코드의 컨셉 C0에서는 식 `t.data`가 `int` 타입으로 변환 가능(`convertible_to<int>`)라고 제약하고 있지만, 컨셉 C1 처럼 int 타입과 동일한(`same_as<int>`) 라고 제한하면 프로그래머의 예상대로 작동하지 않는다.  
```
#include <concepts>

struct S {
  int data;   // int 타입의 데이터 멤버
  int& mf();  // int& 타입을 반환하는 멤버 함수
};

template <typename T> concept C0 = requires (T t) {
  // T 타입의 값 t는 데이터 멤버 data를 가지고, int 타입으로 변환 가능
  { t.data } -> std::convertible_to<int>;
  // T 타입의 값 t는 멤버 함수 mf()을 가지고, 호출 결과는 int& 타입과 같다
  { t.mf() } -> std::same_as<int&>;
};
static_assert( C0<S> );  // OK

template <typename T> concept C1 = requires (T t) {
  // T 타입의 값 t은 데이터 멤버 data를 가지고, "식 t.data"는 int 타입과 같다
  { t.data } -> std::same_as<int>;
  // ...
};
static_assert( C1<S> );  // NG: 식 t.data의 값은 int&
```  
    
반환 형식을 제약하는 복합 요구 사항 `{ E } -> C<U>;`은 두 가지 제약 `E; requires C<decltype((E)), U>;`와 동일하다. 여기서 decltype 지정자 피연산자가 괄호로 둘러싸인 식 `(E)`가 되는 것에 유의.  
```
template <typename T> concept C1 = requires (T t) {
  // { t.data } -> std::same_as<int>; 와 같은 제약
  t.data;
  requires std::same_as<decltype((t.data)), int>;
  // ...
};
```
  
클래스의 데이터 멤버 형식을 same_as 컨셉으로 제약 할 경우 아래 컨셉 C2처럼 중첩 요구 사항(nested-requirement)을 이용한다.
```
template <typename T> concept C2 = requires {
  // 멤버 T::data는 int 타입과 같다
  requires std::same_as<decltype(T::data), int>;
  // ...
}
static_assert( C2<S> );  // OK
```  
  
아래의 컨셉 C3처럼 복합 요건과 same_as 개념을 결합하면 데이터 멤버 타입은 int 또는 int& 모두 제약을 만족한다. 이것은 same_as 컨셉 이용 의도로 하면 잘못된 제약 표현이라고 할 수있다.  
```
struct S1 {
  int data;   // int 타입의 데이터 멤버
};
struct S2 {
  int& data;  // int& 타입의 데이터 멤버
};

template <typename T> concept C3 = requires (T t) {
  { t.data } -> std::same_as<int&>;
};

// OK: 데이터 멤버가 int 타입 또는 int& 타입이라면 제약을 만족한다
static_assert( C3<S1> && C3<S2> ); 
```  
   
    
## 컨셉과 단락 평가
[출처](https://yohhoy.hatenadiary.jp/entry/20201119/p1 )  
  
컨셉의 제약 식(constraint-expression)은 논리 연산 &&, ||는 단락 평가된다.  
  
메타 함수 `is_lock_free`의 정의로 `&&` 연산자를 사용한 경우 좌변 `std::is_trivially_copyable<T>::value`의 진위에 관계 없이 우변의 `is_lock_free_impl<T>::value` 인스턴스화가 행해진다.  
```
#include <atomic>
#include <type_traits>

// 컨셉 is_lock_free
template <typename T>
concept is_lock_free
  = std::is_trivially_copyable_v<T> && std::atomic<T>::is_always_lock_free;

// (std::atomic<int> 타입이 락플리 동작을 할 수 있는가를 가정）
static_assert( is_lock_free<int>, "int is lock-free" );  // OK

// 비 Trivially Copyable한 클래스 타입X
struct X { ~X() {} };
static_assert( !is_lock_free<X>, "X is not lock-free" );  // OK
```  
      
	  
## 참고
- [C++ Standard concepts diagrams](https://github.com/yohhoy/cpp-concepts/blob/main/README.md)	  