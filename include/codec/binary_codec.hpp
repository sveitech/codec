#ifndef CODEC_BINARY_CODEC_HEADER
#define CODEC_BINARY_CODEC_HEADER

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "codec/codec.hpp"

namespace codec
{
    namespace binary
    {
        enum Prefix_Type
        {
            L0,
            L8,
            L16,
            L32,
            L64
        };

        struct Codec
        {
            std::vector<uint8_t> data;
            std::unordered_map<intptr_t, std::vector<Prefix_Type>> meta;

            template <class Codec, class Object>
            static size_t length(Codec& codec,
                                 Object& object,
                                 std::vector<Prefix_Type> const& meta)
            {
                printf("length. MEta size: %i\n", meta.size());
                if (meta.size() > 0)
                {
                    switch (meta[0])
                    {
                        case Prefix_Type::L8:
                        {
                            uint8_t length = object.size();
                            ::codec::field(codec, length);
                            return length;
                        }
                        case Prefix_Type::L32:
                        {
                            uint32_t length = object.size();
                            ::codec::field(codec, length);
                            return length;
                        }
                    }
                }
                else
                {
                    return object.size();
                }
            }

            template <class Object>
            std::vector<Prefix_Type> get_meta(
                Object& object,
                std::vector<Prefix_Type> default_meta)
            {
                printf("get meta for %u\n", (intptr_t)&object);
                auto it = meta.find((intptr_t)&object);

                if (it != meta.end())
                    return it->second;
                else
                {
                    printf("not found.\n");
                    return default_meta;
                }
            }
        };

        struct Encoder : public Codec
        {
            void reset() { data.clear(); }
        };

        struct Decoder : public Codec
        {
            void reset(std::vector<uint8_t> const& data) { this->data = data; }
        };
    }

    codec_define_meta(binary::Encoder, binary::Prefix_Type, {
        printf("Registering meta for: %u\n", (intptr_t)&object);
        c.meta[(intptr_t)&object].push_back(meta);
    });

    // FIELDS

    // Partial specialization for catching structs
    template <class Object>
    struct Field<binary::Encoder, Object>
    {
        static void _(binary::Encoder& c, Object& value)
        {
            ::codec::codec(c, value);
        }
    };

    codec_define_field(binary::Encoder, uint8_t, {
        c.data.push_back(value & 0xFF);
    });

    codec_define_field(binary::Encoder, uint16_t, {
        c.data.push_back((value >> 0) & 0xFF);
        c.data.push_back((value >> 8) & 0xFF);
    });

    codec_define_field(binary::Encoder, uint32_t, {
        c.data.push_back((value >> 0) & 0xFF);
        c.data.push_back((value >> 8) & 0xFF);
        c.data.push_back((value >> 16) & 0xFF);
        c.data.push_back((value >> 24) & 0xFF);
    });

    codec_define_field(binary::Encoder, uint64_t, {
        c.data.push_back((value >> 0) & 0xFF);
        c.data.push_back((value >> 8) & 0xFF);
        c.data.push_back((value >> 16) & 0xFF);
        c.data.push_back((value >> 24) & 0xFF);
        c.data.push_back((value >> 32) & 0xFF);
        c.data.push_back((value >> 40) & 0xFF);
        c.data.push_back((value >> 48) & 0xFF);
        c.data.push_back((value >> 56) & 0xFF);
    });

    // STRING
    template <>
    struct Field<binary::Encoder, std::string>
    {
        static void _(binary::Encoder& c, std::string& value)
        {
            auto meta = c.get_meta(value, {binary::Prefix_Type::L8});
            binary::Codec::length(c, value, meta);
            c.data.insert(c.data.end(), value.begin(), value.end());
        }
    };

    // VECTOR
    template <class T>
    struct Field<binary::Encoder, std::vector<T>>
    {
        static void _(binary::Encoder& c,
                      std::vector<T>& value,
                      std::vector<binary::Prefix_Type> meta = {
                          binary::Prefix_Type::L8})
        {
            meta = c.get_meta(value, meta);
            binary::Codec::length(c, value, meta);

            for (auto& element : value)
            {
                if (meta.size() > 1)
                {
                    printf("===\n");
                    
                }
                else
                {
                    ::codec::field(c, element);
                }
            }
        }
    };
}

#endif