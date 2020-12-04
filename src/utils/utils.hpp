#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <functional>

#include "memflow.h"
#include "memflow_win32.h"

namespace apex
{
    extern Kernel *main_kernel;
    namespace utils
    {
        // automatically generated
        enum class keycode_t : std::uint8_t
        {
            lbutton = 0x01,
            rbutton = 0x02,
            cancel = 0x03,
            mbutton = 0x04,
            xbutton1 = 0x05,
            xbutton2 = 0x06,
            back = 0x08,
            tab = 0x09,
            clear = 0x0C,
            enter = 0x0D,
            shift = 0x10,
            control = 0x11,
            menu = 0x12,
            pause = 0x13,
            capital = 0x14,
            kana = 0x15,
            hangeul = kana,
            hangul = kana,
            junja = 0x17,
            final = 0x18,
            hanja = 0x19,
            kanji = hanja,
            escape = 0x1B,
            convert = 0x1C,
            nonconvert = 0x1D,
            accept = 0x1E,
            modechange = 0x1F,
            space = 0x20,
            prior = 0x21,
            next = 0x22,
            end = 0x23,
            home = 0x24,
            left = 0x25,
            up = 0x26,
            right = 0x27,
            down = 0x28,
            select = 0x29,
            print = 0x2A,
            execute = 0x2B,
            snapshot = 0x2C,
            insert = 0x2D,
        };

        bool is_key_down( keycode_t code );
        bool are_movement_keys_pressed();
        long current_tick();

        std::optional<std::tuple<std::string, PID, Address>> get_process( Kernel *kernel, std::function<bool( std::string_view, PID )> validate_fn );
    }
}