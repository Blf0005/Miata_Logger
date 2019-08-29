/*
 * Logger.h
 *
Copyright (c) 2019 PREMERGY,INC / BLAKELY FABIANI

 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <Arduino.h>
#include "config.h"

class Logger {
public:
    enum LogLevel {
        Debug = 0, Info = 1, Warn = 2, Error = 3, Off = 4
    };
    static void debug(char *, ...);
    static void info(char *, ...);
    static void warn(char *, ...);
    static void error(char *, ...);
    static void console(char *, ...);
    static void setLoglevel(LogLevel);
    static LogLevel getLogLevel();
    static uint32_t getLastLogTime();
    static boolean isDebug();
private:
    static LogLevel logLevel;
    static uint32_t lastLogTime;

    static void log(LogLevel, char *format, va_list);
    static void logMessage(char *format, va_list args);
};

#endif /* LOGGER_H_ */


