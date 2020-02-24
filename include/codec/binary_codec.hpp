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
            L32
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

    // Allow further specializations for field, in user code.
    template <class Codec, class Type>
    void field(Codec& codec, Type& type);

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

    void field(binary::Encode& codec,
               std::string& value,
               std::vector<binary::Meta> const& meta = {})
    {
        if (meta.size() > 0)
            length(codec, value.size(), meta[0]);
        else
            length(codec, value.size(), binary::L8);

        for (size_t i = 0; i < value.size(); i++)
            codec.data.push_back(value[i]);
    }

    template <class T>
    void field(binary::Encode& codec,
               std::vector<T>& value,
               std::vector<binary::Meta> const& meta = {})
    {
        if (meta.size() > 0)
            length(codec, value.size(), meta[0]);
        else
            length(codec, value.size(), binary::L8);

        for (auto& i : value)
        {
            if (meta.size() > 1)
            {
                field(codec,
                      i,
                      std::vector<binary::Meta>(meta.begin() + 1, meta.end()));
            }
            else
            {
                field(codec, i);
            }
        }
    }

    // NOTE! The position of this funtion is important! It MUST come after
    // the field specializations, otherwise it picks up the generic
    // field template.
    void length(binary::Encode& codec, size_t value, binary::Meta meta_data)
    {
        switch (meta_data)
        {
            case binary::L8:
            {
                uint8_t length = value;
                field(codec, length);
                break;
            }
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