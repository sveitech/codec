#ifndef CODEC_CODEC_HEADER
#define CODEC_CODEC_HEADER

namespace codec
{
    template <class C, class O>
    void codec(C& c, O& o)
    {
        layout(c, o);
    }
}

#endif