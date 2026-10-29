
#pragma once

/**
 * First app in the prototype series, an X-based
 * prototype similar to KDE Plasmoids.
 *
 * Desktop widgets with various views can be stuck in
 * place to the desktop below other windows.
 *
 * Widget remembers position, size, settings etc.
 *
 * Cursor hover reveals PinButton that toggles widget
 * in & out of "Stuck" state.
 *
 * When stuck to the desktop, widget ignores clicks &
 * passes all mouse actions to desktop (input transparency),
 * and has a transparent visual background.
 *
 * While "Stuck" to the desktop, the widgets window
 * button in the panels Task manager is removed.
 *
 */

/**
 * Common charsets to enable colorized cout messages.
 * "Ansii escape sequences for cursor color controls".
 */

// Disable colors if messages going to file.
static inline const bool USE_XCOLOR = isatty(STDOUT_FILENO);

static inline const char* XCOLOR_NORMAL  = USE_XCOLOR ? "\033[0m"     : "";
static inline const char* XCOLOR_BLACK   = USE_XCOLOR ? "\033[0;30m"  : "";
static inline const char* XCOLOR_WHITE   = USE_XCOLOR ? "\033[0;37m"  : "";
static inline const char* XCOLOR_RED     = USE_XCOLOR ? "\033[0;31m"  : "";
static inline const char* XCOLOR_GREEN   = USE_XCOLOR ? "\033[1;32m"  : "";
static inline const char* XCOLOR_YELLOW  = USE_XCOLOR ? "\033[1;33m"  : "";
static inline const char* XCOLOR_BLUE    = USE_XCOLOR ? "\033[1;34m"  : "";
static inline const char* XCOLOR_MAGENTA = USE_XCOLOR ? "\033[1;35m"  : "";
static inline const char* XCOLOR_CYAN    = USE_XCOLOR ? "\033[1;36m"  : "";

// X11 Render colors (not properly blended).
static inline constexpr XRenderColor TRANSPARENT_RCOLOR =
    { 0x0000, 0x0000, 0x0000, 0x0000 };

static inline constexpr XRenderColor BLACK_RCOLOR =
    { 0x0000, 0x0000, 0x0000, 0xffff };
static inline constexpr XRenderColor WHITE_RCOLOR =
    { 0xffff, 0xffff, 0xffff, 0xffff };

static inline constexpr XRenderColor GRAY_RCOLOR =
    { 0xd800, 0xd500, 0xd200, 0xffff };

static inline constexpr XRenderColor RUBBERBAND_RCOLOR =
    { 0xd200, 0xd800, 0xdc00, 0xffff };

static inline constexpr XRenderColor RED_RCOLOR =
    { 0xffff, 0x0000, 0x0000, 0x8000 };
static inline constexpr XRenderColor GREEN_RCOLOR =
    { 0x0000, 0xffff, 0x0000, 0xffff };
static inline constexpr XRenderColor BLUE_RCOLOR =
    { 0x0000, 0x0000, 0xffff, 0xffff };

static inline constexpr XRenderColor TURQUOISE_RCOLOR =
    { 0x0000, 0xffff, 0xfaff, 0xffff };

// x11 Fonts.
static inline constexpr const char*
    TIME_DISPLAY_FONT = "Serif-42:bold";


// App globals.
extern DisplayHelper* mDisplayHelper;
extern Display* mDisplay;
extern XHelper* mXHelper;

extern RecentsHelper* mRecentsHelper;
extern SettingsHelper* mSettingsHelper;
extern TranslationHelper* mTranslationHelper;

extern StickyWindow* mStickyWindow;
extern Canvas* mCanvas;

extern XImage* mPinInXImage;
extern XImage* mPinOutXImage;

extern Atom mCloseAppMessage;
extern Atom mConfigUpdated;

// Pngs.
static inline const QString PIN_IN_PNG_FILENAME = "pinIn.png";
static inline const QString PIN_OUT_PNG_FILENAME = "pinOut.png";

// Window Messages.
static inline const char* WINDOW_CLOSED = "WM_DELETE_WINDOW";
static inline const char* CONFIG_UPDATED = "CONFIG_UPDATED";
