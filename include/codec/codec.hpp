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

    template <class T>
    struct Box
    {
        Box(T& r) : ref(r) {}
        T& ref;
    };

    // Default layout function. Specialize to create layout functions
    // for each custom object.
    template <class Codec, class Object>
    void layout(Codec& codec, Object& object)
    {
    }

    // Default field function. Specialize to create fields for
    // each particular codec.
    template <class Codec, class Type>
    void field(Codec& codec, Type& type)
    {
    }

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
