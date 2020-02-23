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

            std::unordered_map<intptr_t, std::vector<Meta>> meta;
            intptr_t meta_field = 0;
        };

        class Decode
        {
        public:
        private:
        };
    }

    template <class T>
    void length(binary::Encode& codec, T& value, bool pop = false);

    binary::Meta get_meta(binary::Encode& codec,
                          intptr_t value,
                          bool pop = false)
    {
        // Find meta info
        auto it = codec.meta.find(value);

        if (it != codec.meta.end())
        {
            if (it->second.size() > 0)
            {
                binary::Meta m = it->second[0];
                if (pop)
                    it->second.erase(it->second.begin());
                return m;
            }
        }

        return binary::Meta::DEFAULT;
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

    template <>
    void field(binary::Encode& codec, std::string& value)
    {
        printf("==== Encoding string. Metafield: %ul\n", codec.meta_field);
        // Length field is determined by meta-data
        length(codec, value, false);

        for (size_t i = 0; i < value.size(); i++)
            codec.data.push_back(value[i]);
    }

    // By default, use 1 byte to encode length field
    template <class T>
    void field(binary::Encode& codec, std::vector<T>& value)
    {
        printf("== Encoding vector: %ul\n", (intptr_t)&value);
        // Length field is determined by meta-data
        length(codec, value, true);
        codec.meta_field = (intptr_t)&value;

        for (auto& i : value)
            field(codec, i);

        codec.meta_field = 0;
    }

    // Meta declarations

    template <class T>
    void meta(binary::Encode& codec,
              T const& value,
              std::vector<binary::Meta> const& meta_data)
    {
        codec.meta[(intptr_t)&value] = meta_data;
    }

    // NOTE! The position of this funtion is important! It MUST come after
    // the field specializations, otherwise it picks up the generic
    // field template.
    template <class T>
    void length(binary::Encode& codec, T& value, bool pop)
    {
        intptr_t meta_field = (intptr_t)&value;

        if (codec.meta_field != 0)
            meta_field = codec.meta_field;

        switch (get_meta(codec, meta_field, pop))
        {
            case binary::L16:
            {
                printf("Found L16\n");
                uint16_t length = value.size();
                field(codec, length);
                break;
            }
            case binary::L32:
            {
                printf("Found L32\n");
                uint32_t length = value.size();
                field(codec, length);
                break;
            }
            default:
            {
                printf("None found\n");
                break;
            }
        }
    }
}

#endif