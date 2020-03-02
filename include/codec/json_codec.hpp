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
        struct Encoder : public ::codec::Codec
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

        struct Decoder : public ::codec::Codec
        {
            nlohmann::json json;
            std::unordered_map<intptr_t, std::string> meta;
        };

        /**
         * Only register meta types which are of the string type.
         * Note: We cannot merge these two register_meta functions into
         * one. The Prefix type, used for meta-data belongs to the same
         * namespace as these functions, so when meta-data is registered in the
         * field() function, other codecs may erroneously pick up these
         * register_meta functions first, because ADL looks in the namespace of
         * Prefix.
         */
        template <class Object, std::size_t N>
        void register_meta(Encoder& c, Object& o, const char (&meta)[N])
        {
            c.meta[(intptr_t)&o] = std::string(meta);
        }

        template <class Object, std::size_t N>
        void register_meta(Decoder& c, Object& o, const char (&meta)[N])
        {
            c.meta[(intptr_t)&o] = std::string(meta);
        }

        /**
         * By using class templates, which can be partially specialiced, and
         * traits (enable_if) we can process integrals in one fell swoop.
         *
         * The base Demultiplex class handles objects, by just calling layout
         * on them again.
         */
        template <class Codec, class Object, class Enabled = void>
        struct Demultiplex
        {
            static void _(Codec& c,
                          Object& o,
                          nlohmann::json::json_pointer pointer)
            {
                auto org_root = c.root;
                c.get(o, pointer) = nlohmann::json::object();
                c.root = pointer;
                layout(c, o);
                c.root = org_root;
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
            static void _(Codec& c,
                          Object& o,
                          nlohmann::json::json_pointer pointer)
            {
                __(c, o, pointer);
            }

            static void __(Encoder& c,
                           Object& o,
                           nlohmann::json::json_pointer pointer)
            {
                c.get(o, pointer) = o;
            }

            static void __(Decoder& c,
                           Object& o,
                           nlohmann::json::json_pointer pointer)
            {}
        };

        /**
         * This version of type() is the main entry point. Almost all fields
         * pass through here.
         */
        template <class Codec, class Object>
        void type(Codec& c,
                  Object& o,
                  nlohmann::json::json_pointer pointer =
                      nlohmann::json::json_pointer())
        {
            Demultiplex<Codec, Object>::_(c, o, pointer);
        }

        void type(Encoder& c,
                  std::string& o,
                  nlohmann::json::json_pointer pointer =
                      nlohmann::json::json_pointer())
        {
            c.get(o, pointer) = o;
        }

        template <class T>
        void type(Encoder& c,
                  std::vector<T>& o,
                  nlohmann::json::json_pointer pointer =
                      nlohmann::json::json_pointer())
        {
            c.get(o, pointer) = nlohmann::json::array();

            for (size_t i = 0; i < o.size(); i++)
                type(c, o[i], pointer / i);
        }
    }
}

#endif