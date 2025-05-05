#pragma once
#include <tgbot/tgbot.h>

extern TgBot::ReplyKeyboardMarkup::Ptr keyboardMain;
extern TgBot::ReplyKeyboardMarkup::Ptr keyboardWaiting;

void initKeyboards();
