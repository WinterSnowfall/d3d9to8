#include "d3d9_logger.h"

ThreadSafeLogger::LogLevel ThreadSafeLogger::s_level = ThreadSafeLogger::LogLevel(D3D9TO8_LOG_LEVEL);
bool ThreadSafeLogger::s_writeToLogFile = D3D9TO8_WRITE_TO_LOG_FILE;
bool ThreadSafeLogger::s_fileInitialized = false;
std::mutex ThreadSafeLogger::s_logMutex;

static const std::string D3D9TO8_LOG_FILE_PATH = "d3d9to8.log";

inline std::string ThreadSafeLogger::getTimestamp() {
  time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::ostringstream oss;
  oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

void ThreadSafeLogger::initializeFile() {
  if (s_writeToLogFile && !s_fileInitialized) {
    std::lock_guard<std::mutex> lock(s_logMutex);

    std::ifstream logFileCheck(D3D9TO8_LOG_FILE_PATH);
    bool logFileExists = logFileCheck.good();
    logFileCheck.close();

    if (logFileExists) {
      std::ofstream file(D3D9TO8_LOG_FILE_PATH, std::ios::trunc);
      file.close();
      s_fileInitialized = true;
    }
  }
}

void ThreadSafeLogger::logWithLevel(LogLevel logLevel, const std::string& prefix, const std::string& message) {
  if (logLevel < s_level)
    return;

  std::lock_guard<std::mutex> lock(s_logMutex);
  std::string logEntry = "[" + getTimestamp() + "] [" + prefix + "] " + message;
  std::cout << logEntry << std::endl << std::flush;

  if (s_writeToLogFile) {
    std::ofstream file(D3D9TO8_LOG_FILE_PATH, std::ios::app);
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
