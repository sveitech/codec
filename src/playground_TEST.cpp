
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <type_traits>

#include "codec/binary_codec.hpp"
#include "codec/json_codec.hpp"

using namespace testing;

struct Box
{
    uint16_t width = 0;
    uint16_t height = 0;
};

struct Bar
{
    uint32_t a = 100;
};

struct Object
{
    uint32_t a = 0xaabbccdd;
    uint8_t b = 0xff;
    std::string c{"Hello"};
    std::string d{"Hello"};
    std::vector<std::string> e = {"a", "b", "c"};
    Box box;
    Bar bar;
};

// layout specification for Object
namespace codec
{
    template <class Codec>
    void layout(Codec& codec, ::Bar& bar)
    {
        field(codec, bar.a);
    }

    template <class Codec>
    void layout(Codec& codec, ::Box& box)
    {
        field(codec, box.width);
        field(codec, box.height);
    }

    template <
        class Codec,
        typename std::enable_if<std::is_same<Codec, binary::Encode>::value,
                                int>::type = 0>
    void layout(Codec& codec, Object& object)
    {
        using namespace codec::binary;

        field(codec, object.a);
        field(codec, object.b);
        field(codec, object.c, {binary::L16});
        field(codec, object.d);
        field(codec, object.e, {binary::L16, binary::L32});
        field(codec, object.box);
        field(codec, object.bar);
    }

    // Create specialization for the Bar type
    template <>
    void field(binary::Encode& codec, Bar& value)
    {
        printf("Encoding BAR\n");
    }

    template <class Codec,
              typename std::enable_if<std::is_same<Codec, json::Encode>::value,
                                      int>::type = 0>
    void layout(Codec& codec, Object& object)
    {
        field(codec, object.a, "a");
        field(codec, object.b, "b");
    }
}

TEST(playground, encode_binary)
{
    Object object;
    auto& c = codec::codec<codec::binary::Encode>(object);

    printf("Size of encoded: %d\n", c.data.size());

    for (auto& e : c.data)
        printf("%x, ", (uint8_t)e);
    printf("\n");
}

TEST(playground, encode_json)
{
    Object object;
    auto& c = codec::codec<codec::json::Encode>(object);
}