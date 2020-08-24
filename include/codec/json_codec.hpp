#ifndef CODEC_JSON_CODEC_HEADER
#define CODEC_JSON_CODEC_HEADER

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "codec/codec.hpp"
#include "nlohmann/json.hpp"

namespace codec
{
    namespace json
    {
        struct Codec
        {
            std::string to_pretty_string() { return json.dump(4); }
            std::string to_string() { return json.dump(); }

            template <class T>
            nlohmann::json& get(T& value, nlohmann::json::json_pointer& pointer)
            {
                reroot(value, pointer);
                return json[pointer];
            }

            template <class T>
            void reroot(T& value, nlohmann::json::json_pointer& pointer)
            {
                if (pointer.empty())
                    pointer = root / pointer;

                auto const& n = name(value);

                if (not n.empty())
                    pointer /= n;
            }

            template <class T>
            std::string const& name(T& value)
            {
                static const std::string empty = "";
                auto it = meta.find((intptr_t)&value);

                if (it != meta.end())
                    return it->second;
                else
                    return empty;
            }

            nlohmann::json json;
            nlohmann::json::json_pointer root;
            std::unordered_map<intptr_t, std::string> meta;
        };

        struct Encoder : public ::codec::Codec, public Codec
        {
            void reset()
            {
                json = nlohmann::json();
                root = nlohmann::json::json_pointer();
                meta.clear();
            }
        };

        struct Decoder : public ::codec::Codec, public Codec
        {
            void reset(std::string const& json)
            {
                this->json = nlohmann::json::parse(json);
                meta.clear();
            }
        };

        /**
         * Only register meta types which are of the string type.
         */
        template <class Codec, class Object, std::size_t N>
        void register_meta(Codec& codec, Object& object, const char (&meta)[N])
        {
            codec.meta[(intptr_t)&object] = std::string(meta);
        }

        /**
         * By using class templates, which can be partially specialized, and
         * traits (enable_if) we can process integrals in one fell swoop.
         *
         * The base Demultiplex class handles objects, by just calling layout
         * on them again.
         */
        template <class Codec, class Object, class Enabled = void>
        struct Demultiplex
        {
            static void _(Codec& codec,
                          Object& object,
                          nlohmann::json::json_pointer pointer)
            {
                __(codec, object, pointer);
            }

            static void __(Encoder& codec,
                           Object& object,
                           nlohmann::json::json_pointer pointer)
            {
                auto org_root = codec.root;
                codec.get(object, pointer) = nlohmann::json::object();
                codec.root = pointer;
                layout(codec, object);
                codec.root = org_root;
            }

            static void __(Decoder& codec,
                           Object& object,
                           nlohmann::json::json_pointer pointer)
            {
                // Invoke get, just to get pointer updated. No need to create
                // an objet, as there already is an object here.
                auto org_root = codec.root;
                codec.get(object, pointer);
                codec.root = pointer;
                layout(codec, object);
                codec.root = org_root;
            }
        };

        /**
         * This specialization is for integrals. It only instantiates if the
         * Object is an integral.
         */
        template <class Codec, class Object>
        struct Demultiplex<
            Codec,
            Object,
            typename std::enable_if<std::is_integral<Object>::value>::type>
        {
            static void _(Codec& codec,
                          Object& object,
                          nlohmann::json::json_pointer pointer)
            {
                __(codec, object, pointer);
            }

            static void __(Encoder& codec,
                           Object& object,
                           nlohmann::json::json_pointer pointer)
            {
                codec.get(object, pointer) = object;
            }

            static void __(Decoder& codec,
                           Object& object,
                           nlohmann::json::json_pointer pointer)
            {
                object = (Object)codec.get(object, pointer);
            }
        };

        /**
         * This version of type() is the main entry point. Almost all fields
         * pass through here.
         */
        template <class Codec, class Object>
        void type(Codec& codec,
                  Object& object,
                  nlohmann::json::json_pointer pointer =
                      nlohmann::json::json_pointer())
        {
            Demultiplex<Codec, Object>::_(codec, object, pointer);
        }

        /**
         * String
         */
        inline void type(Encoder& codec,
                  std::string& object,
                  nlohmann::json::json_pointer pointer =
                      nlohmann::json::json_pointer())
        {
            codec.get(object, pointer) = object;
        }

        inline void type(Decoder& codec,
                  std::string& object,
                  nlohmann::json::json_pointer pointer =
                      nlohmann::json::json_pointer())
        {
            object = (std::string)codec.get(object, pointer);
        }

        /**
         * Vector
         */
        template <class T>
        void type(Encoder& codec,
                  std::vector<T>& object,
                  nlohmann::json::json_pointer pointer =
                      nlohmann::json::json_pointer())
        {
            codec.get(object, pointer) = nlohmann::json::array();

            for (size_t i = 0; i < object.size(); i++)
                type(codec, object[i], pointer / i);
        }

        template <class T>
        void type(Decoder& codec,
                  std::vector<T>& object,
                  nlohmann::json::json_pointer pointer =
                      nlohmann::json::json_pointer())
        {
            auto& json = codec.get(object, pointer);
            object.resize(json.size());

            for (size_t i = 0; i < object.size(); i++)
                type(codec, object[i], pointer / i);
        }
    }
}

#endif