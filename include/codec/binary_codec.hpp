#ifndef CODEC_BINARY_CODEC_HEADER
#define CODEC_BINARY_CODEC_HEADER

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "codec/codec.hpp"

namespace codec
{
    namespace binary
    {
        enum Prefix
        {
            L0,
            L8,
            L16,
            L32,
            L64
        };

        struct Encoder
        {
            std::vector<uint8_t> data;
        };

        //
        // All Codecs MUST define these field() overloads
        //

        template <class C, class O, class... A>
        void field(C& c, O& o, A... args)
        {
            printf("binary default\n");
            // When no overloads, default to the no-args versions.
            field(c, o);
        }

        template <class C, class O, class M1, class... M>
        void field(C& c, O& o, M1 m1, M... m)
        {
            printf("Ignoring unknown meta field\n");
            field(c, o, m...);
        }

        template <class C, class O, class... Meta>
        void field(C& c, O& o, Prefix p, Meta... m)
        {
            printf("binary with meta\n");

            field(c, o, m...);
        }

        template <class O>
        void field(Encoder& c, O& o)
        {
            printf("object\n");
            layout(c, o);
        }

        void field(Encoder& c, uint8_t& o) { printf("uint8_t\n"); }
    }
}

#endif