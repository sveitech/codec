#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <type_traits>

#include "codec/binary_codec.hpp"

using namespace testing;

struct Object
{
    uint8_t value_8 = 10;
    uint16_t value_16 = 2000;
    std::string value_string;
    std::vector<std::vector<std::string>> complex;
    std::vector<uint32_t> list;
};

namespace codec
{
    template <class Codec>
    void layout(Codec& c, Object& o)
    {
        field(c, o.value_8);
        field(c, o.value_16);
        field(c, o.value_string, binary::L32);
        // field(c, o.complex, binary::L32, binary::L32, binary::L32);
        // field(c, o.list, binary::L32, "string_value");
    }
}

TEST(codec, usage)
{
    Object object;
    object.value_8 = 111;
    object.value_16 = 2222;
    object.value_string = "John doe";
    object.list = {1, 2, 3, 4, 5};
    object.complex = {{"a", "b", "c"}, {"a", "b", "c"}};

    codec::Binary_Encode encoder;
    encoder.reset();
    codec::layout(encoder, object);

    // Object new_object;
    // codec::binary::Decode decoder;
    // decoder.reset(encoder.data);
    // codec::layout(decoder, new_object);

    // ASSERT_EQ(111, new_object.value_8);
    // ASSERT_EQ(2222, new_object.value_16);
    // ASSERT_EQ(std::string("John doe"), new_object.value_string);
    // ASSERT_EQ(5, new_object.list.size());
    // ASSERT_EQ(1, new_object.list[0]);
    // ASSERT_EQ(2, new_object.list[1]);
    // ASSERT_EQ(3, new_object.list[2]);
    // ASSERT_EQ(4, new_object.list[3]);
    // ASSERT_EQ(5, new_object.list[4]);

    // ASSERT_EQ(2, new_object.complex.size());
    // ASSERT_EQ(3, new_object.complex[0].size());
    // ASSERT_EQ(3, new_object.complex[1].size());
}

struct Foo
{};

struct Box
{
    uint16_t width = 0;
    uint16_t height = 0;
};

struct Every_Type
{
    uint8_t value_u8 = 0;
    uint16_t value_u16 = 0;
    uint32_t value_u32 = 0;
    uint64_t value_u64 = 0;

    int8_t value_i8 = 0;
    int16_t value_i16 = 0;
    int32_t value_i32 = 0;
    int64_t value_i64 = 0;

    Box box;
    Foo foo;
};

template <class Codec>
void codec::layout(Codec& c, Foo& o)
{
    printf("Foo layout\n");
}

template <class Codec>
void codec::layout(Codec& c, Box& o)
{
    field(c, o.width);
    field(c, o.height);
}

template <class Codec>
void codec::layout(Codec& c, Every_Type& o)
{
    // field(c, o.value_u8);
    // field(c, o.value_i8);
    // field(c, o.box);
    field(c, o.foo);
}

TEST(codec, every_type)
{
    codec::Binary_Encode encoder;
    Every_Type object;
    object.value_u8 = 10;
    object.value_i8 = -10;

    object.box.width = 100;
    object.box.height = 200;

    codec::layout(encoder, object);

    // codec::Binary_Decode decoder(encoder.data);
    // Every_Type restored_object;

    // codec::layout(decoder, restored_object);

    // ASSERT_EQ(10, restored_object.value_u8);
    // ASSERT_EQ(100, restored_object.box.width);
    // ASSERT_EQ(200, restored_object.box.height);
}

// namespace ns
// {
//     template <class T, class O>
//     void tf(T& value, O& o)
//     {
//         printf("dummy tf\n");
//     }
// }

// template <class T, class O>
// void foo(T& value, O& o)
// {
//     ns::tf(value, o);
// }

// template <class O>
// void ns::tf<uint32_t, O>(uint32_t& value, O& o)
// {
//     printf("uint32_t specialization\n");
// }

// /**
//  * Functions cannot be partially specialized. Instead, they
//  * behave like overloads. This is why this function picks up
//  * the dummy tf first, because tf is not a dependent name.
//  */
// TEST(codec, adl_no_work)
// {
//     uint32_t a = 100;
//     std::string b = "hej";
//     foo(a, b);
// }

// namespace ns_2
// {
//     template <class T>
//     void tf(T& value)
//     {
//         printf("dummy tf\n");
//     }
// }

// namespace ns_2
// {
//     namespace other
//     {
//         template <class T>
//         void foo(T& value)
//         {
//             ns::tf(value);
//         }
//     }
// }

// template <class T>
// void ns_2::tf(uint32_t& value)
// {
//     printf("uint32_t specialization\n");
// }

// TEST(codec, adl_work)
// {
//     uint32_t a = 100;
//     ns_2::other::foo(a);
// }

struct Functor
{
    Functor() {}

    void operator()() { printf("Operator ()\n"); }
};

namespace codec
{
    template <class Codec, class Object>
    struct Layout
    {
        void operator()(Codec& codec, Object& object)
        {
            printf("base Layout operator\n");
        }
    };
}

struct Some_Codec
{

};

struct Person
{};

namespace codec
{
    template <class C, class O>
    void codec(C& c, O& o)
    {
        codec::Layout<C, O> l;
        l(c, o);
    }
}

namespace codec
{
    template <class Codec>
    struct Layout<Codec, Person>
    {
        void operator()(Codec& codec, Person& object)
        {
            printf("Person Layout operator\n");
        }
    };
}

TEST(codec, functors)
{
    // Functor f;
    // f();

    uint32_t T;
    // std::string S;
    // codec::Layout<uint32_t, std::string> l;
    // l(T, S);

    // codec::codec(T, S);

    Person p;
    codec::codec(T, p);
}