#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "codec/binary_codec.hpp"
#include "codec/json_codec.hpp"

using namespace testing;

namespace
{
    struct Box
    {
        std::vector<uint8_t> items = {1, 2, 3, 4};
        std::string name = "box";
    };

    struct Thing
    {
        uint8_t a = 100;
        uint16_t b = 200;
        std::vector<std::vector<Box>> boxes = {{Box(), Box()}, {Box()}};
    };

    struct Object
    {
        uint8_t value_u8 = 100;
        uint16_t value_u16 = 30000;
        uint32_t value_u32 = 0;
        uint64_t value_u64 = 0;
        std::string value_str = "hello";
        std::vector<uint8_t> value_vu8 = {1, 2, 3, 4};
        Thing thing;
        std::vector<Thing> value_vthing{Thing(), Thing(), Thing()};
    };

    template <class Codec>
    void layout(Codec& c, Box& o)
    {
        codec::field(c, o.items, "items");
        codec::field(c, o.name, "name");
    }

    template <class Codec>
    void layout(Codec& c, Thing& o)
    {
        codec::field(c, o.a, "a");
        codec::field(c, o.b, "b");
        codec::field(c, o.boxes, "boxes");
    }

    template <class Codec>
    void layout(Codec& c, Object& o)
    {
        codec::field(c, o.value_u8, "value_u8");
        codec::field(c, o.value_u16, "value_u16");
        codec::field(c, o.value_u32, "value_u32");
        codec::field(c, o.value_u64, "value_u64");
        codec::field(c, o.value_str, codec::binary::L32, "value_str");
        codec::field(c, o.value_vu8, codec::binary::L16, "value_vu8");
        codec::field(c, o.thing, "thing");
        codec::field(c, o.value_vthing, "things");
    }
}

TEST(multiple_codec, binary_serialization)
{
    codec::binary::Encoder encoder;
    Object object;

    codec::codec(encoder, object);

    for (auto& d : encoder.data)
        printf("%x, ", d);
    printf("\n");
}

TEST(multiple_codec, json_serialization)
{
    codec::json::Encoder encoder;
    Object object;

    codec::codec(encoder, object);

    printf("%s\n", encoder.to_pretty_string().c_str());
}

TEST(multiple_codec, binary_deserialization)
{
    codec::binary::Encoder encoder;
    Object object;
    object.value_u8 = 1;
    object.value_u16 = 1111;
    object.value_str = "something something dark side";
    object.value_u32 = 0x11223344;
    object.value_u64 = 0x1122334455667788;
    object.value_vu8 = {10, 11, 12, 13, 14, 15};
    object.thing.a = 99;
    object.thing.b = 999;
    object.value_vthing = {Thing(), Thing(), Thing()};

    codec::codec(encoder, object);

    codec::binary::Decoder decoder;
    Object restored;
    decoder.reset(encoder.data);
    codec::codec(decoder, restored);

    ASSERT_EQ(object.value_u8, restored.value_u8);
    ASSERT_EQ(object.value_u16, restored.value_u16);
    ASSERT_EQ(object.value_u32, restored.value_u32);
    ASSERT_EQ(object.value_u64, restored.value_u64);
    ASSERT_EQ(object.value_str, restored.value_str);
    ASSERT_EQ(object.value_vu8, restored.value_vu8);
    ASSERT_EQ(object.thing.a, restored.thing.a);
    ASSERT_EQ(object.thing.b, restored.thing.b);
    ASSERT_EQ(3, restored.value_vthing.size());
    ASSERT_EQ(object.value_vthing.size(), restored.value_vthing.size());
}

TEST(multiple_codec, json_deserialization)
{
    codec::json::Encoder encoder;
    Object object;
    object.value_u8 = 1;
    object.value_u16 = 1111;
    object.value_str = "something something dark side";
    object.value_u32 = 0x11223344;
    object.value_u64 = 0x1122334455667788;
    object.value_vu8 = {10, 11, 12, 13, 14, 15};
    object.thing.a = 99;
    object.thing.b = 999;
    object.value_vthing = {Thing(), Thing(), Thing()};

    codec::codec(encoder, object);

    codec::json::Decoder decoder;
    Object restored;
    decoder.reset(encoder.to_string());

    codec::codec(decoder, restored);

    ASSERT_EQ(object.value_u8, restored.value_u8);
}