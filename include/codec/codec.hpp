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
        static void _(Codec& codec, Object& object)
        {
            printf("Default Layout implementation\n");
        }
    };

    template <class Codec, class Object, class... Args>
    struct Field
    {
        static void _(Codec& codec, Object& object)
        {
            printf("Default Field implementation\n");
        }
    };

    template <class Codec, class Object, class M>
    struct Meta
    {
        static void _(Codec& codec, Object& object, M&& meta)
        {
            printf("Default meta implementation\n");
        }
    };

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

    template <class Codec, class Object, class... M>
    void field(Codec& codec, Object& object, M&&... meta)
    {
        printf("field with meta\n");
        (register_meta(codec, object, std::forward<M>(meta)), ...);
        field(codec, object);
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