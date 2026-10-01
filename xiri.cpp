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
std::string terminal = "kitty";
std::string application_launcher = "rofi -show drun";


// Test Structures for future development
struct Windows {
  uint32_t win_width;
  uint32_t win_height;
};

struct Workspaces {
  uint32_t current_workspace;
  uint32_t workpaces_count;
};

int main() {
  // Setting yp the connection
  xcb_connection_t *connection = xcb_connect (NULL, NULL);
  if (!connection) {
    printf("Failed to connect to the X server.\n");
  } else {
    printf("Connected to the X server succesfully\n");
  }
  const xcb_setup_t *setup = xcb_get_setup (connection);
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator (setup);
  xcb_screen_t *screen = iter.data; 
  printf("Testing\n");
  std::cout << "WM started, current setup is..." << terminal << " and " << application_launcher << std::endl;
  return 0;
}
