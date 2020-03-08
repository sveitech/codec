#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "codec/binary_codec.hpp"

using namespace testing;

namespace
{
    struct Message
    {};
}

TEST(binary_codec, example) {}

namespace
{
    struct Custom_Sizes
    {
        int value = 0;
    };

    template <class Codec>
    void layout(Codec& c, Custom_Sizes& o)
    {
        codec::field(c, o.value, codec::binary::meta::L16);
    }
}

TEST(binary_codec, custom_sizes)
{
    Custom_Sizes cs;
    codec::binary::Encoder encoder;

    codec::codec(encoder, cs);

    ASSERT_EQ(encoder.data.size(), 2);
}