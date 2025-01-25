#include "../includes/sources/button_function.h"

int main(int argc, char *argv[]) {

#ifdef _WIN32
  HWND hwnd = GetConsoleWindow();
  ShowWindow
#endif
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
    bot.getApi().sendMessage(message->chat->id, "__We have on the menu__:", 0,
                             0, keyboard, "MarkdownV2");
  });

  bot.getEvents().onNonCommandMessage([](TgBot::Message::Ptr message) {
    download_mp3(std::move(message->text));
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
    printf("Long poll started\n");
    while (true) {
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
