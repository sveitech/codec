
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "codec/binary_codec.hpp"

using namespace testing;

struct Box
{
    uint16_t width = 0;
    uint16_t height = 0;
};

struct Object
{
    uint32_t a = 0xaabbccdd;
    uint8_t b = 0xff;
    std::string c{"Hello"};
    std::string d{"Hello"};
    std::vector<std::string> e = {"a", "b", "c"};
};

// layout specification for Object
namespace codec
{
    template <class Codec>
    void layout(Codec& codec, ::Box& box)
    {
        field(codec, box.width);
        field(codec, box.height);
    }

    // general layout. Should work with any encoder/decoder.
    template <class Codec>
    void layout(Codec& codec, Object& object)
    {
        using namespace codec::binary;

        field(codec, object.a);
        field(codec, object.b);
        field(codec, object.c);
        field(codec, object.d);
        field(codec, object.e);
        // field(codec, object.d);
        // field(codec, object.box);
        // field(codec, object.e);
        // field(codec, object.f);
    }

    template <class Codec>
    void layout_meta(Codec& codec, Object& object)
    {
        meta(codec, object.c, {binary::L16});
        meta(codec, object.d, {binary::L32});

        // Apply 16 bit length to both the outer vector and the inner strings
        meta(codec, object.e, {binary::L16, binary::L32});
    }
}

TEST(playground, usage)
{
    Object object;
    auto& c = codec::codec<codec::binary::Encode>(object);

    printf("Size of encoded: %d\n", c.data.size());

    for (auto& e : c.data)
        printf("%x, ", (uint8_t)e);
    printf("\n");
}
