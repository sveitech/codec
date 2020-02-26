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

            template <class T>
            std::string const& get_meta(T& object)
            {
                static const std::string dummy = "";
                auto it = meta.find((intptr_t)&object);

                if (it != meta.end())
                    return it->second;
                else
                    return dummy;
            }
        };

        struct Encoder : public Codec
        {
            std::string to_string() { return json.dump(4); }

            nlohmann::json json;
        };

        struct Decoder : public Codec
        {};
    } // namespace json

    /**
     * META Catchers.
     *
     * The json codec collects any meta arguments in the field() method,
     * which are of the type std::string. All others are ignored.
     */
    codec_define_meta(json::Encoder, std::string, {
        printf("Registering meta for : %u\n", (intptr_t)&object);
        c.meta[(intptr_t)&object] = meta;
    });

    codec_define_meta(json::Decoder, std::string, {
        c.meta[(intptr_t)&object] = meta;
    });

    /**
     * PRIMITIVE FIELDS
     */
    codec_define_field(json::Encoder, uint8_t, {
        c.json[c.get_meta(value)] = value;
    });

    codec_define_field(json::Decoder,
                       uint8_t,
                       {
                           // TODO
                       });

    codec_define_field(json::Encoder, uint32_t, {
        c.json[c.get_meta(value)] = value;
    });

    codec_define_field(json::Encoder, std::string, {
        c.json[c.get_meta(value)] = value;
    });
}

#endif