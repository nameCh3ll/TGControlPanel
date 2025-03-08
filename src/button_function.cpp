#include "button_function.h"

namespace buttonTG {

using namespace multiMedia;

void shutdownLaptopButton(Bot &bot, CallbackQuery::Ptr &query) {

  system("shutdown -h now");
}

void uploadSoundButton(Bot &bot, CallbackQuery::Ptr &query) {
  // TODO:async
  bot.getApi().sendMessage(query->message->chat->id, "I'm waiting for the link",
                           0, 0, 0, "MarkdownV2");
}

void playSoundButton(Bot &bot, CallbackQuery::Ptr &query) {
  if (playSound()) {
    std::cerr << "Play sound:" << query->from->username << '\n';
    bot.getApi().sendMessage(query->message->chat->id, "`Mischief manage`", 0,
                             0, 0, "MarkdownV2");
  } else {
    std::cerr << "error";
  }
}

void playVideoButton(Bot &bot, CallbackQuery::Ptr &query) {
  if (playVideo()) {
    std::cerr << "Play sound:" << query->from->username << '\n';
    bot.getApi().sendMessage(query->message->chat->id, "`Mischief manage`", 0,
                             0, 0, "MarkdownV2");
  } else {
    std::cerr << "error";
  }
}

} // namespace buttonTG
