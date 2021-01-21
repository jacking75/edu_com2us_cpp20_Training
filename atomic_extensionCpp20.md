# atomic Extensions in C++20
  
## std::atomic_ref
atomic_ref 클래스 템플릿은 생성자에서 받은 T 타입 변수에 참조에 대한 원자 조작을 적용하는 형태이다.   
비 원자적 개체에 대해서, 값 복사에 의한 std::atomic 타입 변환 필요 없이 원자 조작을 적용한다.  
  
개체가 대부분의 경우에 비 원자적으로 사용되고, 충돌 해결을 위해 원자적으로 행동해야하는 것이 일부 장면인 경우 개체를 강제로 원자 객체로 변환 해 버리는 것은 성능 저하로 이어진다. atomic_ref 클래스를 사용하여 이러한 상황에 대응하고, 일부 상황에서만 객체를 원자적으로 동작하도록 한다.  
  
같은 객체를 참조하는 서로 다른 atomic_ref 객체를 통해 이루어지는 원자적 조작은 공통 참조하는 개체에 대해 원자적으로 이루어진다. 즉, 국소적으로 atomic_ref 객체로 변환 하여 연산을 적용 할 수있다.

객체를 참조로 원자적 조작을 하기 위한 제약으로 아키텍처 별 얼라이먼트 제약을 채우는 것이 요구된다. 생성자와 대입 연산자 객체를 참조 할 때 멤버 상수로 얼라이먼트 값 required_alignment의 위치에 오브젝트가 배치 되는 것이다.  
  
atomic_ref 클래스 템플릿은 타입 T의 값을 복사가 아닌 참조로 유지하기 위해 atomic_ref 개체보다 먼저 참조되는 변수의 수명이 다 되어서는 안된다.  
  

### 공통 멤버 변수  
  
| 이름                    | 설명                                               | 대응 버전 |
|-------------------------|----------------------------------------------------|----------------|
| (constructor)           | 생성자                                     | C++20          |
| ~atomic_ref() = default | 소멸자                                       | C++20          |
| operator=               | 대임                                               | C++20          |
| is_lock_free            | 오브젝트가 락프리 행동을 할 수 있는지 판정한다 | C++20          |
| store                   | 값을 쓴다                                       | C++20          |
| load                    | 값을 읽는다                                       | C++20          |
| operator T              | 변수 T로의 변환 연산자                                  | C++20          |
| exchange                | 값을 교체한다                                     | C++20          |
| compare_exchange_weak   | 약한 비교러 값을 교체한다                           | C++20          |
| compare_exchange_strong | 강한 비교로 값을 교체한다                           | C++20          |
| wait                    | 깨울 때까지 대기한다                             | C++20          |
| notify_one              | 대기하고 있는 스레드를 하나 깨운다             | C++20          |
| notify_all              | 대기하고 있는 모든 스레드를 깨운다             | C++20          |  
  

### 예제 코드 
  
```
#include <iostream>
#include <atomic>
#include <thread>

struct Info {
  int value = 0;
};

int main()
{
  Info info;

  std::thread consumer_thread {[&info] {
    std::atomic_ref<int> x{info.value};
    while (true) {
       if (int value = x.exchange(0); value != 0) {
         std::cout << value << std::endl;
         break;
       }
    }
  }};

  std::atomic_ref{info.value}.store(3); // 클래스 템플릿의 템플릿 인수 추론도 사용 가능(<int> 생략)
  consumer_thread.join();
}
```  
  
출력  
<pre>
3
</pre>
   
 

##  std::atomic_wait
이 함수는 블럭킹 동기를 하기 위한 기능이고, 비지 루프에 의한 폴링 보다 에너지 소비가 낮은 효율적인 대기를 구현할 수 있다.  
이 함수에 의해 대기하면 이것을 깨워주는 함수는 atomic_notify_one(), atomic_notify_all() 이다.  
  
이 함수는 Windows에서는 WaitOnAddress() 함수, POSIX에서는 futex() 함수를 사용하고 있다.  

```
#include <iostream>
#include <atomic>
#include <thread>

class my_mutex {
  std::atomic<bool> state_{false}; // false:unlock, true:lock
public:
  void lock() noexcept {
    while (std::atomic_exchange(&state_, true) == true) {
      std::atomic_wait(&state_, true);
    }
  }

  void unlock() noexcept {
    std::atomic_store(&state_, false);
    std::atomic_notify_one(&state_);
  }
};

my_mutex mut;
void print(int x) {
  mut.lock();
  std::cout << x << std::endl;
  mut.unlock();
}

int main()
{
  std::thread t1 {[] {
    for (int i = 0; i < 5; ++i) {
      print(i);
    }
  }};
  std::thread t2 {[] {
    for (int i = 5; i < 10; ++i) {
      print(i);
    }
  }};

  t1.join();
  t2.join();
}
```
   
출력  
<pre>
0
5
1
6
2
7
3
8
4
9
</pre>
    
  
## std::atomic_wait_explicit
기능적으로 std::atomic_wait와 같다. 차이는 memory order 최적화를 할 수 있다.  
```
void lock() noexcept {
  while (std::atomic_exchange_explicit(&state_, true, std::memory_order::acquire) == true) {
    std::atomic_wait_explicit(&state_, true, std::memory_order::relaxed);
  }
}
```
  
  
## std::atomic_notify_one
`atomic_wait`로 대기 하고 있는 것 중 하나를 깨운다  
```
std::atomic_notify_one(&state_);
```


## std::atomic_notify_all
`atomic_wait`로 대기 하고 있는 것 모두를 깨운다  
```
std::atomic_notify_all(&state_);
```
  
  
## std::atomic_flag_test
현재 값을 bool 값으로 얻는다.  
memory order이 memory_order_release, memory_order_acq_rel 에서는 사용 불가.  
```
#include <iostream>
#include <atomic>

int main()
{
  std::cout << std::boolalpha;

  std::atomic_flag x = ATOMIC_FLAG_INIT;
  std::cout << std::atomic_flag_test(&x) << std::endl;

  std::atomic_flag_test_and_set(&x);
  std::cout << std::atomic_flag_test(&x) << std::endl;
}
```
  
출력  
<pre>
false
true
</pre>
  
  
## std::atomic_flag_test_explicit
현재 값을 bool 값으로 얻는다.  
memory order이 memory_order_release, memory_order_acq_rel 에서는 사용 불가.  
```
#include <iostream>
#include <atomic>

int main()
{
  std::cout << std::boolalpha;

  std::atomic_flag x = ATOMIC_FLAG_INIT;
  std::cout << std::atomic_flag_test_explicit(&x, std::memory_order::acquire) << std::endl;

  std::atomic_flag_test_and_set(&x, std::memory_order::release);
  std::cout << std::atomic_flag_test_explicit(&x, std::memory_order::acquire) << std::endl;
}
```  
  

## std::atomic_flag_wait
`atomic_wait`와 비슷하며, atomic_flag_notify_one(), atomic_flag_notify_all() 함수로 대기에서 깨어난다.  
```
class my_mutex {
  std::atomic_flag state_ = ATOMIC_FLAG_INIT; // clear:unlock, set:lock
public:
  void lock() noexcept {
    while (std::atomic_flag_test_and_set(&state_)) {
      std::atomic_flag_wait(&state_, true);
    }
  }

  void unlock() noexcept {
    std::atomic_flag_clear(&state_);
    std::atomic_flag_notify_one(&state_);
  }
};
```
  
  
## std::atomic_flag_wait_explicit
`std::atomic_flag_wait`와 같으면 momory order 지정이 가능.     
```
#include <iostream>
#include <atomic>
#include <thread>

class my_mutex {
  std::atomic_flag state_ = ATOMIC_FLAG_INIT; // clear:unlock, set:lock
public:
  void lock() noexcept {
    while (std::atomic_flag_test_and_set_explicit(&state_, std::memory_order::acquire)) {
      std::atomic_flag_wait_explicit(&state_, true, std::memory_order::relaxed);
    }
  }

  void unlock() noexcept {
    std::atomic_flag_clear_explicit(&state_, std::memory_order::release);
    std::atomic_flag_notify_one(&state_);
  }
};
```
  
  
## std::atomic_flag_notify_one
`atomic_flag_wait`에 의한 대기 상태에서 하나만 깨운다.  
  
  
## std::atomic_flag_notify_all
`atomic_flag_wait`에 의한 대기 상태에서 모두 깨운다.   
  
  
## 참고 사이트
- https://cpprefjp.github.io/reference/atomic.html

