#include "../include/button_function.h"
#include <cstdio>
#include <iostream>

void parse_url(string &&url, string &domain, string &path) {
  const string protocol_delimiter = "://";
  const string path_delimiter = "/";

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

// Функция для загрузки HTML-страницы
void download_mp3(std::string &&url) {
  try {

    string host = "";
    string path = "";
    const string port = "443";
    parse_url(std::move(url), host, path);

    net::io_context ioc;
    ssl::context ctx(ssl::context::tlsv12_client);
    ctx.set_default_verify_paths();

    tcp::resolver resolver(ioc);
    beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

    if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
      throw beast::system_error(beast::error_code(
          static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()));
    }

    auto const results = resolver.resolve(host, port);
    beast::get_lowest_layer(stream).connect(results);
    stream.handshake(ssl::stream_base::client);

    http::request<http::empty_body> req{http::verb::get, path, 11};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::dynamic_body> res;
    http::read(stream, buffer, res);

    std::ofstream output_file("../sound/tmp.mp3", std::ios::binary);
    output_file << beast::buffers_to_string(res.body().data());
    output_file.close();

    std::cout << "MP3 файл успешно загружен: downloaded.mp3" << std::endl;

    // Закрытие соединения
    beast::error_code ec;
    stream.shutdown(ec);
    if (ec == net::error::eof) {
      ec = {}; // EOF - это нормально
    }
    if (ec) {
      throw beast::system_error{ec};
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

void shutdown_laptop(Bot &bot, CallbackQuery::Ptr &query) {

  string response = "Введите пароль";
  bot.getApi().sendMessage(query->message->chat->id, response, nullptr, nullptr,
                           nullptr, "MarkdownV2");
  system("shutdown -h now");
}

void upload_sound(Bot &bot, CallbackQuery::Ptr &query) {

  bot.getApi().sendMessage(query->message->chat->id, "I'm waiting for the link",
                           nullptr, nullptr, nullptr, "MarkdownV2");
}

void play_sound(Bot &bot, CallbackQuery::Ptr &query) {
  Mix_Music *music =
      Mix_LoadMUS("/home/name_chell/TGControlPanel/sound/tmp.mp3");
  if (music == nullptr) {
    cerr << "Failed to load MP3 file! SDL_mixer Error: " << Mix_GetError();

    bot.getApi().sendMessage(query->message->chat->id, "couldn't do it",
                             nullptr, nullptr, nullptr, "MarkdownV2");
    return;
  }
  Mix_PlayMusic(music, 1);
  while (Mix_PlayingMusic()) {
    SDL_Delay(100);
  }
  Mix_FreeMusic(music);
  cout << "Play sound:" << query->from->username << '\n';
  bot.getApi().sendMessage(query->message->chat->id, "`Mischief manage`",
                           nullptr, nullptr, nullptr, "MarkdownV2");
}
