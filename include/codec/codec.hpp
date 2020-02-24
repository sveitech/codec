#ifndef CODEC_CODEC_HPP
#define CODEC_CODEC_HPP

#include <cstdint>
#include <stack>
#include <unordered_map>
#include <vector>

namespace codec
{
    class Codec
    {
    public:
        virtual ~Codec() = default;
        virtual void reset() = 0;
    };

    // Default layout function. Specialize to create layout functions
    // for each custom object.
    template <class Codec, class Object>
    void layout(Codec& codec, Object& object);

    template <class Codec, class Object>
    Codec& codec(Object& object)
    {
        static Codec codec;
        codec.reset();
        layout(codec, object);
        return codec;
    }
}

#endif
