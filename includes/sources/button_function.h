#pragma once

#include "multi_media.h"

namespace buttonTG {

using std::string;
using TgBot::Bot;
using TgBot::CallbackQuery;

void shutdownLaptopButton(Bot &bot, CallbackQuery::Ptr &query);

void uploadSoundButton(Bot &bot, CallbackQuery::Ptr &query);

void playSoundButton(Bot &bot, CallbackQuery::Ptr &query);

void playVideoButton(Bot &bot, CallbackQuery::Ptr &query);

} // namespace buttonTG
