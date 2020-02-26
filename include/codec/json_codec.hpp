#ifndef CODEC_JSON_CODEC_HEADER
#define CODEC_JSON_CODEC_HEADER

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

            // Get current active json object
            nlohmann::json top() { return json[pointer]; }

            nlohmann::json json;
            nlohmann::json::json_pointer pointer;
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
    codec_define_field(json::Encoder, uint8_t, {
        c.json[c.name(value)] = value;
    });

    codec_define_field(json::Decoder, uint8_t, {
        value = c.top()[c.name(value)];
    });

    codec_define_field(json::Encoder, int8_t, {
        c.json[c.name(value)] = value;
    });

    codec_define_field(json::Decoder, int8_t, {
        value = c.top()[c.name(value)];
    });

    codec_define_field(json::Encoder, uint32_t, {
        c.json[c.name(value)] = value;
    });

    codec_define_field(json::Encoder, std::string, {
        c.json[c.name(value)] = value;
    });

    codec_define_field(json::Decoder, std::string, {
        value = c.json[c.name(value)];
    });

    template <class T>
    struct Field<json::Encoder, std::vector<T>>
    {
        static void _(json::Encoder& c, std::vector<T>& value) 
        {
            
        }
    };
}

#endif