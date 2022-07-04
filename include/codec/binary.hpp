#ifndef SVT_CODEC
#define SVT_CODEC

#include <algorithm>
#include <iostream>
#include <iterator>
#include <ostream>
#include <vector>

namespace codec {

enum class prefix_t { L8, L16 };

template <class T>
struct ArraySizePrefix {
  std::vector<prefix_t> size_spec;
  T& value;
};

struct StringSizePrefix {
  prefix_t size_spec;
  std::string& value;
};

template <class T>
ArraySizePrefix<T> array(T& v, std::vector<prefix_t> size_spec) {
  return ArraySizePrefix<T>{size_spec, v};
}

StringSizePrefix string(std::string& v, prefix_t size_spec) {
  return StringSizePrefix{size_spec, v};
}

/*
Hide the *SizePrefix type from the built-in cereal archives.
*/
template <class A, class T>
void save(A& a, ArraySizePrefix<T> const& t) {
  a(t.value);
}

template <class A, class T>
void load(A& a, ArraySizePrefix<T>& t) {
  a(t.value);
}

template <class A>
void save(A& a, StringSizePrefix const& t) {
  a(t.value);
}

template <class A>
void load(A& a, StringSizePrefix& t) {
  a(t.value);
}

class BinaryOutputCodec {
 public:
  BinaryOutputCodec(std::ostream& stream) : _stream(stream) {}

  /*
    unroll functions used to handle nested arrays with length-prefix
    specifiers.
  */
  template <class T>
  void unroll(T& v, std::vector<prefix_t>& r) {
    // no-op. Used to satisfy the compiler.
  }

  template <class T>
  void unroll(std::vector<T>& v, std::vector<prefix_t>& r) {
    std::vector<prefix_t> remain;
    std::copy(r.begin() + 1, r.end(), std::back_inserter(remain));

    if (r.size() > 0) {
      switch (r[0]) {
        case prefix_t::L8: {
          uint8_t size = v.size();
          (*this)(size);
          break;
        }
        case prefix_t::L16: {
          uint16_t size = v.size();
          (*this)(size);
          break;
        }
      }
    }

    for (int i = 0; i < v.size(); i++) {
      if (remain.size() == 0) {
        (*this)(v[i]);
      } else {
        unroll(v[i], remain);
      }
    }
  }

  /*
    Arrays with specific length-prefix.
  */
  template <class T>
  void operator()(ArraySizePrefix<T> a) {
    unroll(a.value, a.size_spec);
  }

  /*
    Strings with specific length-prefix
  */
  void operator()(StringSizePrefix a) {
    std::cout << "field: StringSizePrefix" << std::endl;
    switch (a.size_spec) {
      case prefix_t::L8:
        string<uint8_t>(a.value);
        break;
      case prefix_t::L16:
        string<uint16_t>(a.value);
        break;
    }
  }

  /*
    Generic field.
  */
  template <class T>
  void operator()(T& v) {
    serialize(*this, v);
  }

  /*
   Primitive specializations
  */
  void operator()(uint8_t& v) { integer(v, 1); }
  void operator()(int8_t& v) { integer(v, 1); }
  void operator()(uint16_t& v) { integer(v, 2); }
  void operator()(int16_t& v) { integer(v, 2); }
  void operator()(uint32_t& v) { integer(v, 4); }
  void operator()(int32_t& v) { integer(v, 4); }
  void operator()(uint64_t& v) { integer(v, 8); }
  void operator()(int64_t& v) { integer(v, 8); }

  /*
  std::string
  */
  void operator()(std::string& v) { string<uint8_t>(v); }

  /*
  std::array
  */
  template <class T, std::size_t L>
  void operator()(std::array<T, L>& v) {
    for (int i = 0; i < v.size(); i++) {
      (*this)(v[i]);
    }
  }

  /*
    std::vector
    */
  template <class T>
  void operator()(std::vector<T>& v) {
    uint8_t size = v.size();
    (*this)(size);
    for (int i = 0; i < v.size(); i++) {
      (*this)(v[i]);
    }
  }

 private:
  template <class SizeType>
  void string(std::string& v) {
    SizeType size = v.length();
    (*this)(size);
    _stream << v;
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
