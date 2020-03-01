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
        struct Primitive
        {};

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
    struct Field<json::Encoder, Object>
    {
        // Catch-all. If none of the other functions instantiate, this template
        // will be invoked instead.
        template <class... T>
        static void __(T... args)
        {}

        template <
            class Codec,
            class T,
            typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
        static void __(Codec& c,
                       T& object,
                       nlohmann::json::json_pointer pointer)
        {
            c.set(object, pointer);
        }

        template <
            class Codec,
            class T,
            typename std::enable_if<!std::is_integral<T>::value, int>::type = 0>
        static void __(Codec& c,
                       T& object,
                       nlohmann::json::json_pointer pointer)
        {
            auto original_root = c.root;
            c.create_object(object, pointer);
            c.set_root(pointer);
            ::codec::codec(c, object);
            c.set_root(original_root);
        }

        static void _(json::Encoder& c,
                      Object& value,
                      nlohmann::json::json_pointer pointer =
                          nlohmann::json::json_pointer())
        {
            __(c, value, pointer);
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
                ::codec::Field<json::Encoder, T>::_(c, value[i], pointer_index);
            }
        }
    };
}

#endif