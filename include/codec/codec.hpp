#ifndef CODEC_CODEC_HEADER
#define CODEC_CODEC_HEADER

#include <iostream>

namespace codec
{
    /**
     * All codecs must inherit from this class. This allows ADL to find methods
     * defined in the codec namespace.
     */
    struct Codec
    {};

    /**
     * Dummy implementation. Codecs inheriting from Codec, will find this
     * method via ADL, if they do not themselves implement a register_meta()
     * function. This allows new codecs to ignore meta-fields, without further
     * code.
     */
    template <class Codec, class Object, class... T>
    void register_meta(Codec& c, Object& o, T... t)
    {}

    /**
     * Bootstrap function. Use this to encode/decode an object, using the given
     * codec object.
     */
    template <class Codec, class Object>
    void codec(Codec& c, Object& o)
    {
        std::cout << "codec::codec" << std::endl;
        layout(c, o);
    }

    /**
     * Field definition within a layout() function. The fields are defined here,
     * so that meta-data can be picked up first. Meta-data is then passed on
     * to the codecs via register_meta, and finally the type() function of the
     * codec is invoked. type() and register_meta() are found via ADL.
     */
    template <class Codec, class Object>
    void field(Codec& c, Object& o)
    {
        type(c, o);
    }

    /**
     * Because we are c++11 compliant, we need to unpack the variadic arguments
     * in a recursive way, instead of using the c++17 feature of applying
     * a function to every argument.
     *
     *      (register_meta(codec, object, meta), ...);
     */
    template <class Codec, class Object, class M1, class... M>
    void field(Codec& c, Object& o, M1&& m1, M&&... m)
    {
        register_meta(c, o, std::forward<M1>(m1));
        field(c, o, m...);
    }
}

#endif