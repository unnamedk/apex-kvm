#include <cstdint>

#include "memflow.h"
#include "memflow_win32.h"

#include "utils/singleton.hpp"

namespace apex::cheats
{
    class misc : public utils::singleton<misc>
    {
    public:
        void run();
    };
}