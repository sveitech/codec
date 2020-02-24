#ifndef CODEC_JSON_CODEC_HPP
#define CODEC_JSON_CODEC_HPP

#include "codec.hpp"

namespace codec
{
    namespace json
    {
        class Encode : public Codec
        {
        public:
            void reset() override {}
        };

        class Decode
        {
        };
    }

    // Allow further specializations for field, in user code.
    template <class Codec, class Type>
    void field(Codec& codec, Type& type, const char* name);

    template <>
    void field(json::Encode& codec, uint8_t& value, const char* name)
    {
        printf("Encoding json uint8_t\n");
    }

    template <>
    void field(json::Encode& codec, uint32_t& value, const char* name)
    {
        printf("Encoding json uint32_t\n");
    }
}

#endif