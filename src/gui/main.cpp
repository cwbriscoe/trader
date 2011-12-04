#include "core/router_thread.hpp"
#include "core/bot_thread.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

using namespace cb;

int main(int argc, char **argv) {
  RouterThread router;
  router.addBot(new BotThread((Provider*)&router));
  router.start();

  Fl_Window *window = new Fl_Window(340,180);
    Fl_Box *box = new Fl_Box(20,40,300,100,"Hello, World!");
      box->box(FL_UP_BOX);
      box->labelfont(FL_BOLD+FL_ITALIC);
      box->labelsize(36);
      box->labeltype(FL_SHADOW_LABEL);
  window->end();

  window->show(argc, argv);

  auto ret = Fl::run();

  window->hide();

  //shutdown worker threads cleanly before destructors are called.
  router.shutdown();
  
  return ret;
}

