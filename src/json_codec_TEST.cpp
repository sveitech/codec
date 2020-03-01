#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "codec/json_codec.hpp"

using namespace testing;

namespace
{
    struct Thing
    {
        uint8_t id = 123;
    };

    struct Box
    {
        uint8_t width = 100;
        uint8_t height = 200;
        Thing thing;
        std::vector<std::vector<Thing>> things = {{Thing(), Thing()},
                                                  {Thing(), Thing()}};
    };

    struct Object
    {
        uint8_t value_u8 = 22;
        uint32_t value_u32 = 0x11223344;
        std::string value_string = "Hello";
        std::vector<uint8_t> value_v_u8 = {1, 2, 3, 4};
        Box box;
        Box box2;
        std::vector<uint8_t> value_u8v = {1, 2, 3, 4};
        std::vector<Box> value_boxes = {Box(), Box()};
        std::vector<std::string> value_strings = {"a", "b", "c"};
    };
}

namespace codec
{
    codec_define_layout(Object, {
        field(c, o.value_u8, std::string("value_u8"));
        field(c, o.value_u32, std::string("value_u32"));
        field(c, o.value_string, "value_string");
        field(c, o.value_v_u8, "value_v_u8");
        field(c, o.box, "box");
        field(c, o.box2, "box2");
        field(c, o.value_u8v, "value_u8v");
        field(c, o.value_boxes, "value_boxes");
        field(c, o.value_strings, "value_strings");
    });

    codec_define_layout(Box, {
        field(c, o.width, "width");
        field(c, o.height, "height");
        field(c, o.thing, "thing");
        field(c, o.things, "things");
    });

    codec_define_layout(Thing, { field(c, o.id, "id"); });
}

TEST(json_codec, usage)
{
    Object object;
    codec::json::Encoder encoder;

    codec::codec(encoder, object);
    printf("%s\n", encoder.to_string().c_str());
}

namespace
{
    struct All_Types
    {
        uint8_t value_u8 = 0;
        int8_t value_i8 = 0;
        std::string value_str;
        std::vector<uint32_t> value_v_u32;
    };
}

namespace codec
{
    codec_define_layout(All_Types, {
        field(c, o.value_u8, "value_u8");
        field(c, o.value_i8, "value_i8");
        field(c, o.value_str, "value_str");
    });
}

TEST(json_codec, decode)
{
    All_Types object;
    object.value_u8 = 10;
    object.value_i8 = -10;
    object.value_str = "Hello there";
    object.value_v_u32 = {1, 2, 3, 4, 5};

    codec::json::Encoder encoder;
    codec::codec(encoder, object);

    All_Types restored;

    codec::json::Decoder decoder;
    decoder.reset(encoder.to_string());
    codec::codec(decoder, restored);

    ASSERT_EQ(object.value_u8, restored.value_u8);
    ASSERT_EQ(object.value_i8, restored.value_i8);
    ASSERT_EQ(object.value_str, restored.value_str);
}
