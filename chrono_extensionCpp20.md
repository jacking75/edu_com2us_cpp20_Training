# chrono Extensions in C++20
[예제 코드](https://wandbox.org/permlink/HTrpTtutkN3FKkOq )
    
## C++20 `chrono`에 추가된 시계(Clock) 클래스  
  
| Clock                 | 개요                            | 기점(epoch)                  | 윤초 |
|-----------------------|---------------------------------|------------------------------|----------|
| system_clock          | 시스템 클럭［C++11］       | 1970-01-01 00:00:00 UTC      | 제외     |
| utc_clock             | 협정 세계 시(UTC)クロック         | 1970-01-01 00:00:00 UTC      | 포함   |
| tai_clock             | 국제 원자 시(TAI)クロック         | 1958-01-01 00:00:00 TAI*1    | 삽입 없음 |
| gps_clock             | GPS 클럭                     | 1980-01 첫번째 요일 00:00:00 UTC | 삽입 없음 |
| file_clock            | 파일 시스템 클럭        | 미 규정(unspecified)          | −        |
| steady_clock          | 시간 역행 없는 클럭［C++11］ | −                            | −        |
| high_resolution_clock | 고분해 클럭［C++11］       | −                            | −        |
    
TAI 클럭과 GPS 클럭은 UTC로의 윤초(leap second)이 삽입 될 때마다 1초 선행으로 간다.  
2000-01-01 00:00:00 UTC == 2000-01-01 00:00:32 TAI, 2000-01-01 00:00:00 UTC == 2000-01-01 00:00:13 GPS  
     
또 “타입 T가 Clock 개요를 만족하는지"를 판정하는 메타 함수 std::chrono::is_clock<T>, std::chrono::is_clock_v<T>도 같이 추가 되었다.  
  
  
## Calendar
- 타입 안전(Type safety)： 년(year), 월(month), 일(day), 년일월(year_month_day)을 타입 시스템 상에서 표현한다. 수치에서 암무적 변환은 금지.
    - 예: 2020년7월24일은 `year_month_day{2020, 7, 24}` 가 아닌 `year_month_day{year{2020}, month{7}, day{24}}` 처럼 기술한다.
- 기존 chrono 라이브러리와의 통합: 년월일(year_month_day) 과 시스템 시각(system_clock::time_point)은 상호 변환 가능. 년월일은 필드 단순 보존, 시스템 시각은 실존하는 Time point을 나타낸다.
    - 예 1: `auto tp = sys_days{2020y/July/24} + 9h + 30min + 15s; year_month_day ymd{floor(tp)};`
    - 예 2: 2018년3월32일은 2018년4월1일 `sys_days{2018y/3/32} == sys_days{2018y/4/1}`
- 날짜 리터럴 표기 순서는 년/월/일, 월/일/년［아메리카 식］,  일/월/년［영국 식]을 지원한다.
    - 예: 2020년7월24일은 2020y/July/24, 2020y/7/24, July/24/2020, 24d/July/2020, 24d/7/2020
- 요일(weekday), 최종일(last_spec)을 이용하여 “어느 월의 최종일”, “어느 월의 N X요일”, “어느 월의 마지막 X요일”도 표현 가능
    - 예:  2020년7월의 마지막일 `2020y/7/last == 2020y/7/31`, 2020년7월의 첫번째 월요일 `2020y/7/Monday[1] == 2020y/7/6`, 2020년7월의 마지막 월요일 `2020y/7/Monday[last] == 2020y/7/27`
- 날짜 계산 지원: N년(years), N월(months), N주(weeks), N일(days) 전/후를 계산 가능.
    - 주의:  1년 `years{1}`은 146097/400 日(≒365.24日), 1개월 `months{1}`는 1/12 년(≒30.43日)의 Duration 이라고 정의된다. 년월일(year_month_day) 또는 시스템 시각(Time point) 어느쪽의 타입으로 계산하는가로 결과가 달라진다.
    - 예: 3개월 전(2020-04-24) `2020y/7/24 - months{3}`, 16일후(2020-08-09) `sys_days{2020y/7/24} + days{16}`
- IO 스트림 출력(operator<<), 서식 지정 출력(to_stream), 서식 지정 입력(from_stream)을 추가 제공
    - 예: `std::cout << 2020y/7/24;` 는 "2020-07-24"을 출력한다.
- Gregorian calendar만 지원. 요건을 만족한다면 독자 달력과 상호 운용 가능
    - (당연하지만) 나라・지역・문화에서 서로 다른 “축일” 지원은 없다.
  	  
예제 코드. 2021.01.18. 아직 컴파일러에서 `std::ostream`에서 `std::chrono::year_month`를 지원하지 않음	        
  
```
#include <chrono>
#include <iostream>
#include <iomanip>

void print_calendar(std::ostream& os, const std::chrono::year_month& ym)
{
    using namespace std::chrono;

    unsigned weekday_offset{ weekday{sys_days{ym/1}}.c_encoding() };  // P1466R3
    unsigned lastday_in_month{ (ym/last).day() };

    os << "      " << ym << "\n"
           << "Su Mo Tu We Th Fr Sa\n";
  
    unsigned wd = 0;
    while (wd++ < weekday_offset)
        os << "   ";

    for (unsigned d = 1; d <= lastday_in_month; ++d, ++wd)
        os << std::setw(2) << d << (wd % 7 == 0 ? '\n' : ' ');
}

int main()
{
    using namespace std::chrono_literals;
    print_calendar(std::cout, 2018y/3);
}
```  
  
출력:     
<pre>
      2018/Mar
Su Mo Tu We Th Fr Sa
             1  2  3
 4  5  6  7  8  9 10
11 12 13 14 15 16 17
18 19 20 21 22 23 24
25 26 27 28 29 30 31
</pre>  
  
  

## Time Zone  
`<chrono>` 헤더에 타임존(Time Zone) 지원이 추가 되었다.  
- 타임존 시각(zoned_time)＝타임존 지정＋ 로컬 시각(Time Point)
    - 예: `auto zt = zoned_time{"Asia/Tokyo", local_days{2020y/7/24} + 13h};`
- 타임존 지정＝타임존 이름 or 타임존(time_zone) 구조체. 현 타임존 취득(current_zone())나 타임존 검색(locate_zone())을 제공.
    - 예: `auto zt_now = zoned_time{current_zone(), system_clock::now()};`
- UTC⇔타임존 시각 상호 호환, 서로 다른 시간 간의 상호 호환 지원.
    - 예 1: `auto zt_tokyo = zoned_time{"Asia/Tokyo", system_clock::now()};`
    - 예 2: `auto tp_utc = system_clock::time_point{zt_tokyo};`
    - 예 3: `auto zt_paris = zoned_time{"Europe/Paris", zt_tokyo};`
- 섬머 타임(Daylight Saving Time) 지원.
    - 섬머 타임 시작/종료 근처에서 “존재하지 않는 로컬 시간”、“중복 로컬 시간” 이라는 개념이 생기기 때문에 예외 `nonexistent_local_time`, `ambiguous_local_time`을 제공
    - “중복 로컬 시간” 에서 UTC로 변경 시에는 `choose::earliest`, `choose::latest` 명시 지정으로 예외발생을 회피 할 수 있다
- 타임존 데이터베이스(tzdb)는 라이브러리 내장. 타임존 정보・윤초(leap seconds) 정보에서 구성된다.
    - 타임존 데이터베이스 ・리스트(tzdb_list)를 싱글톤 제공
    - 타임존 데이터베이스 동적 갱신 지원
      

- (working)C++20標準ライブラリ <chrono>ヘッダ Tips  https://qiita.com/yohhoy/items/8e751b019b369693f0f3  



## 예제 코드
  
### 현재 시간 출력
    
```
#include <chrono>
#include <iostream>

auto now_utc = std::chrono::system_clock::now();
std::cout << now_utc << std::endl;
// 출력 예: 2018-04-01 01:23:45.678901 UTC

auto now_local = std::chrono::zoned_time{std::chrono::current_zone(), now_utc};
std::cout << now_local << std::endl;
// 출력 예: 2018-04-01 10:23:45.678901 JST

auto now_jst = std::chrono::zoned_time{"Asia/Tokyo", now_utc};
std::cout << now_jst << std::endl;
// 출력 예: 2018-04-01 10:23:45.678901 JST

// ISO 8601 형식
std::cout << format("%FT%RZ", now_utc) << std::endl;
// 출력 예: 2018-04-01T01:23Z
std::cout << format("%FT%TZ", floor<std::chrono::seconds>(now_utc)) << std::endl;
// 출력 예: 2018-04-01T01:23:45Z
std::cout << format("%FT%TZ", now_utc) << std::endl;
// 출력 예: 2018-04-01T01:23:45.678901Z

std::cout << format("%FT%T%Ez", now_jst) << std::endl;
// 출력 예: 2018-04-01T10:23:45.678901+09:00
```
    
  
### 년월일시분초 필드 분해
  
```
#include <cassert>
#include <chrono>

// 2000-09-01 23:45:56 UTC
using namespace std::chrono_literals;  // y,d,h,min,s
using std::chrono::September;
auto tp = std::chrono::sys_days{2000y/September/1d} + 23h + 45min + 56s;

auto dp = floor<std::chrono::days>(tp);

// 년월일
auto ymd = std::chrono::year_month_day{dp};
assert(ymd.year()  == std::chrono::year{2000});
assert(ymd.month() == std::chrono::month{9});
assert(ymd.day()   == std::chrono::day{1});

// 시분초
auto hms = std::chrono::hh_mm_ss{tp - dp};
assert(hms.hours()   == std::chrono::hours{23});
assert(hms.minutes() == std::chrono::minutes{45});
assert(hms.seconds() == std::chrono::seconds{56});
```
   
  
### 날짜 정규화 
  
```
#include <cassert>
#include <chrono>

using namespace std::chrono_literals;  // y
using std::chrono::year_month_day, std::chrono::sys_days;
// 2018-03-32 == 2018-04-01
year_month_day canonical_ymd = sys_days{ 2018y/3/32 };
assert( canonical_ymd == 2018y/4/1 );
```
  

### 요일 계산

```
#include <cassert>
#include <chrono>

using namespace std::chrono_literals;  // y
using std::chrono::weekday, std::chrono::sys_days;
// 2018-04-01 == 일요일(0 or 7)
auto wd = weekday{sys_days{2018y/4/1}};
assert( wd == std::chrono::Sunday );
assert( wd.c_encoding() == 0 );
assert( wd.iso_encoding() == 7 );
```
   
  
### 日数計算
  
```
#include <cassert>
#include <chrono>

using namespace std::chrono_literals;  // y
using std::chrono::sys_days;
// 기간(일수)
auto olympic_days = 1 + (sys_days{2020y/8/9} - sys_days{2020y/7/24}).count();
assert( olympic_days == 17 );

// 1000일전의 날짜
auto olympic_pre1000d = sys_days{2020y/7/24} - std::chrono::days{1000};
assert( olympic_pre1000d == 2017y/10/28 );
```
   

### 만년 달력
   
```
#include <chrono>
#include <iostream>
#include <iomanip>

void print_calendar(std::ostream& os, const std::chrono::year_month& ym)
{
  using namespace std::chrono;

  unsigned weekday_offset{ weekday{sys_days{ym/1}}.c_encoding() };
  unsigned lastday_in_month{ (ym/last).day() };

  os << "      " << ym << "\n"
     << "Su Mo Tu We Th Fr Sa\n";
  unsigned wd = 0;
  while (wd++ < weekday_offset)
    os << "   ";
  for (unsigned d = 1; d <= lastday_in_month; ++d, ++wd)
    os << std::setw(2) << d << (wd % 7 == 0 ? '\n' : ' ');
}

int main()
{
  using namespace std::chrono_literals;
  print_calendar(std::cout, 2018y/3);
}
```  
  
<pre>  
/*
      2018/Mar
Su Mo Tu We Th Fr Sa
             1  2  3
 4  5  6  7  8  9 10
11 12 13 14 15 16 17
18 19 20 21 22 23 24
25 26 27 28 29 30 31
*/
</pre>
  

### 년내 진행률 계산
  
```
#include <chrono>

double year_progress()
{
  using namespace std::chrono;

  auto today = floor<days>(system_clock::now());
  auto this_year = year_month_day{today}.year();

  auto elapsed_days = (today - sys_days{this_year/1/1}).count();
  auto days_in_year = 1 + (sys_days{this_year/12/31} - sys_days{this_year/1/1}).count();

  return (elapsed_days * 100. / days_in_year);
}

// 2018-03-27 현재는 85/365=23.2877%
```
  

### 윤초년 판전
  
```
#include <cassert>
#include <chrono>

inline
bool is_uruu(int y)
{
  return std::chrono::year{y}.is_leap();
}

assert(  is_uruu(2000) );
assert( !is_uruu(2011) );
assert(  is_uruu(2016) );
assert( !is_uruu(2018) );
```
  

### 어떤 년도의 7월 3번째 월요일 날짜 계산
  
```
#include <cassert>
#include <chrono>

std::chrono::year_month_day
marine_day(std::chrono::year y)
{
  using std::chrono::Monday;
  // 7월 3번째 월요일
  return std::chrono::sys_days{ y/7/Monday[3] };
}

using namespace std::chrono_literals;  // y
assert( marine_day(2018y) == 2018y/7/16 );
``` 
  
  
### 어떤 월의 마지막 금요일 날짜
  
```
#include <cassert>
#include <chrono>

std::chrono::year_month_day
premium_friday(std::chrono::year_month ym)
{
  using std::chrono::last;
  using std::chrono::Friday;
  // 마지막 금요일
  return std::chrono::sys_days{ ym/Friday[last] };
}

using namespace std::chrono_literals;  // y,d
assert( premium_friday(2018y/4) == 2018y/4/27d );
```  
  
  
  
## 참고 사이트
- https://cpprefjp.github.io/reference/chrono.html
- http://d.hatena.ne.jp/yohhoy/20180320/p1 
- http://d.hatena.ne.jp/yohhoy/20180322/p1 
- http://d.hatena.ne.jp/yohhoy/20180326/p1 
- https://qiita.com/yohhoy/items/8e751b019b369693f0f3 