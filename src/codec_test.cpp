
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cstdint>
#include <string>
#include <vector>

#include "codec/binary.hpp"

namespace {
struct Box {
  uint32_t number;
  std::vector<std::array<uint16_t, 8>> nested_vector_array;
};

/*
Cereal signature.
*/
template <class Archive>
void serialize(Archive& a, Box& m) {
  a(m.number);
  a(m.nested_vector_array);
}

struct ExampleStruct {
  uint8_t number_u8 = 0;
  uint16_t number_u16 = 0;
  uint32_t number_u32 = 0;
  uint64_t number_u64 = 0;
  int8_t number_i8 = 0;
  int16_t number_i16 = 0;
  int32_t number_i32;
  int64_t number_i64;
  Box box;
  std::string text;
  std::vector<uint8_t> vector;
  std::vector<std::vector<uint32_t>> vector_nested;
  std::array<uint8_t, 10> array;
  std::vector<Box> vector_struct;
};

/*
Cereal signature.
*/
template <class Archive>
void serialize(Archive& a, ExampleStruct& m) {
  a(m.number_u8);
  a(m.number_u16);
  a(m.number_u32);
  a(m.number_u64);
  a(m.number_i8);
  a(m.number_i16);
  a(m.number_i32);
  a(m.number_i64);
  a(m.box);
  a(m.vector_nested);
  a(codec::array(m.vector, {codec::array_t::L16}));
  a(codec::array(m.vector_nested, {codec::array_t::L16, codec::array_t::L16}));
  a(m.array);
}
}  // namespace

TEST(codec, custom_binary) {
  ExampleStruct msg;

  msg.vector_nested.push_back({1, 2, 3});
  msg.vector_nested.push_back({4, 5, 6});

  std::stringstream ss(std::ios::binary | std::ios::out);
  codec::BinaryOutputCodec c(ss);

  c(msg);

  auto out = ss.str();

  for (int i = 0; i < out.size(); i++) {
    std::cout << std::hex << (int)out[i] << ",";
  }
  std::cout << std::endl;
}

TEST(codec, cerealArchive) {
  ExampleStruct msg;
  msg.vector_nested.push_back({1, 2, 3});
  msg.vector_nested.push_back({4, 5, 6});
  msg.vector.push_back(11);
  msg.vector.push_back(12);
  msg.vector.push_back(13);

  std::stringstream ss;

  // Scope guarantees stream flush at end
  {
    cereal::JSONOutputArchive archive(ss);
    archive(msg);
  }

  std::cout << "Cereal: " << ss.str() << std::endl;
}