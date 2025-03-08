#include "multi_media.h"

namespace multiMedia {

struct audioBuffer {
  char data[MAX_SIZE_AUDIO];
  size_t size;
} static audioBuff;

struct videoBuffer {
  char data[MAX_SIZE_VIDO];
  size_t size;
} static videoBuff;

static deque<audioBuffer> audioHistoryStorage;
static deque<videoBuffer> videoHistoryStorage;

bool playSound() {

  if (audioBuff.size == 0) {
    logMessage<LogLevel::Error>("{}\n", "Audio buffer is empty");
    return false;
  }

  SDL_RWops *pRWF = SDL_RWFromMem(audioBuff.data, audioBuff.size);
  if (!pRWF) {
    logMessage<LogLevel::Error>("{}\n", "Failed to create SDL_RWops");
    return false;
  }

  Mix_Music *music = Mix_LoadMUS_RW(pRWF, 1);
  if (!music) {
    logMessage<LogLevel::Error>("{}\n{}: {}\n", "Failed to download MP3",
                                "SDL_mixer", Mix_GetError());
    return false;
  }

  if (Mix_PlayMusic(music, 1) == -1) {
    logMessage<LogLevel::Error>("{}\n{}: {}\n", "Failed to play music",
                                "SDL_mixer", Mix_GetError());
    return false;
  }

  while (Mix_PlayingMusic()) {
    SDL_Delay(100);
  }

  return true;
}

bool playVideo() {

  const char *videoPath = "../tst/audio.mp4";

  libvlc_instance_t *vlcInstance = libvlc_new(0, nullptr);
  libvlc_media_t *media = libvlc_media_new_path(vlcInstance, videoPath);

  libvlc_media_parse_with_options(media, libvlc_media_parse_local, -1);
  while (libvlc_media_get_parsed_status(media) !=
         libvlc_media_parsed_status_done) {
  }

  unsigned int videoWidth = 0, videoHeight = 0;
  libvlc_media_track_t **tracks;
  int trackCount = libvlc_media_tracks_get(media, &tracks);

  if (trackCount > 0) {
    for (int i = 0; i < trackCount; ++i) {
      if (tracks[i]->i_type == libvlc_track_video) {
        videoWidth = tracks[i]->video->i_width;
        videoHeight = tracks[i]->video->i_height;
        break;
      }
    }
    libvlc_media_tracks_release(tracks, trackCount);
  }

  if (videoWidth == 0 || videoHeight == 0) {
    logMessage<LogLevel::Error>("{}\n", "Failed to get the video size");
    videoWidth = 800;
    videoHeight = 600;
  }

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    logMessage<LogLevel::Error>("{}\n{}: {}\n", "SDL could not initialize",
                                "SDL_Error", SDL_GetError());
    return false;
  }

  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode(0, &displayMode);
  int screenWidth = displayMode.w;
  int screenHeight = displayMode.h;

  std::srand(std::time(nullptr));
  float scaleX = (float)screenWidth / videoWidth;
  float scaleY = (float)screenHeight / videoHeight;
  float scale = std::min(scaleX, scaleY);

  if (scale < 1.0f) {
    videoWidth = (int)(videoWidth * scale);
    videoHeight = (int)(videoHeight * scale);
  }
  int randomX = std::rand() % std::max(1u, screenWidth - videoWidth);
  int randomY = std::rand() % std::max(1u, screenHeight - videoHeight);
  SDL_Window *window =
      SDL_CreateWindow("MP4 Player", randomX, randomY, videoWidth, videoHeight,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP);

  if (!window) {
    logMessage<LogLevel::Error>("{}\n{}: {}\n", "Window creation error",
                                "SDL_Error", SDL_GetError());
    SDL_Quit();
    return false;
  }

  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(window, &wmInfo);
  void *windowHandle = nullptr;

#if defined(__linux__)
  windowHandle = (void *)(uintptr_t)wmInfo.info.x11.window;
#endif

  libvlc_media_player_t *player = libvlc_media_player_new_from_media(media);
  libvlc_media_player_set_xwindow(player, (uint32_t)(uintptr_t)windowHandle);

  libvlc_media_player_play(player);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  SDL_Event event;
  bool running = true;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }

    if (libvlc_media_player_is_playing(player) == 0) {
      running = false;
    }

    SDL_Delay(10);
  }

  libvlc_media_player_stop(player);
  libvlc_media_player_release(player);
  libvlc_media_release(media);
  libvlc_release(vlcInstance);
  SDL_DestroyWindow(window);
  SDL_Quit();
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

    std::memcpy(audioBuff.data,
                boost::beast::buffers_front(res.body().data()).data(),
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
