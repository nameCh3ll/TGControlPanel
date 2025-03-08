#include "button_function.h"
#include <functional>
#include <unordered_map>

// УБРАТЬ ХЭШ МАПУ

using namespace std;
using namespace TgBot;
using namespace multiMedia;

int main(int argc, char *argv[]) {
  signal(SIGINT, signalHandler);

#ifdef _WIN32
  HWND hwnd = GetConsoleWindow();
  ShowWindow(hwnd, SW_HIDE);
#endif
  logMessage<LogLevel::Info>("{}\n\n",
                             "Markers: (II) informational, (WW) warning, (EE) "
                             "error, (UU) not implemented");

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    logMessage<LogLevel::Error>("{}\n{}: {}", "SDL could not initialize",
                                "SDL_Error", SDL_GetError());
    return 1;
  }
  if (Mix_Init(MIX_INIT_MP3) == 0) {
    logMessage<LogLevel::Error>("{}\n{}: {}", "SDL_mixer could not initialize",
                                "SDL_mixer Error", Mix_GetError());
    SDL_Quit();
    return 1;
  }
  if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
    logMessage<LogLevel::Error>("{}\n{}: {}", "SDL_mixer could not open audio",
                                "SDL_mixer Error", Mix_GetError());
    Mix_Quit();
    SDL_Quit();
    return 1;
  }

  const string API_TOKEN = std::move(getApiToken(argc, argv));
  if (!isTokenValid(API_TOKEN)) {
    return 0;
  }

  static Bot bot(API_TOKEN);
  clearPendingUpdates(bot);

  static unordered_map<std::string, function<void(Bot &, CallbackQuery::Ptr &)>>
      callbackData_funck{{"shutdownButton", buttonTG::shutdownLaptopButton},
                         {"playSoundButton", buttonTG::playSoundButton},
                         {"playVideoButton", buttonTG::playVideoButton},
                         {"uploadSoundButton", buttonTG::uploadSoundButton}};

  static InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);

  InlineKeyboardButton::Ptr shutdownButton(new InlineKeyboardButton);
  InlineKeyboardButton::Ptr playSoundButton(new InlineKeyboardButton);
  InlineKeyboardButton::Ptr playVideoButton(new InlineKeyboardButton);
  InlineKeyboardButton::Ptr uploadSoundButton(new InlineKeyboardButton);

  vector<InlineKeyboardButton::Ptr> row0{shutdownButton};
  vector<InlineKeyboardButton::Ptr> row1{playSoundButton};
  vector<InlineKeyboardButton::Ptr> row2{uploadSoundButton};
  vector<InlineKeyboardButton::Ptr> row3{playVideoButton};

  shutdownButton->callbackData = "shutdownButton";
  playSoundButton->callbackData = "playSoundButton";
  playVideoButton->callbackData = "playVideoButton";
  uploadSoundButton->callbackData = "uploadSoundButton";

  shutdownButton->text = "Shutdown laptop";
  playSoundButton->text = "Play sound";
  playVideoButton->text = "Play video";
  uploadSoundButton->text = "Upload sound";

  keyboard->inlineKeyboard = {row0, row1, row2, row3};

  bot.getEvents().onCommand("start", [](Message::Ptr message) {
    std::string spoilerText = "_Hi_ ||*" + message->from->firstName + "*||";
    bot.getApi().sendMessage(message->chat->id, spoilerText, 0, 0, nullptr,
                             "MarkdownV2");
    bot.getApi().sendMessage(message->chat->id, "__We have on the menu__:", 0,
                             0, keyboard, "MarkdownV2");
  });

  bot.getEvents().onNonCommandMessage([](TgBot::Message::Ptr message) {
    if (downloadAudioInBuffer(std::move(message->text))) {
      std::string answerMessage = "Sound is loaded";
      bot.getApi().sendMessage(message->chat->id, answerMessage, 0, 0, nullptr,
                               "MarkdownV2");
    }
  });

  bot.getEvents().onCallbackQuery([](CallbackQuery::Ptr query) {
    callbackData_funck[query->data](bot, query);
  });

  try {
    bot.getApi().deleteWebhook();

    TgLongPoll longPoll(bot);
    logMessage<LogLevel::Info>("{}\n", "--- Bot started ---");
    while (true) {
      longPoll.start();
    }
  } catch (exception &e) {
    logMessage<LogLevel::Info>("{}: {}\n", "error", e.what());
  }

  Mix_CloseAudio();
  Mix_Quit();
  SDL_Quit();
  return 0;
}
