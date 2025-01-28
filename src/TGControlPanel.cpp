#include "button_function.h"
#include <functional>

using namespace TgBot;
using namespace std;

int main(int argc, char *argv[]) {

  signal(SIGINT, signalHandler);

#ifdef _WIN32
  HWND hwnd = GetConsoleWindow();
  ShowWindow(hwnd, SW_HIDE);
#endif

  cout << "Markers: (II) informational, (WW) warning, (EE) error, (NI) not "
          "implemented\n\n";

  if (argc < 2) {
    cerr << "(EE) Enter your Telegram API token\n";
    return 1;
  } else if (argc > 2) {
    cerr << "(EE) Number of arguments is exceeded\n";
    return 1;
  } else if (argc == 2) {
    isTokenValid(argv[1]);
  }

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    cerr << "SDL could not initialize!\nSDL_Error: " << SDL_GetError();
    return 1;
  }
  if (Mix_Init(MIX_INIT_MP3) == 0) {
    cerr << "SDL_mixer could not initialize!\nSDL_mixer Error: "
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
  const string TELEGRAM_API_TOKEN =
      "7768261581:AAEOBImqZAUC9gNSucUVNYVy6sSCjYHh_vY";
  static Bot bot(TELEGRAM_API_TOKEN);
  static unordered_map<string, function<void(Bot &, CallbackQuery::Ptr &)>>
      callbackData_funck{
          {"shutdownButton", shutdownLaptop},
          {"soundButton", playSound},
          {"uploadButton", uploadSound},
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
    downloadMp3(std::move(message->text));
  });

  bot.getEvents().onCallbackQuery([](CallbackQuery::Ptr query) {
    callbackData_funck[query->data](bot, query);
  });

  try {
    bot.getApi().deleteWebhook();

    TgLongPoll longPoll(bot);
    cout << "--- Bot started ---\n";
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
