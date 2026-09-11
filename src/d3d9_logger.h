#pragma once

#include <string>
#include <mutex>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
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
  static void err(const std::string& message);

private:
  enum class LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3
  };

  static std::mutex s_logMutex;
  static LogLevel s_level;
  static bool s_useLogFile;
  static bool s_fileInitialized;

  static std::string getTimestamp();
  static void logWithLevel(LogLevel logLevel, const std::string& prefix, const std::string& message);
};
