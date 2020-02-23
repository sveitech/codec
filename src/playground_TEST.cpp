
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
    uint32_t a = 0;
    uint8_t b = 0;
    std::string c{"Hello"};
    std::vector<uint16_t> d = {1, 2, 3, 4};
    Box box;
    std::vector<uint8_t> e = {1, 1, 1, 1, 1};
    std::vector<std::string> f = {"a", "b"};

    // double boxing. Inner vector should also be 16 bit prefixed,
    // as well as the strings
    // std::vector<std::vector<std::string>> f = {{"a", "b", "c"},
    //                                            {"d", "e", "f"}};
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
        field(codec, object.box);
        field(codec, L_16<std::vector<uint8_t>>(object.e));
        field(codec, L_16<L_16<std::vector<std::string>>>(object.f));
        // field(codec, object.f, L_16<L_16<L_16<std::string>>>());
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
