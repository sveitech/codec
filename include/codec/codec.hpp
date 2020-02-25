#ifndef CODEC_CODEC_HPP
#define CODEC_CODEC_HPP

#include <cstdint>
#include <stack>
#include <unordered_map>
#include <vector>

namespace codec
{
    class Meta_Object
    {
    public:
    };

    // Default layout function. Specialize to create layout functions
    // for each custom object.
    template <class Codec, class Object>
    void layout(Codec& codec, Object& object);

    // Bare minimum specializations for fields
    template <class Codec, class Object, class Meta>
    void register_meta(Codec& codec, Object& object, Meta&& meta)
    {
        // In the default case, ignore metas. Codecs should specialise
        // this template for each meta it supports.
        printf("Ignored meta\n");
    }

    template <class Codec, class Object>
    void field(Codec& codec, Object& object);

    template <class Codec, class Object, class... Meta>
    void field(Codec& codec, Object& object, Meta&&... meta)
    {
        (register_meta(codec, object, std::forward<Meta>(meta)), ...);
        field(codec, object);
    }
}

#endif
