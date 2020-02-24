#ifndef CODEC_CODEC_HPP
#define CODEC_CODEC_HPP

#include <cstdint>
#include <stack>
#include <unordered_map>
#include <vector>

namespace codec
{
    class Codec
    {
    public:
        virtual ~Codec() = default;
        virtual void reset() = 0;
    };

    template <class M>
    M default_meta();

    template <class M>
    class New_Meta
    {
        template <class T>
        M get(T& member)
        {
        }

    private:
        std::unordered_map<intptr_t, std::vector<M>> m_meta;
        std::unordered_map<intptr_t, uint32_t> m_depth;
        std::stack<intptr_t> m_current_member;
    };

    template <class M>
    class Meta_Base
    {
    public:
        Meta_Base() : m_default(default_meta<M>()) {}

        template <class T>
        void push_member(T& member)
        {
            intptr_t pointer = (intptr_t)&member;

            auto it = meta.find(pointer);

            if (it != meta.end())
            {
                m_members.push(pointer);
            }
        }

        template <class T>
        void pop_member(T& member)
        {
            intptr_t pointer = (intptr_t)&member;

            auto it = meta.find(pointer);

            if (it != meta.end())
            {
                m_members.pop();
            }
        }

        template <class T>
        void set(T& member, M meta_data)
        {
            meta[(intptr_t)&member].push(meta_data);
        }

        template <class T>
        void set(T& member, std::vector<M> const& meta_data)
        {
            for (ssize_t i = meta_data.size() - 1; i >= 0; i--)
                meta[(intptr_t)&member].push(meta_data[i]);
        }

        template <class T>
        M pop(T& member)
        {
            return pop((intptr_t)&member);
        }

        M pop(intptr_t member)
        {
            auto it = meta.find(member);

            if (it != meta.end())
            {
                auto& stack = it->second;

                if (stack.size() > 0)
                {
                    M m = stack.top();
                    stack.pop();
                    popped_meta[member].push(m);
                    return m;
                }
            }

            return m_default;
        }

        M pop()
        {
            if (m_members.size() > 0)
            {
                auto pointer = m_members.top();
                return pop(pointer);
            }

            return m_default;
        }

        template <class T>
        void rollback(T& member)
        {
            rollback((intptr_t)&member);
        }

        void rollback(intptr_t member)
        {
            auto it = popped_meta.find(member);

            if (it != popped_meta.end())
            {
                auto& vec = it->second;

                if (vec.size() > 0)
                {
                    M m = vec.top();
                    vec.pop();
                    meta[member].push(m);
                }
            }
        }

        void rollback()
        {
            if (m_members.size() > 0)
            {
                rollback(m_members.top());
            }
        }

        std::unordered_map<intptr_t, std::stack<M>> meta;
        std::unordered_map<intptr_t, std::stack<M>> popped_meta;

    private:
        M m_default;
        std::stack<intptr_t> m_members;
    };

    // Default layout function. Specialize to create layout functions
    // for each custom object.
    template <class Codec, class Object>
    void layout(Codec& codec, Object& object)
    {
    }

    template <class Codec, class Object>
    void layout_meta(Codec& codec, Object& object)
    {
    }

    // Default field function. Specialize to create fields for
    // each particular codec.
    template <class Codec, class Type>
    void field(Codec& codec, Type& type)
    {
    }

    template <class Codec, class Type>
    void meta(Codec& codec, Type& type)
    {
    }

    template <class Codec, class Object>
    Codec& codec(Object& object)
    {
        static Codec codec;
        codec.reset();
        layout_meta(codec, object);
        layout(codec, object);
        return codec;
    }
}

#endif
