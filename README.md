# cpp-library

A small library of various C++ classes and functions.

## Requirements

Requires **C++11** and **Boost 1.64.0 with `Boost.Context`**.

## Usage

### Generators

```c++
#include <frxstrem/generator.hh>
using namespace frxstrem::generator;

/**
 * Simple generator.
**/
sequence<int> range(int start, int end) {
  // wrap inside a callable void(yield_t<int>)
  // which can be converted to sequence<int>
  return [=] (yield_t<int> yield) {
    for(int i = start; i < end; i++) {
      // return next value of sequence
      yield(i);
    }
  };
}

/**
 * Generator of pairs
**/
sequence<std::pair<int, int>> fib(int N) {
  return [=] (yield_t<std::pair<int, int>> yield) {
    int a = -1;
    int b = 1;

    for(int i = 0; i < N; i++) {
      int c = a + b;
      a = b;
      b = c;

      // yield can also be called with constructor arguments
      // of yielded type:
      //   std::pair<int, int>(i, c)
      yield(i, c);
    }
  };
}

/**
 * Yielding vectors
**/
sequence<int> vector2sequence(const std::vector<int> &vec) {
  return [=] (yield_t<int> yield) {
    // yield.all can be used to yield elements of any iterable
    // of a type convertible to the sequence type
    // (including other sequences)
    yield.all(vec);
  };
}

/**
 * void sequences
**/
sequence<void> voidseq(int N) {
  return [=] (yield_t<void> yield) {
    for(int i = 0; i < N; i++) {
      // yield can be used without arguments for void sequences
      yield();
    }
  };
}


int main() {
  // use sequence as an iterable in a for-loop
  for(auto x : range(0, 5))
    std::cout << x << " ";
  std::cout << std::endl;
  // output: 0 1 2 3 4


  // or use .begin() and .end() manually
  auto seq = vector2sequence(std::vector<int>{ 1, 2, 4, 7 });
  for(auto it = seq.begin(); it != seq.end(); ++it)
    std::cout << *it << " ";
  std::cout << std::endl;
  // output: 1 2 4 7


  // use void sequences
  for(auto _ : voidseq(5)) {
    // ...
  }
}
```
