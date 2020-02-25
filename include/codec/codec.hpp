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

    class Meta
    {
        public:
            Meta() {}
            Meta(std::initializer_list<Meta_Object> init) {}

    };

    // Default layout function. Specialize to create layout functions
    // for each custom object.
    template <class Codec, class Object>
    void layout(Codec& codec, Object& object);

    template<class Codec, class Object>
    void field(Codec& codec, Object& object, Meta& meta = {});
}

#endif
