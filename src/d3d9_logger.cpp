#include "d3d9_logger.h"

std::mutex ThreadSafeLogger::s_logMutex;
ThreadSafeLogger::LogLevel ThreadSafeLogger::s_level = ThreadSafeLogger::LogLevel::LOG_WARN;
bool ThreadSafeLogger::s_useLogFile = false;
bool ThreadSafeLogger::s_fileInitialized = false;

std::string ThreadSafeLogger::getTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  auto localTime = std::localtime(&time);

  std::ostringstream oss;
  oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

void ThreadSafeLogger::initializeFile() {
  if (s_useLogFile) {
    std::lock_guard<std::mutex> lock(s_logMutex);
    if (!s_fileInitialized) {
      std::ofstream file("d3d9to8.log", std::ios::trunc);
      file.close();
      s_fileInitialized = true;
    }
  }
}

void ThreadSafeLogger::logWithLevel(LogLevel logLevel, const std::string& prefix, const std::string& message) {
  if (logLevel < s_level) {
    return;
  }

  std::lock_guard<std::mutex> lock(s_logMutex);

  std::string timestamp = getTimestamp();
  std::string logEntry = "[" + timestamp + "] [" + prefix + "] " + message;

  // Output to console
  std::cout << logEntry << std::endl << std::flush;

  // Output to file
  if (s_useLogFile) {
    std::ofstream file("d3d9to8.log", std::ios::app);
    if (file.is_open()) {
      file << logEntry << "\n";
      file.close();
    }
  }
}

void ThreadSafeLogger::debug(const std::string& message) {
  logWithLevel(LogLevel::LOG_DEBUG, "DEBUG", message);
}

void ThreadSafeLogger::info(const std::string& message) {
  logWithLevel(LogLevel::LOG_INFO, "INFO", message);
}

void ThreadSafeLogger::warn(const std::string& message) {
  logWithLevel(LogLevel::LOG_WARN, "WARN", message);
}

void ThreadSafeLogger::warn(REFIID riid) {
  wchar_t guidString[39];
  StringFromGUID2(riid, guidString, 39);
  char buffer[39];
  wcstombs(buffer, guidString, 39);
  ThreadSafeLogger::warn(std::string(buffer));
}

void ThreadSafeLogger::err(const std::string& message) {
  logWithLevel(LogLevel::LOG_ERROR, "ERROR", message);
}
