#ifndef CODEC_JSON_CODEC_HEADER
#define CODEC_JSON_CODEC_HEADER

#include <stack>
#include <string>
#include <unordered_map>

#include "codec/codec.hpp"
#include "nlohmann/json.hpp"

namespace codec
{
    namespace json
    {
        struct Codec
        {
            std::unordered_map<intptr_t, std::string> meta;

            // Get the string name of the field (meta-info)
            template <class T>
            std::string const& name(T& object)
            {
                static const std::string dummy = "";
                auto it = meta.find((intptr_t)&object);

                if (it != meta.end())
                    return it->second;
                else
                    return dummy;
            }

            template <class T>
            void set(T& value, nlohmann::json::json_pointer& pointer)
            {
                adjust_pointer(value, pointer);
                json[pointer] = value;
            }

            template <class T>
            nlohmann::json& create_object(T& value,
                                          nlohmann::json::json_pointer& pointer)
            {
                adjust_pointer(value, pointer);
                json[pointer] = nlohmann::json::object();
                return json[pointer];
            }

            template <class T>
            nlohmann::json& create_list(T& value,
                                        nlohmann::json::json_pointer& pointer)
            {
                adjust_pointer(value, pointer);
                json[pointer] = nlohmann::json::array();
                return json[pointer];
            }

            template <class T>
            void adjust_pointer(T& value, nlohmann::json::json_pointer& pointer)
            {
                if (pointer.empty())
                    pointer = root / pointer;

                auto const& n = name(value);

                if (!n.empty())
                    pointer.push_back(name(value));
            }

            void set_root(nlohmann::json::json_pointer const& pointer)
            {
                root = pointer;
            }

            nlohmann::json json;
            nlohmann::json::json_pointer root;
        };

        struct Encoder : public Codec
        {
            std::string to_string() { return json.dump(4); }
        };

        struct Decoder : public Codec
        {
            void reset(std::string const& json_text)
            {
                json = nlohmann::json::parse(json_text.c_str());
            }
        };
    } // namespace json

    /**
     * META Catchers.
     *
     * The json codec collects any meta arguments in the field() method,
     * which are of the type std::string. All others are ignored.
     */
    codec_define_meta(json::Encoder, std::string, {
        c.meta[(intptr_t)&object] = meta;
    });

    codec_define_meta(json::Decoder, std::string, {
        c.meta[(intptr_t)&object] = meta;
    });

    /**
     * PRIMITIVE FIELDS
     */

    template <class Object>
    struct Field<json::Encoder, Object, nlohmann::json::json_pointer>
    {
        static void _(json::Encoder& c,
                      Object& value,
                      nlohmann::json::json_pointer pointer =
                          nlohmann::json::json_pointer())
        {
            Field<json::Encoder, Object>::_(c, value, pointer);
        }
    };

    template <class Object>
    struct Field<json::Encoder, Object>
    {
        static void _(json::Encoder& c,
                      Object& value,
                      nlohmann::json::json_pointer pointer =
                          nlohmann::json::json_pointer())
        {
            auto original_pointer = c.root;
            c.create_object(value, pointer);
            c.set_root(pointer);
            ::codec::codec(c, value);
            c.set_root(original_pointer);
        }
    };

    template <>
    struct Field<json::Encoder, uint8_t>
    {
        static void _(json::Encoder& c,
                      uint8_t& value,
                      nlohmann::json::json_pointer pointer =
                          nlohmann::json::json_pointer())
        {
            c.set(value, pointer);
        }
    };

    template <>
    struct Field<json::Encoder, uint32_t>
    {
        static void _(json::Encoder& c,
                      uint32_t& value,
                      nlohmann::json::json_pointer pointer =
                          nlohmann::json::json_pointer())
        {
            c.set(value, pointer);
        }
    };

    template <>
    struct Field<json::Encoder, std::string>
    {
        static void _(json::Encoder& c,
                      std::string& value,
                      nlohmann::json::json_pointer pointer =
                          nlohmann::json::json_pointer())
        {
            c.set(value, pointer);
        }
    };

    template <class T>
    struct Field<json::Encoder, std::vector<T>>
    {
        static void _(json::Encoder& c,
                      std::vector<T>& value,
                      nlohmann::json::json_pointer pointer =
                          nlohmann::json::json_pointer())
        {
            c.create_list(value, pointer);

            for (size_t i = 0; i < value.size(); i++)
            {
                auto pointer_index = pointer / i;
                ::codec::Field<json::Encoder, T, nlohmann::json::json_pointer>::
                    _(c, value[i], pointer_index);
            }
        }
    };
}

#endif