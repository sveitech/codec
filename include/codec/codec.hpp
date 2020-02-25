#ifndef CODEC_CODEC_HPP
#define CODEC_CODEC_HPP

#include <cstdint>
#include <stack>
#include <unordered_map>
#include <vector>

namespace codec
{
    // Default layout function. Specialize to create layout functions
    // for each custom object.
    template <class Codec, class Object>
    void layout(Codec& codec, Object& object)
    {
        printf("dummy layout\n");
    }

    // Bare minimum specializations for fields
    template <class Codec, class Object, class Meta>
    void register_meta(Codec& codec, Object& object, Meta&& meta)
    {
        // In the default case, ignore metas. Codecs should specialise
        // this template for each meta it supports.
    }

    template <class Codec, class Object>
    void field(Codec& codec, Object& object)
    {}

    template <class Codec, class Object, class... Meta>
    void field(Codec& codec, Object& object, Meta&&... meta)
    {
        printf("field with meta\n");
        (register_meta(codec, object, std::forward<Meta>(meta)), ...);
        field(codec, object);
    }
}

#endif
