# codec
C++ encoder/decoder. to/from structs to/from arbitrary data format (json, yaml, binary, ...)

## Used Packages / Word of Thanks

* Googletest framework. The very best in C++ unit testing and mocking.
* Nlohmann JSON library. Clean, beautiful and easy to use.

## Mission Statement

We want to take this example struct:

```cpp
struct Object
{
    struct Sub
    {
        uint16_t value_a;
        double value_b;
        float value_c;
    };

    uint8_t value_a;
    uint32_t value_b,
    char value_c;
    std::string value_d;
    std::vector<uint8_t> value_e;
    std::vector<Sub> value_f;
};
```

and encode/decode from/to arbitrary formats. As an academical exercise, this
must be done while preserving the open/closed principle. The structs cannot
be annotated with anything, either inheritance or special boxed member values.
The struct in question must not know that it is being serialized.

As far as possible, all datatypes should be supported, or at least user
definable in some way.

An example of use would be:

```cpp
int main()
{
    Json_Encoder encoder;
    Object object;

    codec::codec(object, encoder);
    printf(encoder.to_string());
};
```

This is a pseudo-code example. Any and all names and call signatures can change,
but this is the foundation for the interface we are trying to create.
An encoder/decoder object of the wanted type is created. We call a codec()
function, tailored for the Object type at hand, and in conjunction with the
encoder object, performs the encoding. The encoder object contains the encoding
after returning from the function.

## Defining Layouts

```cpp
    template <class Codec>
    struct Layout<Codec, Box>
    {
        static void _(Codec& codec, Box& object)
        {
            field(codec, object.u8);
            field(codec, object.u16);
            field(codec, object.u32);
        }
    };

    // Shorthand, using macro. The macro always defines the Codec with name 'c',
    // and the object with name 'o'.
    codec_define_layout(Box, {
        field(c, o.u8);
        field(c, o.u16);
        field(c, o.u32);
    })
```