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

        class Encode
        {
        public:
            void reset() { data.clear(); }
            std::vector<uint8_t> data;
        };

        class Decode
        {
        public:
            void reset() {}

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

#endif