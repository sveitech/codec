#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

using namespace testing;

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

namespace codec
{
    namespace binary
    {
        class Encoder
        {};

        template <class C, class O, class... A>
        void field(C& c, O& o, A... args)
        {
            printf("binary default\n");
            // When no overloads, default to the no-args versions.
            field(c, o);
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
        field(c, o.a);
        field(c, o.b);
        field(c, o.c);
        field(c, o.d);
        field(c, o.e);
        field(c, o.thing);
    }
}

// User extends a codec
namespace codec
{
    namespace binary
    {
        void field(Encoder& c, uint8_t& o)
        {
            printf("binary::Encoder u8 overload\n");
        }
    }
}

TEST(templates, fields2)
{
    ::codec::binary::Encoder encoder;
    ::messages::Box box;

    ::codec::codec(encoder, box);
}