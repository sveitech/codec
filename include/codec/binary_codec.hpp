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
                        case Prefix_Type::L16:
                        {
                            uint16_t length = object.size();
                            ::codec::field(codec, length);
                            return length;
                        }
                        case Prefix_Type::L32:
                        {
                            uint32_t length = object.size();
                            ::codec::field(codec, length);
                            return length;
                        }
                        case Prefix_Type::L64:
                        {
                            uint64_t length = object.size();
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
                auto it = meta.find((intptr_t)&object);

                if (it != meta.end())
                    return it->second;
                else
                {
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
            void reset(std::vector<uint8_t> const& data)
            {
                this->data = data;
                index = 0;
            }

            size_t index = 0;
        };
    } // namespace binary

    /**
     * META Catchers.
     *
     * The binary codec collects any meta arguments in the field() method,
     * which are of the type binary::Prefix_Type. All others are ignored.
     */
    codec_define_meta(binary::Encoder, binary::Prefix_Type, {
        c.meta[(intptr_t)&object].push_back(meta);
    });

    codec_define_meta(binary::Decoder, binary::Prefix_Type, {
        c.meta[(intptr_t)&object].push_back(meta);
    });

    /**
     * FIELD SPECIALIZATIONS
     */

    // Partial specialization for catching user structs/classes, which have
    // their own layout.
    template <class Object>
    struct Field<binary::Encoder, Object>
    {
        static void _(binary::Encoder& c, Object& value)
        {
            ::codec::codec(c, value);
        }
    };

    template <class Object>
    struct Field<binary::Decoder, Object>
    {
        static void _(binary::Decoder& c, Object& value)
        {
            ::codec::codec(c, value);
        }
    };

    /**
     * PRIMITIVE FIELDS
     */
    codec_define_field(binary::Encoder, uint8_t, {
        c.data.push_back(value & 0xFF);
    });

    codec_define_field(binary::Decoder, uint8_t, {
        value = c.data[c.index++] & 0xFF;
    });

    codec_define_field(binary::Encoder, int8_t, {
        c.data.push_back(value & 0xFF);
    });

    codec_define_field(binary::Decoder, int8_t, {
        value = c.data[c.index++] & 0xFF;
    });

    codec_define_field(binary::Encoder, uint16_t, {
        c.data.push_back((value >> 0) & 0xFF);
        c.data.push_back((value >> 8) & 0xFF);
    });

    codec_define_field(binary::Decoder, uint16_t, {
        value = c.data[c.index++];
        value += (c.data[c.index++] << 8);
    });

    codec_define_field(binary::Encoder, int16_t, {
        c.data.push_back((value >> 0) & 0xFF);
        c.data.push_back((value >> 8) & 0xFF);
    });

    codec_define_field(binary::Decoder, int16_t, {
        value = c.data[c.index++];
        value += (c.data[c.index++] << 8);
    });

    codec_define_field(binary::Encoder, uint32_t, {
        c.data.push_back((value >> 0) & 0xFF);
        c.data.push_back((value >> 8) & 0xFF);
        c.data.push_back((value >> 16) & 0xFF);
        c.data.push_back((value >> 24) & 0xFF);
    });

    codec_define_field(binary::Decoder, uint32_t, {
        value = c.data[c.index++];
        value += (c.data[c.index++] << 8);
        value += (c.data[c.index++] << 16);
        value += (c.data[c.index++] << 24);
    });

    codec_define_field(binary::Encoder, int32_t, {
        c.data.push_back((value >> 0) & 0xFF);
        c.data.push_back((value >> 8) & 0xFF);
        c.data.push_back((value >> 16) & 0xFF);
        c.data.push_back((value >> 24) & 0xFF);
    });

    codec_define_field(binary::Decoder, int32_t, {
        value = c.data[c.index++];
        value += (c.data[c.index++] << 8);
        value += (c.data[c.index++] << 16);
        value += (c.data[c.index++] << 24);
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

    codec_define_field(binary::Decoder, uint64_t, {
        value = c.data[c.index++];
        value += (c.data[c.index++] << 8);
        value += (c.data[c.index++] << 16);
        value += (c.data[c.index++] << 24);
        value += ((uint64_t)c.data[c.index++] << 32);
        value += ((uint64_t)c.data[c.index++] << 40);
        value += ((uint64_t)c.data[c.index++] << 48);
        value += ((uint64_t)c.data[c.index++] << 56);
    });

    codec_define_field(binary::Encoder, int64_t, {
        c.data.push_back((value >> 0) & 0xFF);
        c.data.push_back((value >> 8) & 0xFF);
        c.data.push_back((value >> 16) & 0xFF);
        c.data.push_back((value >> 24) & 0xFF);
        c.data.push_back((value >> 32) & 0xFF);
        c.data.push_back((value >> 40) & 0xFF);
        c.data.push_back((value >> 48) & 0xFF);
        c.data.push_back((value >> 56) & 0xFF);
    });

    codec_define_field(binary::Decoder, int64_t, {
        value = c.data[c.index++];
        value += (c.data[c.index++] << 8);
        value += (c.data[c.index++] << 16);
        value += (c.data[c.index++] << 24);
        value += ((int64_t)c.data[c.index++] << 32);
        value += ((int64_t)c.data[c.index++] << 40);
        value += ((int64_t)c.data[c.index++] << 48);
        value += ((int64_t)c.data[c.index++] << 56);
    });

    /**
     * STRING
     */
    template <>
    struct Field<binary::Encoder, std::string>
    {
        static void _(binary::Encoder& c,
                      std::string& value,
                      std::vector<binary::Prefix_Type> meta = {
                          binary::Prefix_Type::L8})
        {
            meta = c.get_meta(value, meta);
            binary::Codec::length(c, value, meta);
            c.data.insert(c.data.end(), value.begin(), value.end());
        }
    };

    /**
     * VECTOR
     */
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
                    ::codec::Field<binary::Encoder, T>::_(
                        c,
                        element,
                        std::vector<binary::Prefix_Type>(meta.begin() + 1,
                                                         meta.end()));
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