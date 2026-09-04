
#pragma once

/**
 * Provides alternatives to horrible X11 type names that
 * tend to clash with other systems such as Qt.
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrender.h>

// Strip X11 macro definitions.
#ifdef None
  #undef None
#endif
#ifdef Success
  #undef Success
#endif
#ifdef KeyRelease
  #undef KeyRelease
#endif
#ifdef Status
  #undef Status
#endif
#ifdef Bool
  #undef Bool
#endif
#ifdef CursorShape
  #undef CursorShape
#endif
#ifdef Complex
  #undef Complex
#endif

// Define overrides for X11 toolkit conflicts.
#define X11_NONE 0L
#define X11_SUCCESS 0
#define X11_KEY_RELEASE 3
