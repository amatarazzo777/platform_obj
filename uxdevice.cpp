/**
\file uxdevice.cpp

\author Anthony Matarazzo

\date 3/26/20
\version 1.0

\brief rendering and platform services.

*/
#include "uxdevice.hpp"

using namespace std;
using namespace uxdevice;

/**
\internal
\brief The routine is the main rendering thread. The thread runs
when necessary based upon a condition variable.
Locks are placed on the surface and
rectangle list. The surface may change due to user resizing the gui
window so a spin flag is used to accommodate the functionality. That is
drawing cannot occur on the graphical while the surface is being resized.
*/
void uxdevice::SurfaceArea::render_loop(void) {
  while (bProcessing) {

    // surfacePrime checks to see if the surface exists.
    // if so, the two possible work flows are painting
    // background rectangles that are cause by the user resizing the
    // window to a greater value. These values are inserted as part
    // of the paint event. As well, the underlying surface may
    // need to be resized. This function acquires locks on these
    // small lists for the multi-threaded necessity.
    // searches for unready and syncs display context
    // if no work exists  it waits on the cvRenderWork condition variable.
    if (context.surface_prime()) {
      context.render();
    }

    if (context.error_state())
      fnError(context.error_text());
  }
}

/*
\brief the dispatch routine is invoked by the messageLoop.
If default
 * handling is to be supplied, the method invokes the
necessary operation.

*/
void uxdevice::SurfaceArea::dispatch_event(const event_t &evt) {

  switch (evt.type) {
  case eventType::none:
    break;
  case eventType::paint: {
    context.state_surface(evt.x, evt.y, evt.w, evt.h);
  } break;
  case eventType::resize:
    context.resize_surface(evt.w, evt.h);
    break;
  case eventType::keydown: {

  } break;
  case eventType::keyup: {

  } break;
  case eventType::keypress: {

  } break;
  case eventType::mousemove:
    break;
  case eventType::mousedown:

    break;
  case eventType::mouseup:
    break;
  case eventType::wheel:
    break;

  case eventType::focus:
    break;
  case eventType::blur:
    break;
  case eventType::mouseenter:
    break;
  case eventType::click:
    break;
  case eventType::dblclick:
    break;
  case eventType::contextmenu:
    break;
  case eventType::mouseleave:
    break;
  }
  fnEvents(evt);
}
/**
\internal
\brief The entry point that processes messages from the operating
system application level services. Typically on Linux this is a
coupling of xcb and keysyms library for keyboard. Previous
incarnations of technology such as this typically used xserver.
However, XCB is the newer form. Primarily looking at the code of such
programs as vlc, the routine simply places pixels into the memory
buffer. while on windows the direct x library is used in combination
with windows message queue processing.
*/
void uxdevice::SurfaceArea::start_processing(void) {
  // setup the event dispatcher
  eventHandler ev = std::bind(&uxdevice::SurfaceArea::dispatch_event, this,
                              std::placeholders::_1);
  context.cacheThreshold = 2000;
  std::thread thrRenderer([=]() {
    bProcessing = true;
    render_loop();
  });

  std::thread thrMessageQueue([=]() {
    bProcessing = true;
    message_loop();
  });

  thrRenderer.detach();
  thrMessageQueue.detach();
}

/**
\internal

\brief The function maps the event id to the appropriate vector.
This is kept statically here for resource management.

\param eventType evtType
*/
std::list<eventHandler> &
uxdevice::SurfaceArea::get_event_vector(eventType evtType) {
  static std::unordered_map<eventType, std::list<eventHandler> &>
      eventTypeMap = {{eventType::focus, onfocus},
                      {eventType::blur, onblur},
                      {eventType::resize, onresize},
                      {eventType::keydown, onkeydown},
                      {eventType::keyup, onkeyup},
                      {eventType::keypress, onkeypress},
                      {eventType::mouseenter, onmouseenter},
                      {eventType::mouseleave, onmouseleave},
                      {eventType::mousemove, onmousemove},
                      {eventType::mousedown, onmousedown},
                      {eventType::mouseup, onmouseup},
                      {eventType::click, onclick},
                      {eventType::dblclick, ondblclick},
                      {eventType::contextmenu, oncontextmenu},
                      {eventType::wheel, onwheel}};
  auto it = eventTypeMap.find(evtType);
  return it->second;
}
/**
\internal
\brief
The function will return the address of a std::function for the purposes
of equality testing. Function from
https://stackoverflow.com/questions/20833453/comparing-stdfunctions-for-equality

*/
template <typename T, typename... U>
size_t getAddress(std::function<T(U...)> f) {
  typedef T(fnType)(U...);
  fnType **fnPointer = f.template target<fnType *>();
  return (size_t)*fnPointer;
}

#if 0
/**

\brief The function is invoked when an event occurs. Normally this occurs
from the platform device. However, this may be invoked by the soft
generation of events.

*/
void uxdevice::SurfaceArea::dispatch(const event &e) {
  auto &v = getEventVector(e.evtType);
  for (auto &fn : v)
    fn(e);
}
#endif

/**
  \internal
  \brief opens a window on the target OS

*/
uxdevice::SurfaceArea::SurfaceArea() {}
uxdevice::SurfaceArea::SurfaceArea(const std::string &sSurfaceAreaTitle) {}

uxdevice::SurfaceArea::SurfaceArea(const eventHandler &evtDispatcher) {}
uxdevice::SurfaceArea::SurfaceArea(const CoordinateList &coordinates) {}

uxdevice::SurfaceArea::SurfaceArea(const CoordinateList &coordinates,
                                   const std::string &sSurfaceAreaTitle) {}


uxdevice::SurfaceArea::SurfaceArea(const CoordinateList &coordinates,
                                   const std::string &sSurfaceAreaTitle,
                                   const eventHandler &evtDispatcher,
                                   const Paint &background) {}

uxdevice::SurfaceArea::SurfaceArea(const CoordinateList &coord,
                                   const std::string &sWindowTitle,
                                   const Paint &background) {
  SurfaceArea ret;
  auto it = coord.begin();

  context.windowWidth = *it;
  it++;
  context.windowHeight = *it;
  context.brush = background;

  // this open provides interoperability between xcb and xwindows
  // this is used here because of the necessity of key mapping.
  context.xdisplay = XOpenDisplay(nullptr);
  if (!context.xdisplay) {
    close_window();
    std::stringstream sError;
    sError << "ERR_XWIN "
           << "  " << __FILE__ << " " << __func__;
    throw std::runtime_error(sError.str());
  }

  /* get the connection to the X server */
  context.connection = XGetXCBConnection(context.xdisplay);
  if (!context.xdisplay) {
    close_window();
    std::stringstream sError;
    sError << "ERR_XWIN "
           << "  " << __FILE__ << " " << __func__;
    throw std::runtime_error(sError.str());
  }

  /* Get the first screen */
  context.screen =
      xcb_setup_roots_iterator(xcb_get_setup(context.connection)).data;
  if (!context.screen) {
    close_window();
    std::stringstream sError;
    sError << "ERR_XWIN "
           << "  " << __FILE__ << " " << __func__;
    throw std::runtime_error(sError.str());
  }

  context.syms = xcb_key_symbols_alloc(context.connection);
  if (!context.syms) {
    close_window();
    std::stringstream sError;
    sError << "ERR_XWIN "
           << "  " << __FILE__ << " " << __func__;
    throw std::runtime_error(sError.str());
  }

  /* Create black (foreground) graphic context */
  context.window = context.screen->root;
  context.graphics = xcb_generate_id(context.connection);
  uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  uint32_t values[] = {context.screen->black_pixel, 0};
  xcb_create_gc(context.connection, context.graphics, context.window, mask,
                values);

  if (!context.graphics) {
    close_window();
    std::stringstream sError;
    sError << "ERR_XWIN "
           << "  " << __FILE__ << " " << __func__;
    throw std::runtime_error(sError.str());
  }

  /* Create a window */
  context.window = xcb_generate_id(context.connection);
  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  mask = XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK;
  mask = XCB_CW_BORDER_PIXEL | XCB_CW_BIT_GRAVITY | XCB_CW_OVERRIDE_REDIRECT |
         XCB_CW_SAVE_UNDER | XCB_CW_EVENT_MASK;

  uint32_t vals[] = {
      context.screen->black_pixel, XCB_GRAVITY_NORTH_WEST, 0, 1,
      XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS |
          XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
          XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_BUTTON_PRESS |
          XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_STRUCTURE_NOTIFY};

  xcb_create_window(context.connection, XCB_COPY_FROM_PARENT, context.window,
                    context.screen->root, 0, 0,
                    static_cast<unsigned short>(context.windowWidth),
                    static_cast<unsigned short>(context.windowHeight), 0,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, context.screen->root_visual,
                    mask, vals);
  // set window title
  xcb_change_property(context.connection, XCB_PROP_MODE_REPLACE, context.window,
                      XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, sWindowTitle.size(),
                      sWindowTitle.data());

  if (!context.window) {
    close_window();
    std::stringstream sError;
    sError << "ERR_XWIN "
           << "  " << __FILE__ << " " << __func__;
    throw std::runtime_error(sError.str());
  }

  /* you init the connection and screen_nbr */
  xcb_depth_iterator_t depth_iter;

  depth_iter = xcb_screen_allowed_depths_iterator(context.screen);
  for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
    xcb_visualtype_iterator_t visual_iter;

    visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
    for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
      if (context.screen->root_visual == visual_iter.data->visual_id) {
        context.visualType = visual_iter.data;
        break;
      }
    }
  }

  // create xcb surface

  context.xcbSurface = cairo_xcb_surface_create(
      context.connection, context.window, context.visualType,
      context.windowWidth, context.windowHeight);
  if (!context.xcbSurface) {
    close_window();
    std::stringstream sError;
    sError << "ERR_CAIRO "
           << "  " << __FILE__ << " " << __func__;
    throw std::runtime_error(sError.str());
  }

  // create cairo context
  context.cr = cairo_create(context.xcbSurface);
  if (!context.cr) {
    close_window();
    std::stringstream sError;
    sError << "ERR_CAIRO "
           << "  " << __FILE__ << " " << __func__;
    throw std::runtime_error(sError.str());
  }

  /* Map the window on the screen and flush*/
  xcb_map_window(context.connection, context.window);
  xcb_flush(context.connection);
  context.windowOpen = true;

  cairo_surface_flush(context.xcbSurface);

  return;
}

/**
  \internal
  \brief closes a window on the target OS


*/
uxdevice::SurfaceArea::~SurfaceArea(void) { close_window(); }

/**
  \internal
  \brief closes a window on the target OS


*/
void uxdevice::SurfaceArea::close_window(void) {
  if (context.xcbSurface) {
    cairo_surface_destroy(context.xcbSurface);
    context.xcbSurface = nullptr;
  }
  if (context.cr) {
    cairo_destroy(context.cr);
    context.cr = nullptr;
  }
  if (context.graphics) {
    xcb_free_gc(context.connection, context.graphics);
    context.graphics = 0;
  }

  if (context.syms) {
    xcb_key_symbols_free(context.syms);
    context.syms = nullptr;
  }

  if (context.window) {
    xcb_destroy_window(context.connection, context.window);
    context.window = 0;
  }
  if (context.xdisplay) {
    XCloseDisplay(context.xdisplay);
    context.xdisplay = nullptr;
  }

  context.windowOpen = false;
}

/**
\internal
\brief the routine handles the message processing for the specific
operating system. The function is called from processEvents.

*/
void uxdevice::SurfaceArea::message_loop(void) {
  xcb_generic_event_t *xcbEvent;

  // is window open?
  while (bProcessing && !context.connection)
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
  if (!context.connection)
    return;

  // setup close window event
  xcb_intern_atom_cookie_t cookie =
      xcb_intern_atom(context.connection, 1, 12, "WM_PROTOCOLS");
  xcb_intern_atom_reply_t *reply =
      xcb_intern_atom_reply(context.connection, cookie, 0);

  xcb_intern_atom_cookie_t cookie2 =
      xcb_intern_atom(context.connection, 0, 16, "WM_DELETE_WINDOW");
  xcb_intern_atom_reply_t *reply2 =
      xcb_intern_atom_reply(context.connection, cookie2, 0);

  xcb_change_property(context.connection, XCB_PROP_MODE_REPLACE, context.window,
                      (*reply).atom, 4, 32, 1, &(*reply2).atom);

  // process Message queue
  std::list<xcb_generic_event_t *> xcbEvents;
  while (bProcessing && (xcbEvent = xcb_wait_for_event(context.connection))) {
    xcbEvents.emplace_back(xcbEvent);

    // qt5 does this, it queues all of the input messages at once.
    // this makes the processing of painting and reading input faster.
    while (bProcessing &&
           (xcbEvent = xcb_poll_for_queued_event(context.connection)))
      xcbEvents.emplace_back(xcbEvent);

    while (!xcbEvents.empty()) {
      xcbEvent = xcbEvents.front();
      switch (xcbEvent->response_type & ~0x80) {
      case XCB_MOTION_NOTIFY: {
        xcb_motion_notify_event_t *motion =
            (xcb_motion_notify_event_t *)xcbEvent;
        dispatch_event(event_t{
            eventType::mousemove,
            (short)motion->event_x,
            (short)motion->event_y,
        });
      } break;
      case XCB_BUTTON_PRESS: {
        xcb_button_press_event_t *bp = (xcb_button_press_event_t *)xcbEvent;
        if (bp->detail == XCB_BUTTON_INDEX_4 ||
            bp->detail == XCB_BUTTON_INDEX_5) {
          dispatch_event(
              event_t{eventType::wheel, (short)bp->event_x, (short)bp->event_y,
                    (short)(bp->detail == XCB_BUTTON_INDEX_4 ? 1 : -1)});

        } else {
          dispatch_event(event_t{eventType::mousedown, (short)bp->event_x,
                               (short)bp->event_y, (short)bp->detail});
        }
      } break;
      case XCB_BUTTON_RELEASE: {
        xcb_button_release_event_t *br = (xcb_button_release_event_t *)xcbEvent;
        // ignore button 4 and 5 which are wheel events.
        if (br->detail != XCB_BUTTON_INDEX_4 &&
            br->detail != XCB_BUTTON_INDEX_5)
          dispatch_event(event_t{eventType::mouseup, (short)br->event_x,
                               (short)br->event_y, (short)br->detail});
      } break;
      case XCB_KEY_PRESS: {
        xcb_key_press_event_t *kp = (xcb_key_press_event_t *)xcbEvent;
        xcb_keysym_t sym = xcb_key_press_lookup_keysym(context.syms, kp, 0);
        if (sym < 0x99) {
          XKeyEvent keyEvent;
          keyEvent.display = context.xdisplay;
          keyEvent.keycode = kp->detail;
          keyEvent.state = kp->state;
          keyEvent.root = kp->root;
          keyEvent.time = kp->time;
          keyEvent.window = kp->event;
          keyEvent.serial = kp->sequence;

          std::array<char, 316> buf{};
          if (XLookupString(&keyEvent, buf.data(), buf.size(), nullptr,
                            nullptr))
            dispatch_event(event_t{eventType::keypress, (char)buf[0]});
        } else {
          dispatch_event(event_t{eventType::keydown, sym});
        }
      } break;
      case XCB_KEY_RELEASE: {
        xcb_key_release_event_t *kr = (xcb_key_release_event_t *)xcbEvent;
        xcb_keysym_t sym = xcb_key_press_lookup_keysym(context.syms, kr, 0);
        dispatch_event(event_t{eventType::keyup, sym});
      } break;
      case XCB_EXPOSE: {
        xcb_expose_event_t *eev = (xcb_expose_event_t *)xcbEvent;

        dispatch_event(event_t{eventType::paint, (short)eev->x, (short)eev->y,
                             (short)eev->width, (short)eev->height});

      } break;
      case XCB_CONFIGURE_NOTIFY: {
        const xcb_configure_notify_event_t *cfgEvent =
            (const xcb_configure_notify_event_t *)xcbEvent;

        if (cfgEvent->window == context.window) {
          dispatch_event(event_t{eventType::resize, (short)cfgEvent->width,
                               (short)cfgEvent->height});
        }
      } break;
      case XCB_CLIENT_MESSAGE: {
        if ((*(xcb_client_message_event_t *)xcbEvent).data.data32[0] ==
            (*reply2).atom) {
          bProcessing = false;
        }
      } break;
      }
      free(xcbEvent);
      xcbEvents.pop_front();
    }
    context.state_notify_complete();
  }
}

/**
\brief API interface, just data is passed to objects. Objects are
dynamically allocated as classes derived from a unit base. Mutex is used one
display list to not get in the way of the rendering loop,

*/

/**
\brief clears the display list
*/

void uxdevice::SurfaceArea::clear(void) {
  DL_SPIN;
  context.clear();
  DL.clear();
  DL_CLEAR;
}

void uxdevice::SurfaceArea::notify_complete(void) {
  context.state_notify_complete();
}


/**
\brief sets the text
*/
SurfaceArea &uxdevice::SurfaceArea::stream_input(const std::string &s) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<std::string>(s));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<std::string>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const std::stringstream &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<std::string>(_val.str()));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<std::string>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const antialias &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<antialias>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<antialias>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const source &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<source>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<source>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const text_outline &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<text_outline>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<text_outline>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const text_fill &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<text_fill>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<text_fill>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const text_shadow &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<text_shadow>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<text_shadow>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::stream_input(const text_outline_none &_val) {
  DL_SPIN;
  auto item = DL.emplace_back( make_shared<CLEARUNIT>([=]() {
    context.currentUnits._text_outline.reset();
  }));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const text_fill_none &_val) {
  DL_SPIN;
  auto item = DL.emplace_back( make_shared<CLEARUNIT>([=]() {
    context.currentUnits._text_fill.reset();
  }));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const text_shadow_none &_val) {
  DL_SPIN;
  auto item = DL.emplace_back( make_shared<CLEARUNIT>([=]() {
    context.currentUnits._text_shadow.reset();
  }));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const text_alignment &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<text_alignment>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<text_alignment>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::stream_input(const alignment_t &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<text_alignment>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<text_alignment>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::stream_input(const coordinates &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<coordinates>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<coordinates>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const index_by &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<index_by>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<index_by>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const line_width &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<line_width>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<line_width>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const indent &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<indent>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<indent>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const ellipse &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<ellipse>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<ellipse>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const line_space &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<line_space>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<line_space>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const tab_stops &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<tab_stops>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<tab_stops>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stream_input(const text_font &_val) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<text_font>(_val));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<text_font>(item));
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::image(const std::string &s) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<IMAGE>(s));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<IMAGE>(item));
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::stroke(const Paint &p) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<STROKE>(p));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<STROKE>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::stroke(u_int32_t c) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<STROKE>(c));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<STROKE>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::stroke(const string &c) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<STROKE>(c));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<STROKE>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::stroke(const std::string &c, double w,
                                           double h) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<STROKE>(c, w, h));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<STROKE>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::stroke(double _r, double _g, double _b) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<STROKE>(_r, _g, _b));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<STROKE>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::stroke(double _r, double _g, double _b,
                                           double _a) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<STROKE>(_r, _g, _b, _a));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<STROKE>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::stroke(double x0, double y0, double x1,
                                           double y1, const ColorStops &cs) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<STROKE>(x0, y0, x1, y1, cs));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<STROKE>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::stroke(double cx0, double cy0,
                                           double radius0, double cx1,
                                           double cy1, double radius1,
                                           const ColorStops &cs) {
  DL_SPIN;
  auto item = DL.emplace_back(
      make_shared<STROKE>(cx0, cy0, radius0, cx1, cy1, radius1, cs));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<STROKE>(item));
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::fill(const Paint &p) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<FILL>(p));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<FILL>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::fill(u_int32_t c) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<FILL>(c));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<FILL>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::fill(const string &c) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<FILL>(c));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<FILL>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::fill(const std::string &c, double w,
                                         double h) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<FILL>(c, w, h));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<FILL>(item));
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::fill(double _r, double _g, double _b) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<FILL>(_r, _g, _b));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<FILL>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::fill(double _r, double _g, double _b,
                                         double _a) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<FILL>(_r, _g, _b, _a));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<FILL>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::fill(double x0, double y0, double x1,
                                         double y1, const ColorStops &cs) {
  DL_SPIN;
  auto item = DL.emplace_back(make_shared<FILL>(x0, y0, x1, y1, cs));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<FILL>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::fill(double cx0, double cy0, double radius0,
                                         double cx1, double cy1, double radius1,
                                         const ColorStops &cs) {
  DL_SPIN;
  auto item = DL.emplace_back(
      make_shared<PEN>(cx0, cy0, radius0, cx1, cy1, radius1, cs));
  item->invoke(context);
  context.set_unit(std::dynamic_pointer_cast<FILL>(item));
  DL_CLEAR;
  return *this;
}
SurfaceArea &uxdevice::SurfaceArea::surface_brush(Paint &b) {
  context.surfaceBrush(b);
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::save(void) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func = std::bind(cairo_save, _1);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::restore(void) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func = std::bind(cairo_restore, _1);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::push(content c) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func;
  if (c == content::all) {
    func = std::bind(cairo_push_group, _1);
  } else {
    func = std::bind(cairo_push_group_with_content, _1,
                     static_cast<cairo_content_t>(c));
  }
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::pop(bool bToSource) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func;
  if (bToSource) {
    func = std::bind(cairo_pop_group_to_source, _1);
  } else {
    func = std::bind(cairo_pop_group, _1);
  }
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::translate(double x, double y) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func = std::bind(cairo_translate, _1, x, y);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::rotate(double angle) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func = std::bind(cairo_rotate, _1, angle);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::device_offset(double x, double y) {
  context.deviceOffset(x, y);
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::device_scale(double x, double y) {
  context.deviceScale(x, y);
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::scale(double x, double y) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func = std::bind(cairo_scale, _1, x, y);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::transform(const Matrix &m) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func = std::bind(cairo_transform, _1, &m._matrix);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::matrix(const Matrix &m) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func = std::bind(cairo_set_matrix, _1, &m._matrix);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::identity(void) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func = std::bind(cairo_identity_matrix, _1);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::device(double &x, double &y) {
  using namespace std::placeholders;
  DL_SPIN;
  auto fn = [](cairo_t *cr, double &x, double &y) {
    double _x = x;
    double _y = y;
    cairo_user_to_device(cr, &_x, &_y);
    x = _x;
    y = _y;
  };

  CAIRO_FUNCTION func = std::bind(fn, _1, x, y);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::device_distance(double &x, double &y) {
  using namespace std::placeholders;
  DL_SPIN;
  auto fn = [](cairo_t *cr, double &x, double &y) {
    double _x = x;
    double _y = y;
    cairo_user_to_device_distance(cr, &_x, &_y);
    x = _x;
    y = _y;
  };

  CAIRO_FUNCTION func = std::bind(fn, _1, x, y);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::user(double &x, double &y) {
  using namespace std::placeholders;
  DL_SPIN;
  auto fn = [](cairo_t *cr, double &x, double &y) {
    double _x = x, _y = y;
    cairo_device_to_user(cr, &_x, &_y);
    x = _x;
    y = _y;
  };

  CAIRO_FUNCTION func = std::bind(fn, _1, x, y);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::user_distance(double &x, double &y) {
  using namespace std::placeholders;
  DL_SPIN;
  auto fn = [](cairo_t *cr, double &x, double &y) {
    double _x = x, _y = y;
    cairo_device_to_user_distance(cr, &_x, &_y);
    x = _x;
    y = _y;
  };

  CAIRO_FUNCTION func = std::bind(fn, _1, x, y);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::cap(lineCap c) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_OPTION func =
      std::bind(cairo_set_line_cap, _1, static_cast<cairo_line_cap_t>(c));
  auto item = DL.emplace_back(make_shared<OPTION_FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::join(lineJoin j) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func =
      std::bind(cairo_set_line_join, _1, static_cast<cairo_line_join_t>(j));
  auto item = DL.emplace_back(make_shared<OPTION_FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::line_width(double dWidth) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func = std::bind(cairo_set_line_width, _1, dWidth);
  auto item = DL.emplace_back(make_shared<OPTION_FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::miter_limit(double dLimit) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func = std::bind(cairo_set_miter_limit, _1, dLimit);
  auto item = DL.emplace_back(make_shared<OPTION_FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::dashes(const std::vector<double> &dashes,
                                           double offset) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func =
      std::bind(cairo_set_dash, _1, dashes.data(), dashes.size(), offset);
  auto item = DL.emplace_back(make_shared<OPTION_FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::tollerance(double _t) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func = std::bind(cairo_set_tolerance, _1, _t);
  auto item = DL.emplace_back(make_shared<OPTION_FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::op(op_t _op) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func =
      std::bind(cairo_set_operator, _1, static_cast<cairo_operator_t>(_op));
  auto item = DL.emplace_back(make_shared<OPTION_FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::source(Paint &p) {
  using namespace std::placeholders;
  DL_SPIN;
  auto fn = [](cairo_t *cr, Paint &p) { p.emit(cr); };
  CAIRO_FUNCTION func = std::bind(fn, _1, p);
  auto item = DL.emplace_back(make_shared<OPTION_FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::arc(double xc, double yc, double radius,
                                        double angle1, double angle2,
                                        bool bNegative) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func;
  func = std::bind(cairo_arc, _1, xc, yc, radius, angle1, angle2);
  auto item = DL.emplace_back(make_shared<DRAW_FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::negative_arc(double xc, double yc, double radius,
                                        double angle1, double angle2,
                                        bool bNegative) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func;
  func = std::bind(cairo_arc_negative, _1, xc, yc, radius, angle1, angle2);

  auto item = DL.emplace_back(make_shared<DRAW_FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::curve(double x1, double y1, double x2,
                                          double y2, double x3, double y3) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func;
  if (bRelative) {
    func = std::bind(cairo_rel_curve_to, _1, x1, y1, x2, y2, x3, y3);
  } else {
    func = std::bind(cairo_curve_to, _1, x1, y1, x2, y2, x3, y3);
  }
  auto item = DL.emplace_back(make_shared<DRAW_FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::line(double x, double y) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func;
  if (bRelative) {
    func = std::bind(cairo_rel_line_to, _1, x, y);
  } else {
    func = std::bind(cairo_line_to, _1, x, y);
  }
  auto item = DL.emplace_back(make_shared<DRAW_FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::rectangle(double x, double y, double width,
                                              double height) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func = std::bind(cairo_rectangle, _1, x, y, width, height);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

SurfaceArea &uxdevice::SurfaceArea::close_path() {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func;
  func = std::bind(cairo_close_path, _1);
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::stroke(bool bPreserve) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func;
  if (bPreserve) {
    func = std::bind(cairo_stroke_preserve, _1);
  } else {
    func = std::bind(cairo_stroke, _1);
  }
  auto fn = [=, &p](cairo_t *cr) {
    p.emit(cr);
    func(cr);
  };
  auto item = DL.emplace_back(make_shared<FUNCTION>(fn));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::fill(bool bPreserve) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func;
  if (bPreserve) {
    func = std::bind(cairo_fill_preserve, _1);
  } else {
    func = std::bind(cairo_fill, _1);
  }
  auto fn = [=, &p](cairo_t *cr) {
    p.emit(cr);
    func(cr);
  };

  auto item = DL.emplace_back(make_shared<FUNCTION>(fn));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::paint(double alpha) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func;
  if (alpha == 1.0) {
    func = std::bind(cairo_paint, _1);
  } else {
    func = std::bind(cairo_paint_with_alpha, _1, alpha);
  }
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}
/**
\brief
*/
SurfaceArea &uxdevice::SurfaceArea::move_to(double x, double y) {
  using namespace std::placeholders;
  DL_SPIN;
  CAIRO_FUNCTION func;
  if (bRelative) {
    func = std::bind(cairo_rel_move_to, _1, x, y);
  } else {
    func = std::bind(cairo_move_to, _1, x, y);
  }
  auto item = DL.emplace_back(make_shared<FUNCTION>(func));
  item->invoke(context);
  DL_CLEAR;
  return *this;
}

/***************************************************************************/

/**
  \internal
  \brief the function draws the cursor.
  */
void uxdevice::SurfaceArea::draw_caret(const int x, const int y, const int h) {}

std::string _errorReport(std::string sourceFile, int ln, std::string sfunc,
                         std::string cond, std::string ecode) {
  std::stringstream ss;
  ss << sourceFile << "(" << ln << ") " << sfunc << "  " << cond << ecode;
  return ss.str();
}
