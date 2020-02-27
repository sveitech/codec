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
        enum class Object_Type
        {
            OBJECT,
            ELEMENT,
            LIST
        };

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
            void set(T& value,
                     nlohmann::json::json_pointer& pointer,
                     json::Object_Type type)
            {
                adjust_pointer(pointer);

                if (type != json::Object_Type::LIST)
                    pointer.push_back(name(value));

                json[pointer] = value;
            }

            template <class T>
            nlohmann::json& create_object(T& value,
                                          nlohmann::json::json_pointer& pointer)
            {
                adjust_pointer(pointer);

                pointer.push_back(name(value));
                json[pointer] = nlohmann::json::object();
                return json[pointer];
            }

            template <class T>
            nlohmann::json& create_list(T& value,
                                        nlohmann::json::json_pointer& pointer)
            {
                adjust_pointer(pointer);

                pointer.push_back(name(value));
                json[pointer] = nlohmann::json::array();
                return json[pointer];
            }

            void adjust_pointer(nlohmann::json::json_pointer& pointer)
            {
                pointer = root / pointer;

                printf("Pointer adjusted to: %s\n",
                       pointer.to_string().c_str());
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
                // pointer = nlohmann::json::json_pointer();
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
    struct Field<json::Encoder,
                 Object,
                 nlohmann::json::json_pointer,
                 json::Object_Type>
    {
        static void _(json::Encoder& c,
                      Object& value,
                      nlohmann::json::json_pointer pointer =
                          nlohmann::json::json_pointer(),
                      json::Object_Type type = json::Object_Type::ELEMENT)
        {
            Field<json::Encoder, Object>::_(c, value, pointer, type);
        }
    };

    template <class Object>
    struct Field<json::Encoder, Object>
    {
        static void _(json::Encoder& c,
                      Object& value,
                      nlohmann::json::json_pointer pointer =
                          nlohmann::json::json_pointer(),
                      json::Object_Type type = json::Object_Type::ELEMENT)
        {
            printf("object. pointer: %s\n", pointer.to_string().c_str());
            auto original_pointer = pointer;
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
                          nlohmann::json::json_pointer(),
                      json::Object_Type type = json::Object_Type::ELEMENT)
        {
            printf("uint8_t: %s\n", pointer.to_string().c_str());
            c.set(value, pointer, type);
            printf("uint8_t done\n");
        }
    };

    // codec_define_field(json::Decoder, uint8_t, {
    //     value = c.top()[c.name(value)];
    // });

    // codec_define_field(json::Encoder, int8_t, {
    //     c.json[c.name(value)] = value;
    // });

    // codec_define_field(json::Decoder, int8_t, {
    //     value = c.top()[c.name(value)];
    // });

    // codec_define_field(json::Encoder, uint32_t, { c.set(value); });

    // codec_define_field(json::Encoder, std::string, {
    //     c.json[c.name(value)] = value;
    // });

    // codec_define_field(json::Decoder, std::string, {
    //     value = c.json[c.name(value)];
    // });

    template <class T>
    struct Field<json::Encoder, std::vector<T>>
    {
        static void _(json::Encoder& c,
                      std::vector<T>& value,
                      nlohmann::json::json_pointer pointer =
                          nlohmann::json::json_pointer(),
                      json::Object_Type type = json::Object_Type::ELEMENT)
        {
            printf("vector. pointer: %s\n", pointer.to_string().c_str());
            c.create_list(value, pointer);

            for (size_t i = 0; i < value.size(); i++)
            {
                auto pointer_index = pointer / i;
                ::codec::Field<json::Encoder,
                               T,
                               nlohmann::json::json_pointer,
                               json::Object_Type>::_(c,
                                                     value[i],
                                                     pointer_index,
                                                     json::Object_Type::LIST);
            }
        }
    };
}

#endif