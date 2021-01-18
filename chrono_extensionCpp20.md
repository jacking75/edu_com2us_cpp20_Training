# chrono Extensions in C++20
  
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
  	  
예제 코드	  
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
  
  
## 참고 사이트
- http://d.hatena.ne.jp/yohhoy/20180320/p1 
- http://d.hatena.ne.jp/yohhoy/20180322/p1 
- http://d.hatena.ne.jp/yohhoy/20180326/p1 
- https://qiita.com/yohhoy/items/8e751b019b369693f0f3 