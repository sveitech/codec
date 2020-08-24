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
        /**
         * IMPORTANT!
         *
         * Meta information, which is added in the codec::field() function,
         * CANNOT live in the same namespace as the function register_meta().
         * Because of ADL, if a field() contains a binary::Prefix, and a json
         * loader is used, for example, the register_meta() for the binary
         * codec will still be invoked, because ADL adds the namespace of
         * Prefix to the lookup. We need to ensure that ADL ONLY finds the
         * dummy register_meta() function in codec.hpp, when parsing meta fields
         * not relevant for the current codec.
         */
        namespace meta
        {
            enum Prefix
            {
                NONE,
                L0,
                L8,
                L16,
                L32,
                L64
            };
        }

        struct Encoder : public ::codec::Codec
        {
            void reset()
            {
                data.clear();
                meta.clear();
            }

            std::vector<uint8_t> data;
            std::unordered_map<intptr_t, std::vector<meta::Prefix>> meta;
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
            std::unordered_map<intptr_t, std::vector<meta::Prefix>> meta;
        };

        /**
         * Only register meta types which are of the Prefix type.
         */
        template <class Codec, class Object>
        void register_meta(Codec& codec, Object& object, meta::Prefix meta)
        {
            codec.meta[(intptr_t)&object].push_back(meta);
        }

        /**
         * The binary codec passes around meta information as an extra parameter
         * to the type() function. This template allows everything to compile
         * and delegates to the non-meta versions of type. This allows us to
         * mix and match different type() functions with or without meta
         * arguments.
         */
        template <class Codec, class Object, class... T>
        void type(Codec& codec, Object& object, T... t)
        {
            type(codec, object);
        }

        template <class Codec, class Object>
        meta::Prefix get_meta(Codec& codec,
                              Object& object,
                              std::vector<meta::Prefix> const& prefix)
        {
            auto it = codec.meta.find((intptr_t)&object);

            if (it != codec.meta.end())
                if (prefix.size() > 0)
                    return prefix[0];

            return meta::Prefix::NONE;
        }

        template <class T, class Codec, class Object>
        size_t codec_prefix(Codec& codec, Object& object)
        {
            T length = object.size();
            type(codec, length);
            return length;
        }

        template <class Codec, class Object>
        size_t codec_prefix(Codec& codec,
                            Object& object,
                            std::vector<meta::Prefix>& prefix)
        {
            auto p = get_meta(codec, object, prefix);

            switch (p)
            {
                case meta::Prefix::L8:
                    return codec_prefix<uint8_t>(codec, object);
                case meta::Prefix::L16:
                    return codec_prefix<uint16_t>(codec, object);
                case meta::Prefix::L32:
                    return codec_prefix<uint32_t>(codec, object);
                case meta::Prefix::L64:
                    return codec_prefix<uint64_t>(codec, object);
                default:
                    return object.size();
            }

            return object.size();
        }

        template <class Codec, class Object>
        void encode_primitive(Codec& codec, Object& object)
        {
            
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
            static void _(Codec& codec, Object& object)
            {
                layout(codec, object);
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
            static void _(Codec& codec, Object& object) { __(codec, object); }

            static void __(Encoder& codec, Object& object)
            {
                for (size_t i = 0; i < sizeof(Object); i++)
                    codec.data.push_back((object >> (i * 8)) & 0xFF);
            }

            static void __(Decoder& codec, Object& object)
            {
                object = 0;
                for (size_t i = 0; i < sizeof(Object); i++)
                    object += (Object)codec.data[codec.index++] << (i * 8);
            }
        };

        /**
         * This version of type() is the main entry point. Almost all fields
         * pass through here.
         */
        template <class Codec, class Object>
        void type(Codec& codec, Object& object)
        {
            Demultiplex<Codec, Object>::_(codec, object);
        }

        /**
         * Strings.
         *
         * NOTE: inline to allow inclusion of these non-template functions in
         * multiple compilation units. They could be non-inline, and placed
         * in a cpp file, but it would
         * create a disconnect between these functions and the rest of this
         * header, which is entirely template functions.
         * The only disadvantage of inlining is that the function has a
         * definition per compilation unit; same as for alle the other template
         * functions here.
         */
        inline void type(Encoder& codec,
                         std::string& object,
                         std::vector<meta::Prefix> prefix = {meta::Prefix::L8})
        {
            codec_prefix(codec, object, prefix);
            codec.data.insert(codec.data.end(), object.begin(), object.end());
        }

        inline void type(Decoder& codec,
                         std::string& object,
                         std::vector<meta::Prefix> prefix = {meta::Prefix::L8})
        {
            auto const length = codec_prefix(codec, object, prefix);
            object = std::string(codec.data.begin() + codec.index,
                                 codec.data.begin() + codec.index + length);
            codec.index += length;
        }

        /**
         * Vectors
         */
        template <class T>
        void type(Encoder& codec,
                  std::vector<T>& object,
                  std::vector<meta::Prefix> prefix = {meta::Prefix::L8})
        {
            codec_prefix(codec, object, prefix);

            for (auto& e : object)
            {
                if (prefix.size() > 1)
                    type(codec,
                         e,
                         std::vector<meta::Prefix>(prefix.begin() + 1,
                                                   prefix.end()));
                else
                    type(codec, e);
            }
        }

        template <class T>
        void type(Decoder& codec,
                  std::vector<T>& object,
                  std::vector<meta::Prefix> prefix = {meta::Prefix::L8})
        {
            auto const length = codec_prefix(codec, object, prefix);
            object.resize(length);

            for (size_t i = 0; i < length; i++)
            {
                if (prefix.size() > 1)
                    type(codec,
                         object[i],
                         std::vector<meta::Prefix>(prefix.begin() + 1,
                                                   prefix.end()));
                else
                    type(codec, object[i]);
            }
        }
    }
}

#endif