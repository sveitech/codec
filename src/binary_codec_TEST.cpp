#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "codec/binary_codec.hpp"

using namespace testing;

namespace
{
    struct Person
    {
        uint32_t age = 0;
        std::string name = "John";
    };

    struct Box
    {
        uint8_t u8 = 10;
        uint16_t u16 = 1000;
        uint32_t u32 = 1000000;
        uint64_t u64 = 0x1122334455667788;
        std::string text = "hello";
        Person person;
        std::vector<uint8_t> data = {1, 2, 3, 4};
        std::vector<std::vector<uint8_t>> nested = {{255, 255}, {255, 255}};
        std::vector<std::string> names = {"a", "b", "c"};
    };
}

namespace codec
{
    template <class Codec>
    struct Layout<Codec, Box>
    {
        static void _(Codec& codec, Box& object)
        {
            field(codec, object.u8);
            field(codec, object.u16);
            field(codec, object.u32);
            field(codec, object.u64);
            field(codec, object.text);
            field(codec, object.person);
            field(codec, object.data, binary::L32);
            field(codec, object.nested, binary::L16, binary::L16);
            field(codec, object.names, binary::L16, binary::L16);
        }
    };

    // Shorthand, using macro
    codec_define_layout(Person, {
        field(c, o.age);
        field(c, o.name);
    });
}

// Do something special when encoding a Person in binary
namespace codec
{
    codec_define_field(binary::Encoder, Person, {
        printf("Hello from specialized person serializer\n");
    });
}

TEST(binary_codec, usage)
{
    Box box;
    codec::binary::Encoder encoder;

    codec::codec(encoder, box);

    for (auto& c : encoder.data)
        printf("%x, ", (uint8_t)c);
    printf("\n");
}
