#include "cb/memory.hpp"
#include "cb/iostream.hpp"
#include "core/router_thread.hpp"
#include "core/bot_thread.hpp"
#include "core/event_thread.hpp"
#include "gui/gui.hpp"

using namespace cb;

EventThread* mpEvents = nullptr;
BotThreadPtr mpQuotes;
bool mShuttingDown = false;
unique_ptr<Fl_Double_Window> mpGui;

/******************************************************************************/
/** GUI Callbacks                                                            **/
/******************************************************************************/
void cb::gui_update_callback(void*) {
  if (mShuttingDown) return;
  auto tran = mpEvents->get();
  auto cnt = 0;
  while (tran){
    ++cnt;
    switch (tran->mRsltType) {
      case InRslt::TickPrice: {
        auto ptr = std::static_pointer_cast<TickPriceRslt>(tran);
        switch (ptr->mFieldType) {
          case 1:
            gTableQuote->updateBidPrice(ptr->mSymbol, ptr->mValue);
            break;
          case 2:
            gTableQuote->updateAskPrice(ptr->mSymbol, ptr->mValue);
            break;
          case 4:
            gTableQuote->updateLastPrice(ptr->mSymbol, ptr->mValue);
            break;
          default:
            break;
        }
      }
      case InRslt::TickSize: {
        auto ptr = std::static_pointer_cast<TickSizeRslt>(tran);
        switch (ptr->mFieldType) {
          case 0:
            gTableQuote->updateBidSize(ptr->mSymbol, ptr->mSize);
            break;
          case 3:
            gTableQuote->updateAskSize(ptr->mSymbol, ptr->mSize);
            break;
          case 5:
            gTableQuote->updateLastSize(ptr->mSymbol, ptr->mSize);
            break;
          case 8:
            gTableQuote->updateVolume(ptr->mSymbol, ptr->mSize);
            break;
          default:
            break;
        }
      }
      default:
        break;
    }
    tran = mpEvents->get();
  }
  if (cnt)
    gTableQuote->redraw();
  Fl::repeat_timeout(0.1f, gui_update_callback);
}

void cb::input_symbol_entered(Fl_Input* o) {
  mpQuotes->requestTicker(o->value());
  gTableQuote->addSymbol(o->value());
  gTableQuote->redraw();
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

