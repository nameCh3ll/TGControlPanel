#pragma once
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
/*#include <sys/socket.h>*/
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

void parse_url(string &&url, string &domain, string &path);

void download_mp3(std::string &&url);

void shutdown_laptop(Bot &bot, CallbackQuery::Ptr &query);

void upload_sound(Bot &bot, CallbackQuery::Ptr &query);

void play_sound(Bot &bot, CallbackQuery::Ptr &query);
