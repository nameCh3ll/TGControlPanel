#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <boost/container_hash/hash_fwd.hpp>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <functional>
#include <string>
#include <sys/socket.h>
#include <tgbot/tgbot.h>
#include <unordered_map>
#include <utility>

using namespace std;
using namespace TgBot;

void shutdown_laptop(Bot &bot, CallbackQuery::Ptr &query) {

  string response = "Введите пароль";
  bot.getApi().sendMessage(query->message->chat->id, response, nullptr, nullptr,
                           nullptr, "MarkdownV2");
  system("shutdown -h now");
}

void upload_sound(Bot &bot, CallbackQuery::Ptr &query) {

  bot.getApi().sendMessage(query->message->chat->id, "I'm waiting for the link",
                           nullptr, nullptr, nullptr, "MarkdownV2");
}

void play_sound(Bot &bot, CallbackQuery::Ptr &query) {
  Mix_Music *music = Mix_LoadMUS("/home/name_chell/pult/tmp.mp3");
  if (music == nullptr)
    cerr << "Failed to load MP3 file! SDL_mixer Error: " << Mix_GetError();

  Mix_PlayMusic(music, 1);
  while (Mix_PlayingMusic()) {
    SDL_Delay(100);
  }
  Mix_FreeMusic(music);

  bot.getApi().sendMessage(query->message->chat->id, "`Mischief manage`",
                           nullptr, nullptr, nullptr, "MarkdownV2");
}

int main(int argc, char *argv[]) {

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError();
    return 1;
  }
  if (Mix_Init(MIX_INIT_MP3) == 0) {
    cerr << "SDL_mixer could not initialize! SDL_mixer Error: "
         << Mix_GetError();
    SDL_Quit();
    return 1;
  }
  if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
    cerr << "SDL_mixer could not open audio! SDL_mixer Error: "
         << Mix_GetError();
    Mix_Quit();
    SDL_Quit();
    return 1;
  }

  static Bot bot(std::move("7768261581:AAGIOc_LhHV_yap2ytJiq45QwJRVCPyDTf8"));
  static unordered_map<string, function<void(Bot &, CallbackQuery::Ptr &)>>
      callbackData_funck{
          {"shutdownButton", shutdown_laptop},
          {"soundButton", play_sound},
          {"uploadButton", upload_sound},
      };
  static InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
  InlineKeyboardButton::Ptr shutdownButton(new InlineKeyboardButton);
  InlineKeyboardButton::Ptr soundButton(new InlineKeyboardButton);
  InlineKeyboardButton::Ptr uploadButton(new InlineKeyboardButton);

  vector<InlineKeyboardButton::Ptr> row0{shutdownButton};
  vector<InlineKeyboardButton::Ptr> row1{soundButton};
  vector<InlineKeyboardButton::Ptr> row2{uploadButton};

  shutdownButton->callbackData = "shutdownButton";
  soundButton->callbackData = "soundButton";
  uploadButton->callbackData = "uploadButton";

  shutdownButton->text = "Shutdown laptop";
  soundButton->text = "Play sound";
  uploadButton->text = "Upload sound";

  keyboard->inlineKeyboard = {row0, row1, row2};

  bot.getEvents().onCommand("start", [](Message::Ptr message) {
    std::string spoilerText = "_Hi_ ||*" + message->from->firstName + "*||";
    bot.getApi().sendMessage(message->chat->id, spoilerText, 0, 0, nullptr,
                             "MarkdownV2");
    bot.getApi().sendMessage(message->chat->id,
                             "__We have on the menu__:", nullptr, nullptr,
                             keyboard, "MarkdownV2");
  });
  bot.getEvents().onNonCommandMessage([](TgBot::Message::Ptr message) {
    std::string systemcall = "curl --output /home/name_chell/pult/tmp.mp3 " + message->text;
    system(systemcall.c_str());
    bot.getApi().sendMessage(message->chat->id,
                             "Downloaded from: " + message->text);
  });
  bot.getEvents().onCallbackQuery([](CallbackQuery::Ptr query) {
    callbackData_funck[query->data](bot, query);
  });

  signal(SIGINT, [](int s) {
    printf("SIGINT got\n");
    exit(0);
  });
  try {
    bot.getApi().deleteWebhook();

    TgLongPoll longPoll(bot);
    while (true) {
      printf("Long poll started\n");
      longPoll.start();
    }
  } catch (exception &e) {
    printf("error: %s\n", e.what());
  }

  Mix_CloseAudio();
  Mix_Quit();
  SDL_Quit();
  return 0;
}
