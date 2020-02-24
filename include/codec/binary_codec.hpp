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
        enum Prefix_Type
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

        class Decode : public Codec
        {
        public:
            Decode(std::vector<uint8_t> const& data) : data(data), index(0) {}
            void reset() override {}

            std::vector<uint8_t> data;
            size_t index = 0;
        };

        //
        // field() definitions
        //

        // general case. Catch custom structs
        template <class T>
        void field(Encode& c, T& value)
        {
            layout(c, value);
        }

        template <class T>
        void field(Decode& c, T& value)
        {
            layout(c, value);
        }

        //
        // uint8_t
        //
        void field(Encode& c, uint8_t& value)
        {
            c.data.push_back(value & 0xFF);
        }

        void field(Decode& c, uint8_t& value)
        {
            value = c.data.back();
            c.data.pop_back();
        }

        //
        // uint32_t
        //
        void field(Encode& c, uint32_t& value)
        {
            c.data.push_back((value >> 0) & 0xFF);
            c.data.push_back((value >> 8) & 0xFF);
            c.data.push_back((value >> 16) & 0xFF);
            c.data.push_back((value >> 24) & 0xFF);
        }

        void field(Decode& c, uint32_t& value)
        {
            value = c.data.back();
            c.data.pop_back();
            value += c.data.back() << 8;
            c.data.pop_back();
        }
    }
}

/*
    template <class T>
    void field(binary::Encode& codec, T& value);

    void length(binary::Encode& codec,
                size_t value,
                binary::Prefix_Type meta_data);

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
               std::vector<binary::Prefix_Type> const& meta = {})
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
               std::vector<binary::Prefix_Type> const& meta = {})
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
                      std::vector<binary::Prefix_Type>(meta.begin() + 1,
                                                       meta.end()));
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
    void length(binary::Encode& codec,
                size_t value,
                binary::Prefix_Type meta_data)
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

template <class T>
void codec::binary::Encode::field(T& value)
{
    printf("struct field\n");
    layout(*this, value);
}

void codec::binary::Encode::field(uint8_t& value) { printf("uint8_t field\n"); }

void codec::binary::Encode::field(uint32_t& value)
{
    printf("uint32_t field\n");
}
*/

#endif