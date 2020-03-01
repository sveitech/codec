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