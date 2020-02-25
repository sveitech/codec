#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <type_traits>

#include "codec/binary_codec.hpp"

using namespace testing;

struct Object
{
    uint8_t value_8 = 10;
    uint16_t value_16 = 2000;
    std::string value_string;
    std::vector<std::vector<std::string>> complex;
    std::vector<uint32_t> list;
};

namespace codec
{
    template <class Codec>
    void layout(Codec& c, Object& o)
    {
        field(c, o.value_8);
        field(c, o.value_16);
        field(c, o.value_string, binary::L32);
        field(c, o.complex, binary::L32, binary::L32, binary::L32);
        field(c, o.list, binary::L32, "string_value");
    }
}

TEST(codec, decode)
{
    Object object;
    object.value_8 = 111;
    object.value_16 = 2222;
    object.value_string = "John doe";
    object.list = {1, 2, 3, 4, 5};
    object.complex = {{"a", "b", "c"}, {"a", "b", "c"}};

    codec::binary::Encode encoder;
    encoder.reset();
    codec::layout(encoder, object);

    Object new_object;
    codec::binary::Decode decoder;
    decoder.reset(encoder.data);
    codec::layout(decoder, new_object);

    ASSERT_EQ(111, new_object.value_8);
    ASSERT_EQ(2222, new_object.value_16);
    ASSERT_EQ(std::string("John doe"), new_object.value_string);
    ASSERT_EQ(5, new_object.list.size());
    ASSERT_EQ(1, new_object.list[0]);
    ASSERT_EQ(2, new_object.list[1]);
    ASSERT_EQ(3, new_object.list[2]);
    ASSERT_EQ(4, new_object.list[3]);
    ASSERT_EQ(5, new_object.list[4]);

    ASSERT_EQ(2, new_object.complex.size());
    ASSERT_EQ(3, new_object.complex[0].size());
    ASSERT_EQ(3, new_object.complex[1].size());
}