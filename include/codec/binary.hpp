#ifndef SVT_CODEC
#define SVT_CODEC

#include <iostream>
#include <ostream>
#include <vector>

namespace codec {

enum class array_t { L8, L16 };

template <class T>
struct Array {
  std::vector<array_t> size_spec;
  T& value;
};

template <class T>
Array<T> array(T& v, std::vector<array_t> size_spec) {
  return Array<T>{size_spec, v};
}

/*
Hide the Array<T> type from the built-in cereal archives.
*/
template <class A, class T>
void save(A& a, Array<T> const& t) {
  save(a, t.value);
}

template <class A, class T>
void load(A& a, Array<T>& t) {
  load(a, t.value);
}

class BinaryOutputCodec {
 public:
  BinaryOutputCodec(std::ostream& stream) : _stream(stream) {}

  template <class T>
  void operator()(Array<T> a) {
    std::cout << "field: Array" << std::endl;
  }

  template <class T>
  void operator()(T& v) {
    std::cout << "field: catch-all" << std::endl;
    serialize(*this, v);
  }

  void operator()(uint8_t& v) {
    std::cout << "field: uint8_t" << std::endl;
    integer(v, 1);
  }

  void operator()(int8_t& v) {
    std::cout << "field: int8_t" << std::endl;
    integer(v, 1);
  }

  void operator()(uint16_t& v) {
    std::cout << "field: uint16_t" << std::endl;
    integer(v, 2);
  }

  void operator()(int16_t& v) {
    std::cout << "field: int16_t" << std::endl;
    integer(v, 2);
  }

  void operator()(uint32_t& v) {
    std::cout << "field: uint32_t" << std::endl;
    integer(v, 4);
  }

  void operator()(int32_t& v) {
    std::cout << "field: int32_t" << std::endl;
    integer(v, 4);
  }

  void operator()(uint64_t& v) {
    std::cout << "field: uint64_t" << std::endl;
    integer(v, 8);
  }

  void operator()(int64_t& v) {
    std::cout << "field: int64_t" << std::endl;
    integer(v, 8);
  }

  template <class T, std::size_t L>
  void operator()(std::array<T, L>& v) {
    std::cout << "field: std::array" << std::endl;
  }

  /*
  Default vectors assume uint8 prefix.
  */
  template <class T>
  void operator()(std::vector<T>& v) {
    std::cout << "field: vector" << std::endl;
    vector<T, uint8_t>(v);
  }

 private:
  template <class T, class SizeType>
  void vector(std::vector<T>& v) {
    SizeType size = v.size();
    (*this)(size);
  }

  template <class T>
  void integer(T& v, int byte_size) {
    for (int i = 0; i < byte_size; i++) {
      _stream.put((v >> i * 8) & 0xFF);
    }
  }

 private:
  std::ostream& _stream;
};
}  // namespace codec

#endif
