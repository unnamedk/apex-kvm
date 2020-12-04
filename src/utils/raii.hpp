#pragma once

namespace apex::utils
{
    template <typename T, typename Fn>
    struct owned_resource
    {
    public:
        owned_resource( T *ptr, Fn &&fn )
            : m_ptr( ptr )
            , m_fn( fn )
        { }
        ~owned_resource()
        {
            m_fn( m_ptr );
        }

        operator T *() { return this->m_ptr; }

    private:
        T *m_ptr;
        Fn &&m_fn;
    };
}