#pragma once

#include "d3d9_include.h"
#include "d3d9_options.h"

#include <string>
#include <mutex>
#include <fstream>
#include <iostream>

class ThreadSafeLogger {

public:

  ThreadSafeLogger() = delete;
  ThreadSafeLogger(const ThreadSafeLogger&) = delete;
  ThreadSafeLogger& operator=(const ThreadSafeLogger&) = delete;

  static void initializeFile();

  static void debug(const std::string& message);

  static void info(const std::string& message);

  static void warn(const std::string& message);

  static void warn(REFIID riid);

  static void err(const std::string& message);

private:

  enum class LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3
  };

  inline static std::string getTimestamp();

  static void logWithLevel(LogLevel logLevel, const std::string& prefix, const std::string& message);

  static LogLevel   s_level;

  static bool       s_writeToLogFile;
  static bool       s_fileInitialized;

  static std::mutex s_logMutex;

};
