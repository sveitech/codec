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

            // Get current active json object
            nlohmann::json& top() { return json[pointer]; }

            template <class T>
            void set(T& object)
            {
                if (get_type() == Object_Type::ELEMENT)
                    top()[name(object)] = object;
                else if (get_type() == Object_Type::LIST)
                    push(object);
            }

            template <class T>
            void push(T& object)
            {
                top().push_back(object);
            }

            template <class T>
            void enter(T& object, Object_Type type)
            {
                this->type.push(type);

                if (type == Object_Type::OBJECT)
                {
                    pointer.push_back(name(object));
                    top()[name(object)] = nlohmann::json::object();
                }
                else if (type == Object_Type::LIST)
                {
                    pointer.push_back(name(object));
                    top()[name(object)] = nlohmann::json::array();
                }
            }

            void exit()
            {
                pointer = pointer.parent_pointer();
                type.pop();
            }

            Object_Type get_type()
            {
                if (type.size() > 0)
                    return type.top();
                else
                    return Object_Type::ELEMENT;
            }

            nlohmann::json* init(nlohmann::json* json)
            {
                if (json == nullptr)
                    return &this->json;
                else
                    return json;
            }

            nlohmann::json json;
            nlohmann::json::json_pointer pointer;
            std::stack<Object_Type> type;
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
                pointer = nlohmann::json::json_pointer();
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
                      nlohmann::json* json = nullptr)
        {
            Field<json::Encoder, Object>::_(c, value, json);
        }
    };

    template <class Object>
    struct Field<json::Encoder, Object>
    {
        static void _(json::Encoder& c,
                      Object& value,
                      nlohmann::json* json = nullptr)
        {
            // printf("a\n");
            // c.enter(value, json::Object_Type::OBJECT);
            // printf("b\n");

            // c.exit();
            // if (c.get_type() == json::Object_Type::ELEMENT)
            // {
            //     c.top()[c.name(value)] = nlohmann::json::object();
            // }
            // else
            // {
            //     c.top().push_back(nlohmann::json::object());
            // }

            // c.enter(value, json::Object_Type::ELEMENT);
            // c.top() = nlohmann::json::object();
            // ::codec::codec(c, value);
            // c.exit();
        }
    };

    // codec_define_field(json::Encoder, uint8_t, { c.set(value); });

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
                      nlohmann::json* json = nullptr)
        {
            json = c.init(json);

            (*json)[c.name(value)] = nlohmann::json::array();
            // pointer.push_back(c.name(value));
            // c.type.push(json::Object_Type::LIST);
            // c.json[pointer] = nlohmann::json::array();

            // for (auto& element : value)
            //     Field<json::Encoder, T>::_(c, element, pointer);
            // c.enter(value, json::Object_Type::LIST);

            // for (auto& v : value)
            //     ::codec::field(c, v);

            // c.exit();
        }
    };
}

#endif