#pragma once

#include "utils.h"

using TgBot::Bot;
using TgBot::CallbackQuery;
using std::string;

void downloadMp3(std::string &&url);

void shutdownLaptop(Bot &bot, CallbackQuery::Ptr &query);

void uploadSound(Bot &bot, CallbackQuery::Ptr &query);

void playSound(Bot &bot, CallbackQuery::Ptr &query);
