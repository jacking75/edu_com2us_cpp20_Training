//https://zenn.dev/onihusube/articles/98a0fce9827fa87a88d3

// 인터페이스 파일은 1개만 있을 수 있다
// 다만 인터페이스를 몇개의 파일로 나누고 싶을 때는 파티션 으로 분할 할 수 있다.
// 같은 모듈에 속해 있는 인터페이스 파티션은 모두 프라이머리 인터페이스 단위에서 export 해야 한다

// mylib 모듈의 프라이머리 인터페이스 단위 선언
export module mylib;

// 모든 인터페이스 파티션의 재 export. 필수
export import :interface1;
export import :interface2;

