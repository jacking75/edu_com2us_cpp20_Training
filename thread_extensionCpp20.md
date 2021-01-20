# thread Extensions in C++20

  

## std::jthread
jthread 클래스는 thread과 마찬가지로 새로운 실행 스레드(thread of execution)(이하, 단순히 스레드 라고 한다)의 작성/대기/기타 작업을 수행하는 메커니즘을 제공한다. 이 클래스는 또한 실행중인 스레드에 대한 중지 요청을 다루는 기능과 자동으로 join 조작을 하는 기능을 제공한다.  
   

## std::thread 클래스와의 차이
- 중지 요청 기능: jthread는 자신과 연결된 스레드에 대한 중지 요청을 처리하는 방법을 제공한다. 이를 위해 `<stop_token>` 헤더에 정의된 클래스를 이용한다.
- 자동 join 기능: jthread는 소멸자와 무브 대입 연산자가 호출 될 때, 만약 자신과 관련된 thread가 존재하는 경우 스레드 중지 요청을 만들고, join()을 호출하여 스레드의 종료를 기다린다. 따라서 thread 클래스와 달리 `joinable() == true` 이라도 소멸자와 무브 대입 연산자를 호출 할 수 있으며, `std::terminate()`이 호출되지 않는다.
  
  
## 멤버 함수

| 이름            | 설명                                                               | 대응 버전 |
|-----------------|--------------------------------------------------------------------|----------------|
| (constructor)   | 생성자                                                     | C++20          |
| (destructor)    | 소멸자                                                       | C++20          |
| operator=       | 대입연산자                                                         | C++20          |
| swap            | 다른 jthread와 교환한다                                              | C++20          |
| joinable        | 스레드와 연결 되어 있는지 알아본다                       | C++20          |
| join            | 스레드가 종료할 때까지 기다린다                                     | C++20          |
| detach          | 스레드 관리를 벗어난다                                             | C++20          |
| get_id          | 연결된 스레드의 스레드 식별자를 얻는다               | C++20          |
| native_handle   | 스레드에 연결된 네이티브 스레드 핸들을 얻는다 | C++20          |
| get_stop_source | 정지 요구를 만들기 위해서 stop_source 오브젝트를 얻는다          | C++20          |
| get_stop_token  | 정지 상태를 기다리기 위해 stop_token 오브젝트를 얻는다       | C++20          |
| request_stop    | 스레드에 대한 정지 요구를 만든다                                 | C++20          |
   
## 정적 멤버 함수
- hardware_concurrency: 처리계에 따라서 지원되는 스레드 병렬 수를 얻는다  
  
## 멤버 타입
- id: 스레드 식별자(type-alias)  
- native_handle_type: 네이티브 핸들 타입(type-alias)   
  
## 비 멤버 함수
- swap: 2개의 jthread 오브젝트를 교환한다
  
## 예제

```
#include <cassert>
#include <thread>

int main()
{
	int x = 0, y = 0;

	std::jthread t([&](std::stop_token st) { x++; });

	// 스레드에 대한 정지 요구 작성에 맞추어서 호출된다
	// 콜백을 정의한다
	std::stop_callback sc{ t.get_stop_token(), [&] { y++; } };

	assert(y == 0);

	// 명시적으로 join()을 호출하지 않고 t를 덮어쓴다
	// 이 때 move 연산자 호출 중에서 자동으로 정기 요구 작성과 join() 호출을 한다.
	t = std::jthread{};

	assert(x == 1 && y == 1);

	return 0;
}
```    
  
```
#include <chrono>
#include <iostream>
#include <stop_token>  // NEW
#include <thread>
using namespace std::chrono_literals;  // ms, s

// 워커 스레드 처리 with 정지 토큰
void process(std::stop_token st, int n)
{
  // 정지 요구가 있을 때까지...
  while (!st.stop_requested()) {
    // 워커 스레드 처리를 게속한다
    std::this_thread::sleep_for(100ms);
    std::cout << n++ << " " << std::flush;
  }
  std::cout << "done!" << std::endl;
}

int main()
{
  // 워커 스레드 기동(jthread)
  std::jthread worker{ process, 0 };
  // worker에 연결되는 정지 토큰은 process의 첫번째 인수로 전달된다

  // 메인 스레드 처리: 여기에서는 sleep만 한다
  std::this_thread::sleep_for(1s);
  // 워커 스레드를 명시적으로 정지 하고 싶다면 worker.request_stop()을 호출한다.

  // worker 변수 스코프 종료에서 정지 토큰에 대해서 정지 요구가 자동 발행된다
  // 워커 스레드 종료할 때까지 메인 스레드는 대기(join)한다.
}
```
  

## std::stop_token
stop_token 클래스는 중지 요청이 작성 되었는지 여부 또는 중지 요청이 생성 될 수 있는 여부 등 정지 상태를 문의하기 위한 인터페이스를 제공한다.  
stop_source 클래스의 get_token() 멤버 함수를 호출하면, 해당 stop_source 클래스의 객체와 정지 상태를 공유하는 stop_token 클래스의 객체를 구축 할 수 있다. 따라서 stop_source측에서 중지 요청을 만들 때 이 stop_token을 통해 정지 상태를 문의 할 수 있다.  
  
또한 stop_token 클래스는 아래의 클래스에서도 이용된다:   
- stop_callback
    - 중지 요청에 따라 호 출되는 콜백을 등록 할 때 생성자에서 stop_token을 받는다.
- condition_variable_any
    - 인터럽트 가능한 대기 처리를 할 때 wait() 멤버 함수에서 stop_token을 받는다.
  
```
#include <cassert>
#include <stop_token>

int main()
{
  std::stop_source ss;
  std::stop_token st = ss.get_token();
  assert(st.stop_requested() == false);

  ss.request_stop();

  assert(st.stop_requested() == true);
}
```
  

## std::stop_source
stop_source 클래스는 중지 요청을 생성하기 위한 인터페이스를 제공한다.  
또한 자신과 정지를 공유하는 stop_token 클래스의 객체를 구축 할 수 있다.  
      
어떤 stop_source에 대해서 만든 중지 요청은 같은 정지 상태를 공유하는 것 외에 stop_source 이나 stop_token에서 보인다.  
단 중지 요청을 만들면 이것을 나중에 취소할 수는 없다. 또한 이후에 만든 정지 요구는 효과를 가지지 않는다.  
    
```
#include <cassert>
#include <stop_token>

int main()
{
  std::stop_source ss;
  std::stop_token st = ss.get_token();

  bool invoked = false;
  std::stop_callback cb {st, [&] { invoked = true; }};

  assert(st.stop_requested() == false);
  assert(invoked == false);

  ss.request_stop();

  assert(st.stop_requested() == true);
  assert(invoked == true);
}
```
   

## std::stop_callback  
stop_callback 클래스 템프릿은 정지 요구가 만들어질 때 호출되는 콜백을 뜻한다.  
    
```
#include <cassert>
#include <stop_token>
#include <string>

int main()
{
  std::string msg;

  std::stop_source ss;
  std::stop_token st = ss.get_token();
  std::stop_callback cb1(st, [&] { msg += "hello"; });

  assert(msg == "");

  ss.request_stop();

  // 정지 요구가 만들어지기 전에 등록 되어 있던 콜백 함수는 
  // 정지 요구가 만들어졌을 때 호출된다
  assert(msg == "hello");

  std::stop_callback cb2(st, [&] { msg += " world"; });

  // 정지 요구가 만들어진 후에 등록된 콜백 함수는 
  // std::stop_callback 클래스의 생성자 안에 즉각 호출된다
  assert(msg == "hello world");
}
```  
  

## std::nostopstate
nostopstate_t 타입과 이 값 nostopstate는 정지 상태를 취급하지 않는 stop_source를 구축하기 위한 태그이다.  
stop_source 클래스의 생성자에 nostopstate을 넘기면, 이 stop_source는 중지 요청을 처리하기 위한 자원을 확보하지 않고, 중지 요청을 만들거나 stop_token 클래스와 정지 상태를 공유 할 수 없는 상태가 된다.  
  
```
#include <cassert>
#include <stop_token>

int main()
{
  std::stop_source ss1;
  std::stop_source ss2(std::nostopstate);

  assert(ss1.stop_possible() == true);
  assert(ss2.stop_possible() == false);

  std::stop_token st1 = ss1.get_token();
  std::stop_token st2 = ss2.get_token();

  assert(st1.stop_possible() == true);
  assert(st2.stop_possible() == false);
}
```
  


## std::latch
latch 클래스는 래치 통과/도달 스레드가 기대하는 개수가 될 때까지 여러 스레드의 진행을 차단하는 스레드 조정 기구(동기화 프리미티브)이다.  
래치는 1개의 카운터로 상태 관리 되는 동기화 프리미티브로 간주한다.  
카운터는 0 이상, 초기 값 이하의 값을 취하고, 이 카운터 값을 래치 미 도달의 스레드 개수로 해석된다.  카운트 다운 래치 라고 부르기도 한다.  
- 생성자에서 카운터 값을 설정한다.
- count_down() 에 의해 카운터 값 빼기로 통지한다.(래치 통과)
- wait() 에 의해 카운터 값이 0 될 때까지 호출 스레드를 차단합니다. (래치 대기)
- arrive_and_wait()는 count_down() + wait()에 해당한다.(래치 도달 겸 대기)
    
래치 개체는 한 번만 사용할 수 있고, 카운터 값이 0에 도달한 객체의 재사용은 할 수 없다. 여러 스레드 간 동기화를 반복 취하는 경우는 barrier을 이용한다.  
latch는 카운터 값 0을 대기하지만, 유사한 기능으로 세마포어 counting_semaphore는 카운터 값이 0 보다 더 클 때까지 대기한다는 차이가 있다.  
   
### 멤버 함수
- count_down: 통과 통지
- try_wait: 카운터 값이 0인지 여부를 확인
- wait: 대기 처리
- arrive_and_wait: 도달 통지 및 대기 처리  
  

### 예제 코드
    
```
#include <iostream>
#include <latch>
#include <mutex>
#include <thread>

constexpr int NWORKER = 3;  // 워커 수

std::mutex cout_mtx;  // 행 단위 cout 출력용 뮤텍스

int main()
{
  // 1回만 초기화 되는 공유 데이터
  int shared_data = 0;

  // 초기화 끝는 래치: 초기 카운터 값은 1
  std::latch initialized{1};
  // 태스크 종료를 나타내는 래치: 초기 카운터 값=워커 스레드 수 + 1(메인 스레드)
  std::latch completed{NWORKER+1};

  // 워커 스레드 그룹을 Fire-and-Forget 기동
  for (int id = 1; id <= NWORKER; id++) {
    std::thread([&,id]{
      // 공유 데이터의 초기화 종료를 대기
      initialized.wait();
      // 메인 스레드 상에서 shared_data 대입 완료는 보증 되고
      // 이후는 shared_data 읽기만 하면 데이터 경합은 발생하지 않는다
      int local_data = shared_data;

      { // 워커 스레드의 태스크를 실행
        std::lock_guard lk{cout_mtx};
        std::cout << "Worker#" << id << ":" << local_data << std::endl;
      }

      // 태스크 종료를 통지
      completed.count_down();
      // 워커 스레드는 블럭 되지 않은 채로 종료한다
    }).detach();
  }

  // 공유 데이터 초기화
  shared_data = 42;

  // 공유 데이터 초기화 완룐을 워크 스레드 그룹에 통지
  initialized.count_down();
  // 메인 스레드는 블럭 되지 않고 계속 실행한다

  { // 메인 스레드의 태스크를 실행
    std::lock_guard lk{cout_mtx};
    std::cout << "Main:" << shared_data << std::endl;
  }

  // 메인 스러드의 태스크 완료 통지 겸 모든 워커 스레드의 태스크 완료 대기
  completed.arrive_and_wait();
}
```
  
<pre>
Worker#1:42
Worker#2:42
Main:42
Worker#3:42
</pre>  



## std::barrier
barrier 클래스는 여러 스레드에 의해 동시에 수행되는 반복적인 작업 제어에 편리한 스레드 조정기구(동기화 프리미티브)이다.  
베리어의 존속 기간은 배리어 페이즈 열로 구성되며, 각 단계에서 예정 스레드 수가 베리어에 도달 할 때까지 선행 도달 스레드의 실행을 차단 할 수있다.  
여러 스레드가 차단 개체에서 약속(합류)을 하는 모습에서 랑데부 포인트(Rendezvous Point)라고도 불린다.  
  
배리어 동기화를 이용하면 Fork-Join 모델을 기반으로 태스크 병행 구조를 쉽게 구현할 수 있다.  
- 생성자에서 배리어 동기화에 참여하는 스레드 수를 설정한다.
- arrive_and_wait()에 의해 자신 스레드의 현재 단계 완료를 통지하고, 다른 스레드 군의 현재 단계 완료를 대기한다. 배리어 동기화 참여 중인 스레드가 합류하고 나면 각 스레드 블록이 해제 되어 다음 단계를 시작한다.
- arrive_and_drop()에 의해 자신 스레드의 현재 단계 완료를 통지하고, 다음 단계에서 배리어 동기화에 참여하지 않는다(참여 스레드 수를 하나 감소). 배리어 동기화 참여 중인 스레드가 합류하고 나면 자신 스레드 이외의 블록이 해제 되어 다음 단계를 시작한다.
  
배리어 개체는 여러 스레드 사이의 합류 제어를 반복 할 수 있지만 한 번 제어으로 충분하다면 래치 latch도 이용 후보가 될 수 있다.    
    
```
#include <barrier>
#include <iostream>
#include <thread>

constexpr int NWORKERS = 2;  // 워커 수
constexpr int NPHASES  = 3;  // 페이즈 수

std::mutex cout_mtx;  // 행 단위 cout 출력 용 뮤텍스

// 이 프로그램에서는 (NWORKERS+1)*NPHASES = 9개의 태스크를 실행한다.
// 같은 페이즈에 속하는 태스크는 복수의 스레드 상에서 동시 병행으로 실행 된다
// 베리어에 의해 서로 다른 페이즈에 속하는 태스크가 동시 실행 되지 않는 것을 보증한다.
// 여기에서는 주기적으로 Fork-Join 모델 태스크 병행 실행을 한다
int main()
{
  // 베리어 주기: 초기 카운터 값=워커 수+1(메인 스레드)
  std::barrier<> sync{NWORKERS+1};

  // 워커 스레드 그룹을 Fire-and-Forget 기동
  for (int id = 1; id <= NWORKERS; id++) {
    std::thread([&,id]{
      for (int phase = 1; phase <= NPHASES; phase++) {
        { // 워커 스레드의 페이즈 태스크를 실행
          std::lock_guard lk{cout_mtx};
          std::cout << "Worker#" << id << " " << phase << std::endl;
        }

        // 합류 포인트: 메인 스레드 + 타 워커 스레드와 동기
        sync.arrive_and_wait();
      }
    }).detach();
  }

  // 메인 스레드 처리
  for (int phase = 1; phase <= NPHASES; phase++) {
    { // 메인 스레드의 페이즈 태스크를 실행
      std::lock_guard lk{cout_mtx};
      std::cout << "Main     " << phase << std::endl;
    }

    // 합류 포인트: 모든 워커 스레드와 동기
    sync.arrive_and_wait();
  }
}
```
   
<pre>
Worker#1 1
Main     1
Worker#2 1
Main     2
Worker#2 2
Worker#1 2
Worker#1 3
Worker#2 3
Main     3
</pre>  
    
  
   
## 참고 사이트
- [Concurrency in C++20 and beyond (pdf)](https://github.com/CppCon/CppCon2019/blob/master/Presentations/concurrency_in_cpp20_and_beyond/concurrency_in_cpp20_and_beyond__anthony_williams__cppcon_2019.pdf  ) 
- https://cpprefjp.github.io/reference/thread/jthread.html  
- https://cpprefjp.github.io/reference/stop_token.html

