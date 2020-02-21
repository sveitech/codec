
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "codec/binary_codec.hpp"

using namespace testing;

struct Object
{
    uint32_t a = 0;
    uint8_t b = 0;
};

// Codec specification for Object
namespace codec
{
    // general codec. Should work with any encoder/decoder.
    template <class Codec>
    void codec(Codec& codec, Object& object)
    {
        field(codec, object.a);
        field(codec, object.b);
    }

    // TODO: Possible specializations. For example for json encoding.
    template <>
    void codec(Json_Codec& codec, Object& object)
    {
        field(codec, object.a);
        field(codec, object.b);
    }
}

// class Test_Codec
// {
// };

// // Define field entries for the Test_Codec
// namespace codec
// {
//     void field(Test_Codec& codec, uint8_t& value) { printf("uint8_t
//     field\n"); }

//     void field(Test_Codec& codec, uint32_t& value)
//     {
//         printf("uint32_t field\n");
//     }
// }

// // Define a codec function for the Object struct
// namespace codec
// {
//     template <class Codec>
//     void codec(Codec& codec, Object& object)
//     {
//         printf("Using defined codec function for Object\n");

//         field(codec, object.a);
//         field(codec, object.b);
//     }
// }

TEST(playground, usage)
{
    // Object object;
    // Test_Codec codec;

    // codec::codec(codec, object);

    Object object;
    codec::Binary_Encode codec;

    codec::codec(codec, object);
}
