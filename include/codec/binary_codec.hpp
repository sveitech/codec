#ifndef CODEC_BINARY_CODEC_HPP
#define CODEC_BINARY_CODEC_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "codec.hpp"

namespace codec
{
    namespace binary
    {
        enum Meta
        {
            L0,
            L8,
            L16,
            L24,
            L32,
            DEFAULT
        };

        class Encode : public Codec
        {
        public:
            void reset() override { data.clear(); }
            std::vector<uint8_t> data;

            Meta_Base<Meta> meta;
        };

        class Decode
        {
        public:
        private:
        };
    }

    template <>
    binary::Meta default_meta()
    {
        return binary::Meta::L0;
    }

    void length(binary::Encode& codec, size_t value, binary::Meta meta_data);

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

    template <>
    void field(binary::Encode& codec, std::string& value)
    {
        codec.meta.push_member(value);
        auto meta = codec.meta.pop();
        length(codec, value.size(), meta);
        codec.meta.rollback();
        codec.meta.pop_member(value);

        for (size_t i = 0; i < value.size(); i++)
            codec.data.push_back(value[i]);
    }

    // By default, use 1 byte to encode length field
    template <class T>
    void field(binary::Encode& codec, std::vector<T>& value)
    {
        codec.meta.push_member(value);
        auto meta = codec.meta.pop();
        length(codec, value.size(), meta);

        for (auto& i : value)
            field(codec, i);

        codec.meta.rollback();
        codec.meta.pop_member(value);
    }

    // Meta declarations
    template <class T>
    void meta(binary::Encode& codec, T const& value, binary::Meta meta_data)
    {
        codec.meta.set(value, meta_data);
    }

    template <class T>
    void meta(binary::Encode& codec,
              T const& value,
              std::vector<binary::Meta> const& meta_data)
    {
        codec.meta.set(value, meta_data);
    }

    // NOTE! The position of this funtion is important! It MUST come after
    // the field specializations, otherwise it picks up the generic
    // field template.
    void length(binary::Encode& codec, size_t value, binary::Meta meta_data)
    {
        switch (meta_data)
        {
            case binary::L16:
            {
                uint16_t length = value;
                field(codec, length);
                break;
            }
            case binary::L32:
            {
                uint32_t length = value;
                field(codec, length);
                break;
            }
        }
    }
}

#endif