#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xfixes.h>

static void selection_request_event(XEvent* in);
static void selection_notify_event(XFixesSelectionNotifyEvent* ev);
static unsigned long get_window_property(Atom property, unsigned char** data);
static void set_selection_owner(Atom selection);
static void cleanup(void);

Atom XA_CLIPBOARD;
Atom XA_TARGETS;
Atom XA_MULTIPLE;
Atom XA_UTF8_STRING;

Display* display = NULL;
Window window;
int xfixes_event_base;
int xfixes_error_base;

unsigned char* buf = NULL;
unsigned int buf_size = 0;
unsigned int CONVERTION_DELAY_NS = 100 * 1000 * 1000;

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  display = XOpenDisplay(NULL);
  __auto_type screen = DefaultScreen(display);
  __auto_type root = DefaultRootWindow(display);
  window = XCreateSimpleWindow(display, root, 0, 0, 100, 100, 0, BlackPixel(display, screen), BlackPixel(display, screen));

  XA_CLIPBOARD = XInternAtom(display, "CLIPBOARD", true);
  XA_TARGETS = XInternAtom(display, "TARGETS", true);
  XA_MULTIPLE = XInternAtom(display, "MULTIPLE", true);
  XA_UTF8_STRING = XInternAtom(display, "UTF8_STRING", true);

  __auto_type xfixes_available = XFixesQueryExtension(display, &xfixes_event_base, &xfixes_error_base);
  if (xfixes_available) {
    XFixesSelectSelectionInput(display, window, XA_PRIMARY, XFixesSetSelectionOwnerNotifyMask);
    XFixesSelectSelectionInput(display, window, XA_CLIPBOARD, XFixesSetSelectionOwnerNotifyMask);

    XEvent ev;

    while (true) {
      XNextEvent(display, &ev);

      if (ev.type == SelectionRequest) {
        selection_request_event(&ev);
      } else if (ev.type == xfixes_event_base + XFixesSelectionNotify) {
        selection_notify_event((XFixesSelectionNotifyEvent*)&ev);
      }
    }

    cleanup();
  }

  return EXIT_FAILURE;
}

static void selection_request_event(XEvent* in) {
  __auto_type req = in->xselectionrequest;
  XEvent out;
  bool propagate = true;

  out.xselection.type = SelectionNotify;
  out.xselection.requestor = req.requestor;
  out.xselection.selection = req.selection;
  out.xselection.target = req.target;
  out.xselection.time = req.time;
  out.xselection.property = None;
  
  if (req.target == XA_UTF8_STRING) {
    out.xselection.property = req.property;
    XChangeProperty(req.display, req.requestor, req.property, req.target, 8, PropModeReplace, buf, buf_size);
  } else if (req.target == XA_TARGETS) {
    Atom targets[] = {XA_TARGETS, XA_MULTIPLE, XA_UTF8_STRING}; 

    out.xselection.property = req.property;
    XChangeProperty(req.display, req.requestor, req.property, XA_ATOM, 32, PropModeReplace, (unsigned char*)targets, sizeof(targets));
  }

  XSendEvent(req.display, req.requestor, propagate, NoEventMask, &out);
}

static void selection_notify_event(XFixesSelectionNotifyEvent* ev) {
  if (window != ev->owner) {
    XConvertSelection(display, ev->selection, XA_UTF8_STRING, ev->selection, window, CurrentTime);
    XSync(display, true);

    struct timespec ts = {.tv_sec = 0, .tv_nsec = CONVERTION_DELAY_NS};
    nanosleep(&ts, NULL);

    buf_size = get_window_property(ev->selection, &buf);

    set_selection_owner(ev->selection == XA_PRIMARY ? XA_CLIPBOARD : XA_PRIMARY);
  }
}

static unsigned long get_window_property(Atom property, unsigned char** data) {
  Atom actual_type;
  int actual_format;
  unsigned long nitems;
  __auto_type offset = 0;
  unsigned long bytes_left;

  XGetWindowProperty(display, window, property, offset, 0, false, AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_left, data);

  if (bytes_left > 0) {
    cleanup();

    XGetWindowProperty(display, window, property, offset, bytes_left, false, AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_left, data);
  }

  return nitems;
}

static void set_selection_owner(Atom selection) {
  XSetSelectionOwner(display, selection, window, CurrentTime);
}

void cleanup() {
  XFree(buf);
  buf_size = 0;
}
