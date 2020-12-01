#pragma once

namespace apex::utils
{
    template <class BaseClass>
    struct singleton
    {
        static BaseClass &get()
        {
            static BaseClass base;
            return base;
        }
    };
}