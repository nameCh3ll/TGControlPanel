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

#define DEFINE_PREFIX(EnumValue, str)              \
  template <>                                      \
  struct prefix_of<EnumValue> {                    \
    static constexpr std::string_view value = str; \
  };


template <auto>
struct prefix_of;

template <typename T>
constexpr bool always_false = false;

template <auto prefix>
static constexpr auto getPrefix_v = prefix_of<prefix>::value;

enum class TypeFile { MP3, MP4 };
DEFINE_PREFIX(TypeFile::MP3, "mp3");
DEFINE_PREFIX(TypeFile::MP4, "mp4");

enum class LogLevel { Info = 0, Warning = 1, Error = 2, Unspecified = 3 };
DEFINE_PREFIX(LogLevel::Info, COLOR_TEXT(BLUE, "(II)"));
DEFINE_PREFIX(LogLevel::Warning, COLOR_TEXT(YELLOW, "(WW)"));
DEFINE_PREFIX(LogLevel::Error, COLOR_TEXT(RED, "(EE)"));
DEFINE_PREFIX(LogLevel::Unspecified, COLOR_TEXT(BLACK, "(UU)"));

consteval std::ostream &getThred(LogLevel logLevel) {
  if (static_cast<int>(logLevel) > static_cast<int>(LogLevel::Warning)) {
    return std::ref(std::cerr);
  }
  return std::ref(std::cout);
}

template <LogLevel logLevel, typename... Args>
inline constexpr void logMessage(std::format_string<Args...> fmt,
                                 Args &&...args) {
  std::print(getThred(logLevel), "{} {}\n", getPrefix_v<logLevel>,
             std::format(fmt, std::forward<Args>(args)...));
}

inline bool shutdownSystem() {
#ifdef _WIN32
  return ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
#elif __linux__
  return system("shutdown -h now");
#endif
}

inline std::string getApiToken(int argc, char *argv[]) {
  for (size_t i = 1; i < argc; ++i) {
    if (std::string_view(argv[i]) == "--api-token" && i + 1 < argc &&
        !std::string_view(argv[i + 1]).empty()) {
      logMessage<LogLevel::Info>("{}", "Using key from --api-token");
      return argv[i + 1];
    }
  }

  if (std::string(COMPILED_TOKEN) != "") {
    logMessage<LogLevel::Info>("{}", "Using key from build");
    return COMPILED_TOKEN;
  }

  logMessage<LogLevel::Error>("{}", "Key not specified");
  return "";
}

inline void signalHandler(int signal) {
  static size_t count_signal = 0;
  if (signal == SIGINT) {
    ++count_signal;
    if (count_signal == 1) {
      logMessage<LogLevel::Info>("{}", "Press ctrl+c again to exit");
    } else if (count_signal >= 2) {
      logMessage<LogLevel::Info>("{}", "Exiting...");
      std::exit(0);
    }
  }
}

inline void clearPendingUpdates(TgBot::Bot &bot) {
  int64_t lastUpdateId = 0;
  std::vector<TgBot::Update::Ptr> updates;

  do {
    auto updates = bot.getApi().getUpdates(lastUpdateId + 1);
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

inline bool isTokenValid(const std::string telegramApiToken) {
  try {
    TgBot::Bot bot(telegramApiToken);
    const std::string username_bot = bot.getApi().getMe()->username;
    logMessage<LogLevel::Info>("{}", "Telegram API token corrected");
    logMessage<LogLevel::Info>("{}: @{}", "Username", username_bot);
    return true;
  } catch (const TgBot::TgException &e) {
    logMessage<LogLevel::Error>("{}: {}", "Invalid token or connection error",
                                e.what());
    return false;
  }
}
template <TypeFile prefix>
inline bool isLink(const std::string &link) {
  std::string pattern =
      std::string(R"(https?:\/\/[^\s]+\.)") + std::string(getPrefix_v<prefix>);
  std::regex link_regex(pattern);
  return std::regex_search(link, link_regex);
}
