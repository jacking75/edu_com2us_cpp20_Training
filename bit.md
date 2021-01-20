# std::bit
`<bit>` 헤더를 include 한다.  
  
## 변환  
bit_cast: 비트 레벨 재해석 캐스트  
```
float f = 3.14f;
std::uint32_t n = std::bit_cast<std::uint32_t>(f);
```  


## 2의 거듭제곱 정수
  
### has_single_bit
1비트만 1로 되어 있는가를 판정한다  
  
```
#include <iostream>
#include <bit>

void check_pow2(unsigned int x)
{
  std::cout << x << "\t : " << std::has_single_bit(x) << std::endl;
}

int main()
{
  std::cout << std::boolalpha;

  if (std::has_single_bit(128u)) {
    std::cout << "128 is power of 2" << std::endl;
  }
  check_pow2(0u);
  check_pow2(3u);
  check_pow2(0xffu);

  std::cout << "---" << std::endl;

  check_pow2(1u);
  for (unsigned int i = 2u; i <= 1024u; i *= 2) {
    check_pow2(i);
  }
}
```    
결과   
<pre>
128 is power of 2
0    : false
3    : false
255  : false
---
1    : true
2    : true
4    : true
8    : true
16   : true
32   : true
64   : true
128  : true
256  : true
512  : true
1024     : true
</pre>

### bit_ceil 
정수 값을 2의 거듭제곱 값으로 올림한다  
```
#include <iostream>
#include <bit>

void convert_to_pow2(unsigned int x)
{
  std::cout << x << "\t : " << std::bit_ceil(x) << std::endl;
}

int main()
{
  std::cout << "127\t : " << std::bit_ceil(127u) << std::endl;

  for (unsigned int i = 0u; i <= 32u; ++i) {
    convert_to_pow2(i);
  }
}
```
    
<pre>
127  : 128
0    : 1
1    : 1
2    : 2
3    : 4
4    : 4
5    : 8
6    : 8
7    : 8
8    : 8
9    : 16
10   : 16
11   : 16
12   : 16
13   : 16
14   : 16
15   : 16
16   : 16
17   : 32
18   : 32
19   : 32
20   : 32
21   : 32
22   : 32
23   : 32
24   : 32
25   : 32
26   : 32
27   : 32
28   : 32
29   : 32
30   : 32
31   : 32
32   : 32
</pre>
  

### bit_floor
정수 값을 2의 거듭제곱 값으로 내림한다  
```
#include <iostream>
#include <bit>

void convert_to_pow2(unsigned int x)
{
  std::cout << x << "\t : " << std::bit_floor(x) << std::endl;
}

int main()
{
  std::cout << "129\t : " << std::bit_floor(129u) << std::endl;

  for (unsigned int i = 0u; i <= 34u; ++i) {
    convert_to_pow2(i);
  }
}  
```
  
<pre>
129  : 128
0    : 0
1    : 1
2    : 2
3    : 2
4    : 4
5    : 4
6    : 4
7    : 4
8    : 8
9    : 8
10   : 8
11   : 8
12   : 8
13   : 8
14   : 8
15   : 8
16   : 16
17   : 16
18   : 16
19   : 16
20   : 16
21   : 16
22   : 16
23   : 16
24   : 16
25   : 16
26   : 16
27   : 16
28   : 16
29   : 16
30   : 16
31   : 16
32   : 32
33   : 32
34   : 32
</pre>    
 
  
### bit_width 
값을 표현하기 위해 필요한 비트 폭을 구한다
    
```  
#include <iostream>
#include <bit>

void convert_to_width(unsigned int x)
{
  std::cout << x << "\t : " << std::bit_width(x) << std::endl;
}

int main()
{
  std::cout << "129\t : " << std::bit_width(129u) << std::endl;
  convert_to_width(127u);
  convert_to_width(1u);
  convert_to_width(0u);

  std::cout << "---" << std::endl;
  for (unsigned int i = 1024u; i >= 2u; i /= 2) {
    convert_to_width(i);
  }
}
```
  
<pre>
129  : 8
127  : 7
1    : 1
0    : 0
---
1024     : 11
512  : 10
256  : 9
128  : 8
64   : 7
32   : 6
16   : 5
8    : 4
4    : 3
2    : 2
</pre>    
  


## 순환 비트 시프트
  
### rotl
왼쪽 순환 시프트  
  
```
#include <cassert>
#include <bit>
#include <cstdint>

int main()
{
  auto i = static_cast<std::uint32_t>(0b0001'0000'0000'0000'0000'0000'0000'0000u);
  std::uint32_t a = std::rotl(i, 4);
  assert(a == 0b0000'0000'0000'0000'0000'0000'0000'0001u);

  std::uint32_t b = std::rotl(i, -4);
  assert(b == 0b0000'0001'0000'0000'0000'0000'0000'0000u);
}  
```
  

### rotr
오른쪽 순환 시프트  
  
```
#include <cassert>
#include <bit>
#include <cstdint>

int main()
{
  auto i = static_cast<std::uint32_t>(0b0000'0000'0000'0000'0000'0000'0000'1000u);
  std::uint32_t a = std::rotr(i, 4);
  assert(a == 0b1000'0000'0000'0000'0000'0000'0000'0000u);

  std::uint32_t b = std::rotr(i, -4);
  assert(b == 0b0000'0000'0000'0000'0000'0000'1000'0000u);
}
```
     

  
## 비트 카운트
  
### countl_zero
왼쪽에서 연속한 0의  비트를 계산한다  
  
```
#include <cassert>
#include <bit>
#include <cstdint>

int main()
{
  auto i = static_cast<std::uint32_t>(0b0001'0000'0000'0000'0000'0000'0000'0000u);
  int n = std::countl_zero(i);
  assert(n == 3);


  assert(std::countl_zero(static_cast<std::uint32_t>(0b1000'0000'0000'0000'0000'0000'0000'0000u)) == 0);
  assert(std::countl_zero(static_cast<std::uint32_t>(0b0100'0000'0000'0000'0000'0000'0000'0000u)) == 1);
  assert(std::countl_zero(static_cast<std::uint32_t>(0u)) == 32);
}  
```
    

### countl_one
왼쪽에서 연속한 1의 비트를 계산한다  
  
```  
#include <cassert>
#include <bit>
#include <cstdint>

int main()
{
  auto i = static_cast<std::uint32_t>(0b1110'0000'0000'0000'0000'0000'0000'0000u);
  int n = std::countl_one(i);
  assert(n == 3);

  i = static_cast<std::uint32_t>(0b1110'0000'0000'0000'0000'0000'0000'0001u);
  n = std::countl_one(i);
  assert(n == 3);

  assert(std::countl_one(static_cast<std::uint32_t>(0u)) == 0);
  assert(std::countl_one(static_cast<std::uint32_t>(0b1000'0000'0000'0000'0000'0000'0000'0000u)) == 1);
  assert(std::countl_one(static_cast<std::uint32_t>(0b1111'1111'1111'1111'1111'1111'1111'1111u)) == 32);
}
```
  

### countr_zero
오른쪽에서 연속한 0의  비트를 계산한다  
   
```
#include <cassert>
#include <bit>
#include <cstdint>

int main()
{
  auto i = static_cast<std::uint32_t>(0b0000'0000'0000'0000'0000'0000'0000'1000u);
  int n = std::countr_zero(i);
  assert(n == 3);

  assert(std::countr_zero(static_cast<std::uint32_t>(0b0000'0000'0000'0000'0000'0000'0000'0001u)) == 0);
  assert(std::countr_zero(static_cast<std::uint32_t>(0b0000'0000'0000'0000'0000'0000'0000'0010u)) == 1);
  assert(std::countr_zero(static_cast<std::uint32_t>(0u)) == 32);
}  
```
    
   
### countr_one
오른쪽에서 연속한 1의  비트를 계산한다  
  
```
#include <cassert>
#include <bit>
#include <cstdint>

int main()
{
  auto i = static_cast<std::uint32_t>(0b0000'0000'0000'0000'0000'0000'0000'0111u);
  int n = std::countr_one(i);
  assert(n == 3);

  assert(std::countr_one(static_cast<std::uint32_t>(0u)) == 0);
  assert(std::countr_one(static_cast<std::uint32_t>(0b0000'0000'0000'0000'0000'0000'0000'0001u)) == 1);
  assert(std::countr_one(static_cast<std::uint32_t>(0b1111'1111'1111'1111'1111'1111'1111'1111u)) == 32);
}  
```
  
  
### popcount
1로 된 비트를 계산한다  
  
```
#include <cassert>
#include <bit>
#include <cstdint>

int main()
{
  auto i = static_cast<std::uint32_t>(0b1000'0000'0000'1010'0000'0000'0000'1000u);
  int n = std::popcount(i);
  assert(n == 4);
}  
```
  
    
  
## 참고 사이트
- https://cpprefjp.github.io/reference/bit.html  
- [<bit> in Visual Studio 2019 version 16.8 Preview 2](https://devblogs.microsoft.com/cppblog/bit-in-visual-studio-2019-version-16-8-preview-2/ )  