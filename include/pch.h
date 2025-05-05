#pragma once

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#define X11_NO_MANS_DEFINES
#include <SDL2/SDL_syswm.h>
#undef BadRequest
#include <tgbot/tgbot.h>
#include <vlc/vlc.h>

#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/container_hash/hash_fwd.hpp>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <deque>
#include <functional>
#include <print>
#include <regex>
#include <string>
#include <thread>
#include <unordered_map>
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <cstdlib>
#endif
