#ifndef CODEC_BINARY_CODEC_HPP
#define CODEC_BINARY_CODEC_HPP

#include <cstdint>
#include <vector>

#include "codec.hpp"

namespace codec
{
    class Binary_Encode
    {
    public:
        std::vector<uint8_t> const& encoded();

    private:
        std::vector<uint8_t> m_encoded_data;
    };

    class Binary_Decode
    {
    public:
    private:
    };

    // Field definitions
    template <>
    void field(Binary_Encode& codec, uint8_t& value)
    {
    }
}

#endif