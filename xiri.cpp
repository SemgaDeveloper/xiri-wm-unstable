// Basic cpp directives
#include <iostream>
#include <string>

// Libs directives
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

// Xcb directives
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>

// Basic Setup for the first time


// Test Structures for future development

struct Settings {
  const char *terminal = "kitty";
  const char *application_launcher = "rofi -show drun";
  const char *display_name = "HDMI-1";
};

struct Windows {
  uint32_t win_width;
  uint32_t win_height;
  int32_t win_x_pos;
  int32_t win_y_pos;
};

struct Workspaces {
  uint32_t current_workspace;
  uint32_t workpaces_count;
};

// Atom Helper
/*static xcb_atom_t internAtom(xcb_connection_t *conn, const char *name) {
  xcb_intern_atom_cookie_t cookie =
      xcb_intern_atom(conn, 0, strlen(name), name);
  xcb_intern_atom_reply_t *reply =
      xcb_intern_atom_reply(conn, cookie, nullptr);
  if (!reply) return XCB_ATOM_NONE;
  xcb_atom_t atom = reply->atom;
  free(reply);
  return atom;
}*/


// Functions for WM work
void spawn(const char *exec) {
  pid_t pid = fork();
  if (pid == 0) {
    setsid();
    execlp("/bin/sh", "sh", "-c", exec, (char *)NULL);
    exit(1);
  }
}

void changeResolution(const char *output, uint32_t w, uint32_t h) {
  std::string cmd = std::string("xrandr --output ") + output +
                    " --mode " + std::to_string(w) + "x" + std::to_string(h);
  system(cmd.c_str());
}

void windowResize(xcb_connection_t *conn, xcb_window_t win, const Windows &winstruct) {
  uint32_t values[4] = {(uint32_t)winstruct.win_x_pos, (uint32_t)winstruct.win_y_pos,
                        winstruct.win_width, winstruct.win_height};
  uint16_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                  XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
  xcb_configure_window(conn, win, mask, values);
}


void mapWindow(xcb_connection_t *conn, xcb_window_t win) {
  xcb_map_window(conn, win);
  xcb_flush(conn);
  printf("Map window.");
}

void unmapWindow(xcb_connection_t *conn, xcb_window_t win) {
  xcb_unmap_window(conn, win);
  xcb_flush(conn);
  printf("Unmap window.");
}

int main() {
  // Setting yp the connection
  xcb_connection_t *connection = xcb_connect (NULL, NULL);
  if (xcb_connection_has_error(connection)) {
    printf("Failed to connect to the X server.\n");
  }
  const xcb_setup_t *setup = xcb_get_setup (connection);
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator (setup);
  xcb_screen_t *screen = iter.data; 
  // Mask Setup
  uint32_t mask = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                  XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;


  // Checking if another window manager are running
  xcb_void_cookie_t cookie = xcb_change_window_attributes_checked(connection, screen->root, XCB_CW_EVENT_MASK, &mask);
  if (xcb_generic_error_t *error = xcb_request_check(connection, cookie)) {
    printf("Another Window Manager already running");
    free(error);
    return 1;
  }
  xcb_flush(connection);
  Windows winstruct;
  Settings config;
  //changeResolution(config.display_name, winstruct.win_width, winstruct.win_height);
  spawn(config.terminal);
  while (xcb_generic_event_t *event = xcb_wait_for_event(connection)) {
    auto *e = (xcb_map_request_event_t *)event;
    switch (event->response_type & ~0x80) {
      case XCB_MAP_REQUEST:
        Windows winstruct;
        winstruct.win_x_pos = 0;
        winstruct.win_y_pos = 0;
        winstruct.win_width = 1920;
        winstruct.win_height = 1080;
        windowResize(connection, e->window, winstruct);
        mapWindow(connection, e->window);
        break;
      case XCB_UNMAP_NOTIFY:
        unmapWindow(connection, e->window);
        break;
    }
  }
  return 0;
}
