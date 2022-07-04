
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/string.hpp>
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
  std::string long_text;
  std::vector<uint8_t> vector;
  std::vector<std::vector<uint8_t>> vector_nested;
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
  a(m.text);
  a(codec::string(m.long_text, {codec::prefix_t::L16}));
  a(m.vector_nested);
  a(codec::array(m.vector, {codec::prefix_t::L16}));
  a(codec::array(m.vector_nested,
                 {codec::prefix_t::L16, codec::prefix_t::L16}));
  a(m.array);
  a(m.vector_struct);
}
}  // namespace

TEST(codec, custom_binary) {
  ExampleStruct msg;
  msg.number_u8 = 1;
  msg.number_u16 = 2;
  msg.number_u32 = 3;
  msg.number_u64 = 4;
  msg.number_i8 = 5;
  msg.number_i16 = 6;
  msg.number_i32 = 7;
  msg.number_i64 = 8;
  msg.box.number = 9;
  msg.box.nested_vector_array.push_back({1, 1, 1, 1, 1, 1, 1, 1});
  msg.text = "Hello there";
  msg.long_text = "Hi there some more";

  msg.vector.push_back(1);
  msg.vector.push_back(2);
  msg.vector.push_back(3);

  msg.vector_nested.push_back({8, 8, 8, 8});

  for (int i = 0; i < msg.array.size(); i++) {
    msg.array[i] = 20;
  }
  // msg.vector_nested.push_back({5, 5, 5, 5});

  std::stringstream ss(std::ios::binary | std::ios::out);
  codec::BinaryOutputCodec c(ss);

  c(msg);

  auto out = ss.str();

  for (int i = 0; i < out.size(); i++) {
    std::cout << std::hex << (int)out[i] << ",";
  }
  std::cout << std::endl;
}

// TEST(codec, cerealArchive) {
//   ExampleStruct msg;
//   msg.vector_nested.push_back({1, 2, 3});
//   msg.vector_nested.push_back({4, 5, 6});
//   msg.vector.push_back(11);
//   msg.vector.push_back(12);
//   msg.vector.push_back(13);
//   msg.text = "Some text";

//   std::stringstream ss;

//   // Scope guarantees stream flush at end
//   {
//     cereal::JSONOutputArchive archive(ss);
//     archive(msg);
//   }

//   std::cout << "Cereal: " << ss.str() << std::endl;
// }