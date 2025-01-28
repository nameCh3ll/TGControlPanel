#include "button_function.h"

namespace beast = boost::beast; // Boost.Beast namespace
namespace http = beast::http;   // HTTP namespace
namespace net = boost::asio;    // Boost.Asio namespace
namespace ssl = net::ssl;       // SSL namespace
using tcp = net::ip::tcp;       // TCP namespace

void downloadMp3(std::string &&url) {
  try {
    string host = "";
    string path = "";
    const string port = "443";
    parseUrl(std::move(url), host, path);

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

    if (res.result() != http::status::ok) {
      throw std::runtime_error("HTTP Error: " +
                               std::to_string(res.result_int()));
    }

    std::ofstream output_file("./tmp.mp3", std::ios::binary);
    output_file << boost::beast::buffers_to_string(res.body().data());
    output_file.close();

    std::cout << "MP3 файл успешно загружен: tmp.mp3" << std::endl;

    beast::error_code ec;
    stream.shutdown(ec);
    if (ec == net::error::eof || ec == ssl::error::stream_truncated) {
      ec = {}; // EOF - это нормально
    }
    if (ec) {
      throw beast::system_error{ec};
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

void shutdownLaptop(Bot &bot, CallbackQuery::Ptr &query) {

  system("shutdown -h now");
}

void uploadSound(Bot &bot, CallbackQuery::Ptr &query) {
  // TODO:async
  bot.getApi().sendMessage(query->message->chat->id, "I'm waiting for the link",
                           0, 0, 0, "MarkdownV2");
}

void playSound(Bot &bot, CallbackQuery::Ptr &query) {
  Mix_Music *music = Mix_LoadMUS("./tmp.mp3");
  if (music == nullptr) {
    std::cerr << "Failed to load MP3 file! SDL_mixer Error: " << Mix_GetError();

    bot.getApi().sendMessage(query->message->chat->id, "couldn't do it", 0, 0,
                             0, "MarkdownV2");
    return;
  }
  Mix_PlayMusic(music, 1);

  while (Mix_PlayingMusic()) {
    SDL_Delay(100);
  }

  Mix_FreeMusic(music);
  std::cout << "Play sound:" << query->from->username << '\n';
  bot.getApi().sendMessage(query->message->chat->id, "`Mischief manage`", 0, 0,
                           0, "MarkdownV2");
}
