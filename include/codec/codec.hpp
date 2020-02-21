#ifndef CODEC_CODEC_HPP
#define CODEC_CODEC_HPP

namespace codec
{
    // Default codec function. Specialize to create codec functions
    // for each custom object.
    template <class Codec, class Object>
    void codec(Codec& codec, Object& object)
    {
    }

    // Default field function. Specialize to create fields for
    // each particular codec.
    template <class Codec, class Type>
    void field(Codec& codec, Type& type)
    {
    }
}

#endif
