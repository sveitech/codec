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
        enum Prefix
        {
            L0,
            L8,
            L16,
            L32,
            L64
        };

        struct Encoder : public ::codec::Codec
        {
            void reset()
            {
                data.clear();
                meta.clear();
            }

            std::vector<uint8_t> data;
            std::unordered_map<intptr_t, std::vector<Prefix>> meta;
        };

        struct Decoder : public ::codec::Codec
        {
            void reset(std::vector<uint8_t> const& data)
            {
                this->data = data;
                index = 0;
                meta.clear();
            }

            std::vector<uint8_t> data;
            size_t index = 0;
            std::unordered_map<intptr_t, std::vector<Prefix>> meta;
        };

        /**
         * Only register meta types which are of the Prefix type.
         * Note: We cannot merge these two register_meta functions into
         * one. The Prefix type, used for meta-data belongs to the same
         * namespace as these functions, so when meta-data is registered in the
         * field() function, other codecs may erroneously pick up these
         * register_meta functions first, because ADL looks in the namespace of
         * Prefix.
         */
        template <class Object>
        void register_meta(Encoder& c, Object& o, Prefix meta)
        {
            c.meta[(intptr_t)&o].push_back(meta);
        }

        template <class Object>
        void register_meta(Decoder& c, Object& o, Prefix meta)
        {
            c.meta[(intptr_t)&o].push_back(meta);
        }

        /**
         * The binary codec passes around meta information as an extra parameter
         * to the type() function. This template allows everything to compile
         * and delegates to the non-meta versions of type. This allows us to
         * mix and match different type() functions with or without meta
         * arguments.
         */
        template <class Codec, class Object, class... T>
        void type(Codec& c, Object& o, T... t)
        {
            type(c, o);
        }

        template <class T, class Codec, class Object>
        size_t codec_prefix(Codec& c, Object& o)
        {
            T length = o.size();
            type(c, length);
            return length;
        }

        template <class Codec, class Object>
        size_t codec_prefix(Codec& c, Object& o, std::vector<Prefix>& prefix)
        {
            auto it = c.meta.find((intptr_t)&o);

            // Found meta override
            if (it != c.meta.end())
                prefix = it->second;

            if (prefix.size() > 0)
            {
                switch (prefix[0])
                {
                    case Prefix::L8:
                        return codec_prefix<uint8_t>(c, o);
                    case Prefix::L16:
                        return codec_prefix<uint16_t>(c, o);
                    case Prefix::L32:
                        return codec_prefix<uint32_t>(c, o);
                    case Prefix::L64:
                        return codec_prefix<uint64_t>(c, o);
                    default:
                        return o.size();
                }
            }

            return o.size();
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
            static void _(Codec& c, Object& o) { layout(c, o); }
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
            static void _(Codec& c, Object& o) { __(c, o); }

            static void __(Encoder& c, Object& o)
            {
                for (size_t i = 0; i < sizeof(Object); i++)
                    c.data.push_back((o >> (i * 8)) & 0xFF);
            }

            static void __(Decoder& c, Object& o)
            {
                o = 0;
                for (size_t i = 0; i < sizeof(Object); i++)
                    o += (Object)c.data[c.index++] << (i * 8);
            }
        };

        /**
         * This version of type() is the main entry point. Almost all fields
         * pass through here.
         */
        template <class Codec, class Object>
        void type(Codec& c, Object& o)
        {
            Demultiplex<Codec, Object>::_(c, o);
        }

        /**
         * Strings
         */
        void type(Encoder& c,
                  std::string& o,
                  std::vector<Prefix> prefix = {Prefix::L8})
        {
            codec_prefix(c, o, prefix);
            c.data.insert(c.data.end(), o.begin(), o.end());
        }

        void type(Decoder& c,
                  std::string& o,
                  std::vector<Prefix> prefix = {Prefix::L8})
        {
            auto const length = codec_prefix(c, o, prefix);
            o = std::string(c.data.begin() + c.index,
                            c.data.begin() + c.index + length);
            c.index += length;
        }

        /**
         * Vectors
         */
        template <class T>
        void type(Encoder& c,
                  std::vector<T>& o,
                  std::vector<Prefix> prefix = {Prefix::L8})
        {
            codec_prefix(c, o, prefix);

            for (auto& e : o)
            {
                if (prefix.size() > 1)
                    type(c,
                         e,
                         std::vector<Prefix>(prefix.begin() + 1, prefix.end()));
                else
                    type(c, e);
            }
        }

        template <class T>
        void type(Decoder& c,
                  std::vector<T>& o,
                  std::vector<Prefix> prefix = {Prefix::L8})
        {
            auto const length = codec_prefix(c, o, prefix);
            o.resize(length);

            for (size_t i = 0; i < length; i++)
            {
                if (prefix.size() > 1)
                    type(c,
                         o[i],
                         std::vector<Prefix>(prefix.begin() + 1, prefix.end()));
                else
                    type(c, o[i]);
            }
        }
    }
}

#endif