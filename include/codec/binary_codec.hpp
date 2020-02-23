#ifndef CODEC_BINARY_CODEC_HPP
#define CODEC_BINARY_CODEC_HPP

#include <cstdint>
#include <vector>

#include "codec.hpp"

namespace codec
{
    namespace binary
    {
        // Field uses a 16 bit length prefix. Use this with
        // std::vector and std::string
        template <class T>
        struct L_16 : codec::Box<T>
        {
            L_16(T& r) : codec::Box<T>(r) {}
        };

        // Field uses a 0-bit length field (None). Use this with
        // std::vector and std::string.
        struct L_0
        {
        };

        class Encode : public Codec
        {
        public:
            void reset() override { data.clear(); }
            std::vector<uint8_t> data;
        };

        class Decode
        {
        public:
        private:
        };
    }

    // Field definitions
    // Unknown structs. Invoke their layout functions.
    // This function MUST be defined before any other specializations,
    // otherwise it will shadow any previous specializations.
    template <class T>
    void field(binary::Encode& codec, T& value)
    {
        layout(codec, value);
    }

    template <>
    void field(binary::Encode& codec, uint8_t& value)
    {
        codec.data.push_back(value);
    }

    template <>
    void field(binary::Encode& codec, uint16_t& value)
    {
        codec.data.push_back(value & 0xFF);
        codec.data.push_back((value >> 8) & 0xFF);
    }

    template <>
    void field(binary::Encode& codec, uint32_t& value)
    {
        codec.data.push_back(value & 0xFF);
        codec.data.push_back((value >> 8) & 0xFF);
        codec.data.push_back((value >> 16) & 0xFF);
        codec.data.push_back((value >> 24) & 0xFF);
    }

    // By default, use 1 byte to encode length field
    template <>
    void field(binary::Encode& codec, std::string& value)
    {
        uint8_t size = value.size();
        field(codec, size);

        for (size_t i = 0; i < value.size(); i++)
            codec.data.push_back(value[i]);
    }

    // By default, use 1 byte to encode length field
    template <class T>
    void field(binary::Encode& codec, std::vector<T>& value)
    {
        uint8_t size = value.size();
        field(codec, size);

        for (auto& i : value)
            field(codec, i);
    }

    // template <class T, class B>
    // void field(binary::Encode& codec, T& value, binary::L_16<B>&& boxed)
    // {
    //     uint16_t size = value.size();
    //     field(codec, size);

    //     for (auto& field : value)
    //         field(codec, field, B());
    // }

    // Custom length encoders
    template <class T>
    void field(binary::Encode& codec, binary::L_16<T>&& boxed)
    {
        uint16_t size = boxed.ref.size();
        field(codec, size);

        for (auto& i : boxed.ref)
            field(codec, i);
    }
}

#endif