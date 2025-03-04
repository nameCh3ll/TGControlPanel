#include "multi_media.h"

namespace multiMedia {

struct audioBuffer {
  char data[MAX_SIZE_AUDIO];
  size_t size;
} static audioBuff;

struct videoBuffer {
  char data[MAX_SIZE_AUDIO];
  size_t size;
} static videoBuff;

static deque<audioBuffer> audioHistoryStorage;
static deque<videoBuffer> videoHistoryStorage;

bool playSound() {

  if (audioBuff.size == 0) {
    cerr << "Ошибка: аудиобуфер пуст!\n";
    return false;
  }

  SDL_RWops *pRWF = SDL_RWFromMem(audioBuff.data, audioBuff.size);
  if (!pRWF) {
    cerr << "Ошибка: не удалось создать SDL_RWops!\n";
    return false;
  }

  Mix_Music *music = Mix_LoadMUS_RW(pRWF, 1);
  if (!music) {
    cerr << "Ошибка: не удалось загрузить MP3! SDL_mixer: "
              << Mix_GetError() << "\n";
    return false;
  }

  if (Mix_PlayMusic(music, 1) == -1) {
    cerr << "()Error: failed to play music! SDL_mixer: " << Mix_GetError()
              << "\n";
    return false;
  }

  while (Mix_PlayingMusic()) {
    SDL_Delay(100);
  }

  return true;
}

bool downloadAudioInBuffer(string &&url) {

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

    std::memcpy(audioBuff.data, boost::beast::buffers_front(res.body().data()).data(),
                res.body().size());
    audioBuff.size = res.body().size();

    beast::error_code ec;
    [[maybe_unused]] auto _ = stream.shutdown(ec);
    if (ec == net::error::eof || ec == ssl::error::stream_truncated) {
      ec = {}; 
    }
    if (ec) {
      throw beast::system_error{ec};
    }
    return true;
  } catch (const std::exception &e) {
    cerr << "Error: " << e.what() << '\n';
    return false;
  }
}

bool downloadVideoInBuffer(string &&url) { return false; }

bool uploadSoundInBuffer() { return false; }

bool pushBackInHistory() { return false; }

} // namespace multiMedia
