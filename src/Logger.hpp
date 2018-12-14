#pragma once

#include <iostream>
#include <fstream>
#include <memory>

#define FILE_NAME "log.txt"

class Logger
{
    public:
    Logger()
    {
        _logFile.open(FILE_NAME);
        std::ofstream test;
    }

    ~Logger()
    {
        _logFile.close();
    }
        
    template<typename... Args>
    static void v(Args... args)
    {
        createInstanceIfNeeded();
        _logger->logOne("Verbose:", args...);
    }

    template<typename... Args>
    static void d(Args... args)
    {
        createInstanceIfNeeded();
        _logger->logOne("Debug:", args...);
    }

    template<typename... Args>
    static void i(Args... args)
    {
        createInstanceIfNeeded();
        _logger->logOne("Info:", args...);
    }

    template<typename... Args>
    static void w(Args... args)
    {
        createInstanceIfNeeded();
        _logger->logOne("Warning:", args...);
    }
    template<typename... Args>
    static void e(Args... args)
    {
        createInstanceIfNeeded();
        _logger->logOne("Error:", args...);
    }

    private:
    static std::unique_ptr<Logger> _logger;
    std::ofstream _logFile;
    
    void createInstanceIfNeeded()
    {
        if (! _logger)
        {
            _logger = std::make_unique<Logger>();
        }
    }

    template<typename T, typename... Args>
    void logOne(T t, Args... args)
    {
        _logFile << " " << t;
        logOne(args...);
    }

    template<typename T>
    void logOne(T t)
    {
        _logFile << " " << t << std::endl;
        _logFile.flush();
    }

};

