#pragma once

#include "keyboards.h"
#include "multi_media.h"

namespace {

using namespace multiMedia;

using std::function;
using std::string;
using std::unordered_map;

using TgBot::Bot;
using TgBot::Message;

using UserID = int64_t;
using UserHandler = std::function<void(Bot &, Message::Ptr)>;

/*extern DynamicHandler currentMessageHandler;*/
/*extern const DynamicHandler startHandler;*/
/*extern const DynamicHandler mainHandler;*/
/*extern UserHandler wraperNonComandMessage;*/

static unordered_map<UserID, UserHandler> userStates;

// static unordered_map<string, function<void(Bot &, Message::Ptr)>>
//     buttonCallbacks;

void shutdownLaptopButton(Bot &bot, Message::Ptr message);

void uploadSoundButton(Bot &bot, Message::Ptr message);

void uploadVideoButton(Bot &bot, Message::Ptr message);

void playSoundButton(Bot &bot, Message::Ptr message);

void playVideoButton(Bot &bot, Message::Ptr message);

}  // namespace buttonTG
