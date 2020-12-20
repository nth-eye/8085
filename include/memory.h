#include <array>

#include "types.h"
#include "misc.h"

#pragma once

namespace intel_8085 {

struct Memory {
    Byte&   operator[](size_t idx)                { return mem[idx]; }
    Memory& operator()(size_t idx)                { ptr = idx; return *this; }
    Memory& operator= (const std::initializer_list<Byte> &other) 
    {
        if (other.size() > mem.size() - ptr) {
            sout 
            << "Instructions require more memory than available from address: 0x" 
            << std::hex << ptr;
        }
        for (auto it : other)
            mem[ptr++] = it;

        return *this;
    }
    void clear()
    {
        for (size_t i = 0; i < mem.size(); ++i)
            mem[i] = 0;
    }
    Byte* data()        { return mem.data(); }
    size_t size() const { return mem.size(); }
private:
    size_t ptr = 0;
    std::array<Byte, 0xFFFF+1> mem = {};
};

} // namespace intel_8085