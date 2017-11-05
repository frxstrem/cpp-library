#ifndef FRXSTREM_GENERATOR_HH
#define FRXSTREM_GENERATOR_HH

#include <functional>
#include <memory>
#include <utility>
#include <type_traits>
#include <stdexcept>
#include <boost/context/all.hpp>

namespace frxstrem {
namespace generator {

// forward declarations
template <typename> class yield_t;
template <typename> class sequence;

// empty class
struct empty_t final {
  constexpr operator bool() const { return false; }
};

template <typename T>
class yield_t {
public:
  using value_t = typename sequence<T>::value_t;

private:
  sequence<T> &seq;
  boost::context::continuation &sink;

  yield_t(sequence<T> &seq, boost::context::continuation &sink);

public:
  value_t &state();
  const value_t &state() const;

  template <typename... Args>
  void operator ()(Args&&... args) { this->operator()(value_t(std::forward<Args>(args)...)); }

  void operator ()(const value_t &value);
  void operator ()(value_t &&value);

  template <typename Iterable>
  void all(Iterable &&x) {
    for(T value : x)
      this->operator()(value);
  }

  template <typename> friend class sequence;
};

template <typename T>
class sequence {
public:
  class iterator;
  using callback_t = std::function<void(yield_t<T>)>;
  using value_t = typename std::conditional<std::is_void<T>::value, empty_t, T>::type;

private:
  std::shared_ptr<boost::context::continuation> sourceptr = nullptr;
  value_t state;
  bool begun = false, done = false;

  value_t &get() { return state; }
  const value_t &get() const { return state; }

  void next() { *sourceptr = sourceptr->resume(); }

public:
  sequence(callback_t cb);

  template <typename Callback>
  sequence(Callback cb) : sequence(callback_t(cb)) { }

  iterator begin() {
    if(!begun)
      next();
    return iterator(*this, done);
  }
  iterator end() { return iterator(*this, true); }

  class iterator {
  private:
    sequence &seq;
    bool end;

    iterator(sequence &seq, bool end) : seq(seq), end(end) { }

  public:
    value_t &operator *() {
      if(end) {
        throw std::runtime_error("Cannot dereference end iterator (frxstrem::generator::sequence::iterator)");
      }

      return seq.get();
    }

    iterator &operator ++() {
      if(end) {
        throw std::runtime_error("Cannot increment end iterator (frxstrem::generator::sequence::iterator)");
      }

      seq.next();
      if(seq.done) {
        end = true;
      }
    }

    bool operator ==(const iterator &it) const {
      return (&seq == &it.seq && end == it.end);
    }

    bool operator !=(const iterator &it) const { return !(*this == it); }

    friend class sequence;
  };

  template <typename> friend class yield_t;
};

template <typename T>
yield_t<T>::yield_t(sequence<T> &seq, boost::context::continuation &sink) : seq(seq), sink(sink) { }

template <typename T>
typename yield_t<T>::value_t &yield_t<T>::state() { return seq.get(); }

template <typename T>
const typename yield_t<T>::value_t &yield_t<T>::state() const { return seq.get(); }

template <typename T>
void yield_t<T>::operator ()(const value_t &value) {
  seq.get() = value;
  sink = sink.resume();
}

template <typename T>
void yield_t<T>::operator ()(value_t &&value) {
  seq.get() = std::move(value);
  sink = sink.resume();
}

template <typename T>
sequence<T>::sequence(callback_t cb) {
  sourceptr = std::make_shared<boost::context::continuation>(
    boost::context::callcc([this, cb] (boost::context::continuation &&sink) {
      yield_t<T> yield(*this, sink);
      sink = sink.resume();
      this->begun = true;

      cb(yield);

      this->done = true;
      return std::move(sink);
    })
  );
}

}}

#endif // FRXSTREM_GENERATOR_HH
