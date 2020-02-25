#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <type_traits>

#include "codec/binary_codec.hpp"

using namespace testing;

struct Object
{
    uint8_t value_8 = 0;
    uint16_t value_16 = 0;
};

namespace codec
{
    // Testing meta objects
    struct String_Meta : public Meta_Object
    {
        String_Meta(const char* value) {}
    };

    template <
        class Codec,
        typename std::enable_if<std::is_same<Codec, binary::Encode>::value ||
                                    std::is_same<Codec, binary::Decode>::value,
                                int>::type = 0>
    void layout(Codec& c, Object& o)
    {
        field(c, o.value_8);
        field(c, o.value_16, Meta({String_Meta("hello")}));
    }
}

TEST(codec, encode) { Object object; }