# codec
C++ encoder/decoder. to/from structs to/from arbitrary data format (json, yaml, binary, ...)

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