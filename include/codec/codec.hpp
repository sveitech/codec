#ifndef CODEC_CODEC_HEADER
#define CODEC_CODEC_HEADER

namespace codec
{
    /**
     * Because C++ does not allow partial specialization of functions, we
     * rely on functors instead.
     */
    template <class Codec, class Object>
    struct Layout
    {
        static void _(Codec& codec, Object& object) {}
    };

    template <class Codec, class Object, class... Args>
    struct Field
    {
        static void _(Codec& codec, Object& object, Args... args) {}
    };

    template <class Codec, class Object, class M>
    struct Meta
    {
        static void _(Codec& codec, Object& object, M&& meta) {}
    };

    /**
     * This specialization allows the meta (M) to be a string literal.
     * It is cast to a std::string, which is easier to use further down, in
     * the Codecs.
     */
    template <class Codec, class Object, std::size_t N>
    void register_meta(Codec& codec, Object& object, const char (&string)[N])
    {
        Meta<Codec, Object, std::string>::_(
            codec, object, std::forward<std::string>(std::string(string)));
    }

    template <class Codec, class Object, class M>
    void register_meta(Codec& codec, Object& object, M&& meta)
    {
        Meta<Codec, Object, M>::_(codec, object, std::forward<M>(meta));
    }

    template <class Codec, class Object>
    void codec(Codec& codec, Object& object)
    {
        Layout<Codec, Object>::_(codec, object);
    }

    template <class Codec, class Object>
    void field(Codec& codec, Object& object)
    {
        Field<Codec, Object>::_(codec, object);
    }

    /**
     * Fold technique only available in C++17. Abandoning, and creating a
     * C++11 compliant implementation instead.
     */
    /**
    template <class Codec, class Object, class... M>
    void field(Codec& codec, Object& object, M&&... meta)
    {
        (register_meta(codec, object, std::forward<M>(meta)), ...);
        field(codec, object);
    }
    */

    /**
     * C++11 compliant variadic argument unpacking technique, relying on
     * recursion.
     */
    template <class Codec, class Object, class M1, class... M>
    void field(Codec& codec, Object& object, M1&& meta_1, M&&... meta)
    {
        register_meta(codec, object, std::forward<M1>(meta_1));
        field(codec, object, std::forward<M>(meta)...);
    }

/**
 * HELPER MACROS
 */
#define codec_define_layout(Object, x)                                         \
    template <class Codec>                                                     \
    struct Layout<Codec, Object>                                               \
    {                                                                          \
        static void _(Codec& c, Object& o) x                                   \
    };

#define codec_define_field(Codec, Object, x)                                   \
    template <>                                                                \
    struct Field<Codec, Object>                                                \
    {                                                                          \
        static void _(Codec& c, Object& value) x                               \
    };

#define codec_define_meta(Codec, M, x)                                         \
    template <class Object>                                                    \
    struct Meta<Codec, Object, M>                                              \
    {                                                                          \
        static void _(Codec& c, Object& object, M&& meta) x                    \
    };
}

#endif