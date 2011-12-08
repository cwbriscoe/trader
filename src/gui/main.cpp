#include "core/router_thread.hpp"
#include "core/event_thread.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

using namespace cb;

using std::cout;
using std::endl;

EventThread* mpEvents = nullptr;
bool mShuttingDown = false;
Fl_Box* box = nullptr;

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
          box->label(ss.str().c_str());
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

/******************************************************************************/
/** Main Entrypoint                                                          **/
/******************************************************************************/
int main(int argc, char **argv) {
  RouterThread router;
  router.start();
  mpEvents = router.getEventThread();

  Fl_Window *window = new Fl_Window(340,180);
    box = new Fl_Box(20,40,300,100,"Hello, World!");
      box->box(FL_UP_BOX);
      box->labelfont(FL_BOLD+FL_ITALIC);
      box->labelsize(36);
      box->labeltype(FL_SHADOW_LABEL);
  window->end();

  window->show(argc, argv);

  Fl::add_timeout(1.0f, gui_update_callback);
  auto ret = Fl::run();
  Fl::remove_timeout(gui_update_callback);

  window->hide();

  //shutdown worker threads cleanly before destructors are called.
  mShuttingDown = true;
  router.shutdown();
  
  return ret;
}

