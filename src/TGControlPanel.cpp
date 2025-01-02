#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/container_hash/hash_fwd.hpp>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <functional>
#include <string>
#include <sys/socket.h>
#include <tgbot/tgbot.h>
#include <unordered_map>
#include <utility>

using namespace std;
using namespace TgBot;
namespace beast = boost::beast; // Boost.Beast namespace
namespace http = beast::http;   // HTTP namespace
namespace net = boost::asio;    // Boost.Asio namespace
namespace ssl = net::ssl;       // SSL namespace
using tcp = net::ip::tcp;       // TCP namespace

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

    std::ofstream output_file("./downloaded.mp3", std::ios::binary);
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
  Mix_Music *music = Mix_LoadMUS("./downloaded.mp3");
  if (music == nullptr)
    cerr << "Failed to load MP3 file! SDL_mixer Error: " << Mix_GetError();

  Mix_PlayMusic(music, 1);
  while (Mix_PlayingMusic()) {
    SDL_Delay(100);
  }
  Mix_FreeMusic(music);

  bot.getApi().sendMessage(query->message->chat->id, "`Mischief manage`",
                           nullptr, nullptr, nullptr, "MarkdownV2");
}

int main(int argc, char *argv[]) {

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError();
    return 1;
  }
  if (Mix_Init(MIX_INIT_MP3) == 0) {
    cerr << "SDL_mixer could not initialize! SDL_mixer Error: "
         << Mix_GetError();
    SDL_Quit();
    return 1;
  }
  if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
    cerr << "SDL_mixer could not open audio! SDL_mixer Error: "
         << Mix_GetError();
    Mix_Quit();
    SDL_Quit();
    return 1;
  }

  static Bot bot(std::move("7768261581:AAGIOc_LhHV_yap2ytJiq45QwJRVCPyDTf8"));
  static unordered_map<string, function<void(Bot &, CallbackQuery::Ptr &)>>
      callbackData_funck{
          {"shutdownButton", shutdown_laptop},
          {"soundButton", play_sound},
          {"uploadButton", upload_sound},
      };
  static InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
  InlineKeyboardButton::Ptr shutdownButton(new InlineKeyboardButton);
  InlineKeyboardButton::Ptr soundButton(new InlineKeyboardButton);
  InlineKeyboardButton::Ptr uploadButton(new InlineKeyboardButton);

  vector<InlineKeyboardButton::Ptr> row0{shutdownButton};
  vector<InlineKeyboardButton::Ptr> row1{soundButton};
  vector<InlineKeyboardButton::Ptr> row2{uploadButton};

  shutdownButton->callbackData = "shutdownButton";
  soundButton->callbackData = "soundButton";
  uploadButton->callbackData = "uploadButton";

  shutdownButton->text = "Shutdown laptop";
  soundButton->text = "Play sound";
  uploadButton->text = "Upload sound";

  keyboard->inlineKeyboard = {row0, row1, row2};

  bot.getEvents().onCommand("start", [](Message::Ptr message) {
    std::string spoilerText = "_Hi_ ||*" + message->from->firstName + "*||";
    bot.getApi().sendMessage(message->chat->id, spoilerText, 0, 0, nullptr,
                             "MarkdownV2");
    bot.getApi().sendMessage(message->chat->id,
                             "__We have on the menu__:", nullptr, nullptr,
                             keyboard, "MarkdownV2");
  });

  bot.getEvents().onNonCommandMessage([](TgBot::Message::Ptr message) {
    download_mp3(std::move(message->text));
    /*std::string systemcall =*/
    /*    "curl --output /home/name_chell/pult/tmp.mp3 " + message->text;*/
  });

  bot.getEvents().onCallbackQuery([](CallbackQuery::Ptr query) {
    callbackData_funck[query->data](bot, query);
  });

  signal(SIGINT, [](int s) {
    printf("SIGINT got\n");
    exit(0);
  });
  try {
    bot.getApi().deleteWebhook();

    TgLongPoll longPoll(bot);
    while (true) {
      printf("Long poll started\n");
      longPoll.start();
    }
  } catch (exception &e) {
    printf("error: %s\n", e.what());
  }

  Mix_CloseAudio();
  Mix_Quit();
  SDL_Quit();
  return 0;
}
