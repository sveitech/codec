#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "codec/json_codec.hpp"

using namespace testing;

namespace
{
    struct Object
    {
        uint8_t value_u8 = 22;
        uint32_t value_u32 = 0x11223344;
        std::string value_string = "Hello";
    };
}

namespace codec
{
    codec_define_layout(Object, {
        field(c, o.value_u8, std::string("small_number"));
        field(c, o.value_u32, std::string("number"));
        field(c, o.value_string, "phrase");
    });
}

TEST(json_codec, usage)
{
    Object object;
    codec::json::Encoder encoder;

    codec::codec(encoder, object);
    printf("%s\n", encoder.to_string().c_str());
}