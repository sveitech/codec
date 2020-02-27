#ifndef CODEC_BINARY_CODEC_HPP
#define CODEC_BINARY_CODEC_HPP

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
    }

    class Binary_Codec
    {
    public:
        std::unordered_map<intptr_t, std::vector<binary::Prefix_Type>> meta;
    };

    class Binary_Encode : public Binary_Codec
    {
    public:
        void reset() { data.clear(); }
        std::vector<uint8_t> data;
    };

    class Binary_Decode : public Binary_Codec
    {
    public:
        Binary_Decode() {}
        Binary_Decode(std::vector<uint8_t> const& data) : data(data) {}

        void reset(std::vector<uint8_t> const& data)
        {
            this->data = data;
            index = 0;
        }

        std::vector<uint8_t> data;
        size_t index = 0;
    };

    //
    // meta registration. Using std::enable_if to only activate this
    // template for the Encode/Decode classes that inherit from
    // Binary_Codec
    //
    template <
        class Codec,
        class Object,
        typename std::enable_if<std::is_base_of<Binary_Codec, Codec>::value,
                                int>::type = 0>
    void register_meta(Codec& codec, Object& object, binary::Prefix_Type&& meta)
    {
        codec.meta[(intptr_t)&object].push_back(meta);
    }

    //
    // field() definitions
    //

    // general case. Catch custom structs
    template <class T>
    void field(Binary_Encode& c, T& value)
    {
        printf("custom struct found\n");
        layout(c, value);
    }

    template <class T>
    void field(Binary_Decode& c, T& value)
    {
        layout(c, value);
    }

    //
    // uint8_t
    //
    void field(Binary_Encode& c, uint8_t& value)
    {
        c.data.push_back(value & 0xFF);
    }

    void field(Binary_Decode& c, uint8_t& value) { value = c.data[c.index++]; }

    //
    // int8_t
    //
    void field(Binary_Encode& c, int8_t& value)
    {
        c.data.push_back(value & 0xFF);
    }

    void field(Binary_Decode& c, int8_t& value) { value = c.data[c.index++]; }

    //
    // uint16_t
    //
    void field(Binary_Encode& c, uint16_t& value)
    {
        c.data.push_back((value >> 0) & 0xFF);
        c.data.push_back((value >> 8) & 0xFF);
    }

    void field(Binary_Decode& c, uint16_t& value)
    {
        value = c.data[c.index++];
        value += (c.data[c.index++] << 8);
    }

    //
    // uint32_t
    //
    void field(Binary_Encode& c, uint32_t& value)
    {
        c.data.push_back((value >> 0) & 0xFF);
        c.data.push_back((value >> 8) & 0xFF);
        c.data.push_back((value >> 16) & 0xFF);
        c.data.push_back((value >> 24) & 0xFF);
    }

    void field(Binary_Decode& c, uint32_t& value)
    {
        value = c.data[c.index++];
        value += (c.data[c.index++] << 8);
        value += (c.data[c.index++] << 16);
        value += (c.data[c.index++] << 24);
    }

    template <
        class Codec,
        class Object,
        typename std::enable_if<std::is_base_of<Binary_Codec, Codec>::value,
                                int>::type = 0>
    size_t length(Codec& c, Object& object, binary::Prefix_Type meta)
    {
        switch (meta)
        {
            case binary::Prefix_Type::L8:
            {
                uint8_t length = object.size();
                field(c, length);
                return length;
            }
            case binary::Prefix_Type::L32:
            {
                uint32_t length = object.size();
                field(c, length);
                return length;
            }
        }

        return 0;
    }

    template <class Object, class Meta>
    Meta get_meta(Binary_Codec& c, Object& object, Meta default_meta)
    {
        auto it = c.meta.find((intptr_t)&object);

        if (it != c.meta.end())
            if (it->second.size() > 0)
                return it->second;

        return default_meta;
    }

    //
    // std::string
    //
    void field(Binary_Encode& c,
               std::string& value,
               std::vector<binary::Prefix_Type> meta = {binary::L8})
    {
        meta = get_meta(c, value, meta);
        auto l = length(c, value, meta[0]);

        for (size_t i = 0; i < value.size(); i++)
            c.data.push_back(value[i]);
    }

    void field(Binary_Decode& c,
               std::string& value,
               std::vector<binary::Prefix_Type> meta = {binary::L8})
    {
        meta = get_meta(c, value, meta);
        auto l = length(c, value, meta[0]);

        value =
            std::string(c.data.begin() + c.index, c.data.begin() + c.index + l);
        c.index += l;
    }

    //
    // std::vector
    //

    // Dummy template to satisfy the compiler. This is for the vector
    // implementation only. It will never actually be called.
    template <
        class Codec,
        class Object,
        typename std::enable_if<std::is_base_of<Binary_Codec, Codec>::value,
                                int>::type = 0>
    void field(Codec& c,
               Object& object,
               std::vector<binary::Prefix_Type> const& meta)
    {}

    template <class T>
    void field(Binary_Encode& c,
               std::vector<T>& value,
               std::vector<binary::Prefix_Type> meta = {binary::L8})
    {
        meta = get_meta(c, value, meta);
        length(c, value, meta[0]);

        for (auto& v : value)
        {
            if (meta.size() > 1)
            {
                field(c,
                      v,
                      std::vector<binary::Prefix_Type>(meta.begin() + 1,
                                                       meta.end()));
            }
            else
            {
                field(c, v);
            }
        }
    }

    template <class T>
    void field(Binary_Decode& c,
               std::vector<T>& value,
               std::vector<binary::Prefix_Type> meta = {binary::L8})
    {
        meta = get_meta(c, value, meta);
        auto l = length(c, value, meta[0]);

        value.resize(l);

        for (auto& v : value)
        {
            if (meta.size() > 1)
            {
                field(c,
                      v,
                      std::vector<binary::Prefix_Type>(meta.begin() + 1,
                                                       meta.end()));
            }
            else
            {
                field(c, v);
            }
        }
    }
}

#endif