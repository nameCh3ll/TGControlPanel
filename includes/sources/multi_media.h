#pragma once

#include "utils.h"

#define HISTORY_SIZE 10
#define MAX_SIZE_VIDO 1024 * 1024  // 1MB
#define MAX_SIZE_AUDIO 1024 * 1024 // 1MB

namespace multiMedia {

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

using std::cerr;
using std::cout;
using std::deque;
using std::string;

bool playSound();

bool playVideo();

bool pushBackInHistory();

bool downloadAudioInBuffer(string &&url);

bool downloadVideoInBuffer(string &&url);

} // namespace multiMedia
