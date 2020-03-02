#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <map>
#include <string>
#include <unordered_map>

using namespace testing;

/*
namespace
{
    template <class A, class Enable = void>
    struct Object
    {
        A function(A arg) { printf("Dummy implementation\n"); }
    };

    template <class A>
    struct Object<A, typename std::enable_if<std::is_integral<A>::value>::type>
    {
        A function(A arg) { printf("integral specialization\n"); }
    };

    // template <class A>
    // struct Object<A, int>
    // {
    //     A function(A arg) { printf("integral specialization\n"); }
    // };
}

TEST(templates, defaults)
{
    Object<int> o;
    o.function(2);

    Object<std::string> o2;
    o2.function("Hello");
}

namespace
{
    template <class Codec, class Object>
    struct Box
    {
        template <class... Args>
        static void _(Codec& codec, Object& object, Args... args)
        {
            printf("default field method\n");
        }
    };

    template <>
    struct Box<double, double>
    {
        static void _(double& codec, double& object, uint32_t& a)
        {
            printf("uint32_t field method\n");
        }
    };
}

TEST(templates, fields)
{
    Box<double, double> f;
    double a = 2.0;
    double b = 2.0;
    uint32_t c = 2;
    f._(a, b, c);
}

namespace codec
{
    // template <class C, class O>
    // void layout(C& c, O& o)
    // {
    //     printf("layout\n");
    // }
}
*/

/*
namespace codec
{
    namespace binary
    {
        enum Prefix
        {
            L0,
            L8,
            L32
        };

        class Encoder
        {};

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

        template <class C, class O>
        void field(C& c, O& o)
        {
            printf("object\n");
            layout(c, o);
        }

        template <class C>
        void field(C& c, uint32_t& o)
        {
            printf("binary u32 overload\n");
        }

        void field(Encoder& c, uint32_t& o)
        {
            printf("binary::Encoder u32 overload\n");
        }

        void field(Encoder& c, std::string& o, bool prefix = false)
        {
            printf("binary::Encoder string. Prefix: %i\n", (prefix ? 1 : 0));
        }

        template <class T>
        void field(Encoder& c, std::vector<T>& o)
        {
            printf("binary::Encoder vector\n");
            for (auto& e : o)
                field(c, e, true);
        }
    }

    template <class C, class O>
    void codec(C& c, O& o)
    {
        layout(c, o);
    }
}

namespace messages
{
    struct Thing
    {
        uint32_t a = 100;
    };

    struct Box
    {
        uint32_t a = 100;
        uint8_t b = 100;
        std::vector<uint8_t> c = {1, 2, 3, 4};
        std::string d = "Hello";
        std::vector<std::string> e = {"a", "b"};
        Thing thing;
    };

    template <class C>
    void layout(C& c, Thing& o)
    {
        field(c, o.a);
    }

    template <class C>
    void layout(C& c, Box& o)
    {
        field(c,
              o.a,
              "hello",
              codec::binary::L32,
              codec::binary::L32,
              codec::binary::L32);
        // field(c, o.b);
        // field(c, o.c);
        // field(c, o.d);
        // field(c, o.e);
        // field(c, o.thing);
    }
}

// User extends a codec
// namespace codec
// {
//     namespace binary
//     {
//         void field(Encoder& c, uint8_t& o)
//         {
//             printf("binary::Encoder u8 overload\n");
//         }
//     }
// }

TEST(templates, fields2)
{
    ::codec::binary::Encoder encoder;
    ::messages::Box box;

    ::codec::codec(encoder, box);
}
*/

namespace codec
{
    namespace base
    {
        struct Codec
        {};

        // NOTE: If this variadic function is introduced, stuff will compile
        // even when the codec does not support a specific type. This may
        // not be a good idea. Compilation errors are preferred, instead of a
        // codec silently removing a field.
        //
        // Variadic, just to make it not conflict with similar declarations
        // inside the Codecs enclosing namespace.
        // template <class... T>
        // void type(T... t)
        // {
        //     printf("codec::base::type<c, o>\n");
        // }

        // Variadic, just to make it not conflict with similar declarations
        // inside the Codecs enclosing namespace.
        // This base function picks up unsupported meta fields for a Codec.
        // This means that new codecs does not need to do anything, to disregard
        // unwanted metas.
        // NOTE: the signature register_meta(M...m) does not work. Codec and
        // Object needs to be added. Otherwise some weird stuff happens. The
        // compiler throws bad_alloc errors.
        template <class Codec, class Object, class... M>
        void register_meta(Codec& c, Object& o, M... m)
        {
            printf("register meta dummy\n");
        }
    }

    template <class Codec, class Object>
    void codec(Codec& c, Object& o)
    {
        printf("Codec\n");
        layout(c, o);
    }

    template <class Codec, class Object>
    void field(Codec& c, Object& o)
    {
        // NOTE: Rely on ADL to pick up the correct type from within the
        // namespace of the Codec.
        printf("invoking type\n");
        type(c, o);
    }

    template <class Codec, class Object, class M1, class... M>
    void field(Codec& c, Object& o, M1 m1, M... m)
    {
        printf("field with meta\n");
        register_meta(c, o, m1);
        printf("field with meta registered\n");
        field(c, o, m...);
        printf("field with meta done\n");
    }
}

namespace codec
{
    namespace binary
    {
        enum Prefix
        {
            L16,
            L32
        };

        // NOTE: By inheriting from the base::Codec, we get access to the
        // dummy version of register_meta when doing ADL. This allows us to
        // create Codecs without having to deal with throwing away unwanted
        // metas. This is done automatically.
        struct Encoder : public ::codec::base::Codec
        {
            std::vector<uint8_t> data;
            std::unordered_map<intptr_t, std::vector<Prefix>> meta;
        };

        // template <class Codec, class Object>
        // void type(Codec& c, Object& o)
        // {
        //     printf("codec::binary::type<c, o>\n");
        // }

        template <class Object>
        void register_meta(Encoder& c, Object& o, Prefix meta)
        {
            printf("register meta inside binary\n");
            c.meta[(intptr_t)&o] = {};
        }

        // NOTE: The type() functions MUST be within the same namespace as
        // the Encoder/Decoder. Otherwise ADL will not pick them up.

        // Catch-all. This allows us to pass around additional arguments
        // to local type() functions, and avoid compiler-errors, even if
        // we do not let every type() function support all the arguments.
        // Specific beats general. Variadic templates seems to always be
        // considered last, when there are non-variadic versions available.
        template <class Codec, class Object, class... T>
        void type(Codec& c, Object& o, T... t)
        {
            type(c, o);
        }

        // NOTE: Instead of implementing a type() overload for each and
        // every integral type, we utilize class templates which can be
        // partially specialized and selected using traits via enable_if.
        // This allows us to implement serialization and de-serialization
        // for ALL integral types, in just 20 lines of code.
        template <class Codec, class Object, class Enabled = void>
        struct Demultiplex
        {
            static void _(Codec& c, Object& o) { layout(c, o); }
        };

        // Partial specialization for integrals
        template <class Codec, class Object>
        struct Demultiplex<
            Codec,
            Object,
            typename std::enable_if<std::is_integral<Object>::value>::type>
        {
            static void _(Codec& c, Object& o) { __(c, o); }

            static void __(Encoder& c, Object& o)
            {
                for (size_t i = 0; i < sizeof(Object); i++)
                    c.data.push_back((o >> (i * 8)) & 0xFF);
            }
        };

        // Catcher for nested objects.
        template <class Object>
        void type(Encoder& c, Object& o)
        {
            Demultiplex<Encoder, Object>::_(c, o);
        }

        template <class T>
        void type(Encoder& c, std::vector<T>& o)
        {
            for (auto& e : o)
                type(c, e, Prefix::L16);
        }
    }
}

// TEST: Provide alternate version for uint16_t for binary encoder
// Not possible. Results in redefinition
// namespace codec
// {
//     namespace binary
//     {
//         void type(Encoder& c, uint16_t& o)
//         {
//             printf("OVERRIDE: codec::binary::type<c, uint16_t>\n");
//         }
//     }
// }

// TEST: Provide implementation for custom datatype
// Works! It is picked up correctly, even though its defined after the inclusion
// of the codec.
// It even overrides the binary::Codec, because we catch integrals via a
// template.
// namespace codec
// {
//     namespace binary
//     {
//         void type(Encoder& c, uint32_t& o)
//         {
//             printf("CUSTOM: codec::binary::type<c, uint32_t>\n");
//         }
//     }
// }

namespace
{
    struct Thing
    {
        uint8_t price = 111;
    };

    struct Box
    {
        uint8_t width = 100;
        uint8_t height = 200;
        uint16_t weight = 10;
        uint32_t value = 222;
        std::vector<uint16_t> values = {1, 2, 3, 4};
        Thing thing;
    };

    template <class Codec>
    void layout(Codec& c, Thing& o)
    {
        codec::field(c, o.price, "price", codec::binary::L16);
    }

    template <class Codec>
    void layout(Codec& c, Box& o)
    {
        printf("layout: Box\n");
        codec::field(c, o.width, codec::binary::L32, "width");
        printf("layout: Box done\n");
        codec::field(c, o.height, "height", codec::binary::L16);
        codec::field(c, o.weight);
        codec::field(c, o.value);
        codec::field(c, o.values);
        codec::field(c, o.thing, "thing");
    }
}

TEST(templates, usage)
{
    codec::binary::Encoder encoder;
    Box box;

    codec::codec(encoder, box);

    // for (auto& d : encoder.data)
    //     printf("%x, ", d);
    // printf("\n");
}