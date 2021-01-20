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

https://cpprefjp.github.io/reference/atomic/atomic_wait.html


## std::atomic_wait_explicit

https://cpprefjp.github.io/reference/atomic/atomic_wait_explicit.html

## std::atomic_notify_one

https://cpprefjp.github.io/reference/atomic/atomic_notify_one.html

## std::atomic_notify_all

https://cpprefjp.github.io/reference/atomic/atomic_notify_all.html


## std::atomic_flag_test

https://cpprefjp.github.io/reference/atomic/atomic_flag_test.html


## std::atomic_flag_test_explicit

https://cpprefjp.github.io/reference/atomic/atomic_flag_test_explicit.html


## std::atomic_flag_wait

https://cpprefjp.github.io/reference/atomic/atomic_flag_wait.html


## std::atomic_flag_wait_explicit

  https://cpprefjp.github.io/reference/atomic/atomic_flag_wait_explicit.html


## std::atomic_flag_notify_one

https://cpprefjp.github.io/reference/atomic/atomic_flag_notify_one.html


## std::atomic_flag_notify_all
 
 https://cpprefjp.github.io/reference/atomic/atomic_flag_notify_all.html 



## 참고 사이트
- https://cpprefjp.github.io/reference/atomic.html

