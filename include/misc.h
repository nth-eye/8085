#include <mutex>
#include <sstream>
#include <iostream>

#pragma once

// Synchronized thread-safe cout before C++20.
class sync_out : public std::ostringstream {
    static std::mutex mut;
    std::ostream &os;
public:
    sync_out(std::ostream &os_) : os(os_) 
    {
        // imbue(os.getloc());
        // precision(os.precision());
        // width(os.width());
        // setf(std::ios::fixed, std::ios::floatfield);
    }
    ~sync_out()
    {
        std::lock_guard guard(mut);
        std::cout << this->str() << std::endl;
    }
};
inline std::mutex sync_out::mut{};

#define serr sync_out(std::cerr)
#define sout sync_out(std::cout)