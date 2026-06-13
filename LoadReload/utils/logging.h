#pragma once
#include <iostream>
#include <string>

namespace Log {

    enum class LogLevel { Msg, Warn, Error };

    inline void Write(LogLevel lvl, const std::string& msg) {
        switch (lvl) {
        case LogLevel::Msg:  std::cout << "[INFO] "; break;
        case LogLevel::Warn:  std::cout << "[WARN] "; break;
        case LogLevel::Error: std::cout << "[ERROR] "; break;
        }
        std::cout << msg << std::endl;
    }

} // namespace Log

