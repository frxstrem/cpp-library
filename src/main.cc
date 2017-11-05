#include <iostream>
#include <vector>

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
      // of yielded type
      yield(i, c);
    }
  };
}

/**
 * Yielding vectors
**/
sequence<int> vector2sequence(const std::vector<int> &vec) {
  return [=] (yield_t<int> yield) {
    // yield* can be used to yield elements of any iterable
    // of a type convertible to the sequence type
    // (including other sequences)
    yield*(vec);
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
  auto seq = range(5, 10);
  for(auto it = seq.begin(); it != seq.end(); ++it)
    std::cout << *it << " ";
  std::cout << std::endl;
  // output: 5 6 7 8 9


  // use void sequences
  for(auto _ : voidseq(5)) {
    // ...
  }
}
