#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

const std::string loglevel = "DEBUG";

class log
{
public:
    static void debug(const std::string &message)
    {   
        if (loglevel != "DEBUG") return;
        logWithLevel(message, "DEBUG");
    }

    static void info(const std::string &message)
    {   
        if (loglevel != "DEBUG" && loglevel != "INFO") return;
        logWithLevel(message, "INFO");
    }

    static void warn(const std::string &message)
    {
        if (loglevel != "DEBUG" && loglevel != "INFO" && loglevel != "WARN") return;
        logWithLevel(message, "WARN");
    }

    static void error(const std::string &message)
    {
        logWithLevel(message, "ERROR");
    }

private:
    static void logWithLevel(const std::string &message, const std::string &level)
    {
        std::cout << "[" << currentTimestamp() << " | " << level << "] " << message << std::endl;
    }

    static std::string currentTimestamp()
    {
        auto now = std::time(nullptr);
        std::tm tm;
        localtime_r(&now, &tm); // i think this wont work in windows
        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M:%S");
        return oss.str();
    }
};

#endif // LOGGER_H