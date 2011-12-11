#include "cb/memory.hpp"
#include "core/router_thread.hpp"
#include "core/bot_thread.hpp"
#include "core/event_thread.hpp"
#include "gui/gui.hpp"

using namespace cb;

using std::cout;
using std::endl;

EventThread* mpEvents = nullptr;
BotThreadPtr mpQuotes;
bool mShuttingDown = false;
unique_ptr<Fl_Double_Window> mpGui;

/******************************************************************************/
/** GUI Callbacks                                                            **/
/******************************************************************************/
void gui_update_callback(void*) {
  if (mShuttingDown) return;
  auto tran = mpEvents->get();
  while (tran){
    switch (tran->mRsltType) {
      case InRslt::TickPrice: {
        auto ptr = std::static_pointer_cast<TickPriceRslt>(tran);
        if (ptr->mTickerId == 1 && ptr->mFieldType == 4) {
          stringstream ss;
          ss << ptr->mValue;
          //box->label(ss.str().c_str());
        }
        break;
      }
      default:
        break;
    }
    tran = mpEvents->get();
  }
  Fl::repeat_timeout(0.1f, gui_update_callback);
}

void input_symbol_entered(Fl_Input* o) {
  mpQuotes->requestTicker(o->value());
  o->value("");
}

/******************************************************************************/
/** Main Entrypoint                                                          **/
/******************************************************************************/
int main(int argc, char **argv) {
  RouterThread router;
  router.start();
  mpEvents = router.getEventThread();

  mpQuotes = BotThreadPtr(new BotThread());
  router.addBot(mpQuotes);

  Fl::scheme("plastic"); 
  mpGui = unique_ptr<Fl_Double_Window>(make_window());
  mpGui->show(argc, argv);

  Fl::add_timeout(1.0f, gui_update_callback);
  auto ret = Fl::run();
  Fl::remove_timeout(gui_update_callback);

  mpGui->hide();

  //shutdown worker threads cleanly before destructors are called.
  mShuttingDown = true;
  router.shutdown();
  
  return ret;
}

