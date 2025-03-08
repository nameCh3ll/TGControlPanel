#pragma once

#include "pch.h"

#ifdef TELEGRAM_API_TOKEN
#define COMPILED_TOKEN TELEGRAM_API_TOKEN
#else
#define COMPILED_TOKEN ""
#endif

#define RED "\033[31m"
#define BLUE "\033[34m"
#define BLACK "\033[30m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
#define COLOR_TEXT(color, text) color text RESET " "

inline std::string getApiToken(int argc, char *argv[]) {

  for (size_t i = 1; i < argc; ++i) {
    if (std::string_view(argv[i]) == "--api-token" && i + 1 < argc &&
        !std::string_view(argv[i + 1]).empty()) {
      return argv[i + 1];
    }
  }

  if (std::string(COMPILED_TOKEN) != "") {
    return COMPILED_TOKEN;
  }

  return "Token undefined";
}

enum class LogLevel { Info, Warning, Error, Unspecified };

constexpr std::array<std::string_view, 4> prefixes = {
    COLOR_TEXT(BLUE, "(II)"), COLOR_TEXT(YELLOW, "(WW)"),
    COLOR_TEXT(RED, "(EE)"), COLOR_TEXT(BLACK, "(UU)")};

consteval std::string_view get_prefix(LogLevel logLevel) {
  return prefixes[static_cast<int>(logLevel)];
}

template <LogLevel logLevel, typename... Args>
inline constexpr void logMessage(std::format_string<Args...> fmt,
                                 Args &&...args) {
  std::print("{}{}\n", get_prefix(logLevel),
             std::format(fmt, std::forward<Args>(args)...));
}

inline void signalHandler(int signal) {
  static size_t count_signal = 0;
  ++count_signal;
  if (signal == SIGINT && count_signal >= 2) {
    std::cout << "\n(II) SIGINT caught, exiting...\n";
    std::exit(0);
  }
}

inline void clearPendingUpdates(TgBot::Bot &bot) {
  int64_t lastUpdateId = 0;
  std::vector<TgBot::Update::Ptr> updates;

  do {
    updates = bot.getApi().getUpdates(lastUpdateId + 1);
    for (const auto &update : updates) {
      lastUpdateId = update->updateId;
    }
  } while (!updates.empty());
}

inline void parseUrl(std::string &&url, std::string &domain,
                     std::string &path) {
  const std::string protocol_delimiter = "://";
  const std::string path_delimiter = "/";

  auto protocol_end = url.find(protocol_delimiter);
  if (protocol_end == std::string::npos) {
    throw std::invalid_argument("URL не содержит протокол.");
  }
  protocol_end += protocol_delimiter.length();

  auto path_start = url.find(path_delimiter, protocol_end);
  if (path_start == std::string::npos) {
    domain = url.substr(protocol_end);
    path = "/";
  } else {
    domain = url.substr(protocol_end, path_start - protocol_end);
    path = url.substr(path_start);
  }
}

inline bool isTokenValid(const std::string telegram_api_token) {
  try {
    TgBot::Bot bot(telegram_api_token);
    const std::string username_bot = bot.getApi().getMe()->username;

    std::cout << "(II) Telegram API token corrected\n";
    std::cout << "(II) Username: @" << username_bot << std::endl;
    return true;
  } catch (const TgBot::TgException &e) {
    std::cerr << "(EE) Invalid token or connection error: " << e.what()
              << std::endl;
    return false;
  }
}
