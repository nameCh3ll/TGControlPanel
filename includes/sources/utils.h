#pragma once
#include "pch.h"

inline void signalHandler(int signal) {
  static size_t count_signal = 0;
  ++count_signal;
  if (signal == SIGINT && count_signal >= 2) {
    std::cout << "\n(II) SIGINT caught, exiting...\n";
    std::exit(0);
  }
}

inline void parseUrl(std::string &&url, std::string &domain, std::string &path) {
  const std::string protocol_delimiter = "://";
  const std::string path_delimiter = "/";

  // Найти начало домена после протокола
  auto protocol_end = url.find(protocol_delimiter);
  if (protocol_end == std::string::npos) {
    throw std::invalid_argument("URL не содержит протокол.");
  }
  protocol_end += protocol_delimiter.length();

  // Найти начало пути
  auto path_start = url.find(path_delimiter, protocol_end);
  if (path_start == std::string::npos) {
    // Если путь отсутствует, берем только домен
    domain = url.substr(protocol_end);
    path = "/";
  } else {
    // Разделить домен и путь
    domain = url.substr(protocol_end, path_start - protocol_end);
    path = url.substr(path_start);
  }
}

inline bool isTokenValid(const char *telegram_api_token) {
  try {
    TgBot::Bot bot(telegram_api_token);
    const std::string username_bot = bot.getApi().getMe()->username;

    std::cout << "(II) Telegram API token corrected\n";
    std::cout << "(II) Username: @" << username_bot << std::endl;
    return true;
  } catch (const TgBot::TgException &e) {
    std::cerr << "(EE) Invalid token or connection error: " << e.what() << std::endl;
    return false;
  }
}
