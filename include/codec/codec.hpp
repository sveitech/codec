#ifndef CODEC_CODEC_HPP
#define CODEC_CODEC_HPP

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
    void layout(Codec& codec, Object& object)
    {
    }

    template <class Codec, class Object>
    void layout_meta(Codec& codec, Object& object)
    {
    }

    // Default field function. Specialize to create fields for
    // each particular codec.
    template <class Codec, class Type>
    void field(Codec& codec, Type& type)
    {
    }

    template <class Codec, class Type>
    void meta(Codec& codec, Type& type)
    {
    }

    template <class Codec, class Object>
    Codec& codec(Object& object)
    {
        static Codec codec;
        codec.reset();
        layout_meta(codec, object);
        layout(codec, object);
        return codec;
    }
}

#endif
