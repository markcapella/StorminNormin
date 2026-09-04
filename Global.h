
#pragma once

/**
 * Global headers, order important.
 *
 */
// Standard C libraries.
#include <atomic>
#include <chrono>
#include <cstdint>
#include <dirent.h>
#include <execution>
#include <fcntl.h>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <signal.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;
typedef chrono::steady_clock Clock;

// Qt6 libraries.
#include <QApplication>
#include <QSettings>
#include <QImage>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>
#include <QCheckBox>
#include <QLineEdit>
#include <QScrollArea>
#include <QMouseEvent>
#include <QFileInfo>
#include <QDir>
#include <QTimer>

#include <QToolTip>
#include <QStyleOptionFrame>
#include <QPointer>
#include <QComboBox>

// x11 libraries.
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

#include <X11/extensions/shape.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xrender.h>

// Vulcan libraries.
#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan.h>

// libpng library.
#include <png.h>

// libasan library.
#include <sanitizer/lsan_interface.h>
#include <sys/resource.h>

/**
 * Application specific.
 */

// Utiliy Macros.
#define randomIntegerUpTo(n) \
    ((int) (((n) <= 0) ? 0.0 : (drand48() * (n))))

#define randomMSUpTo(n) \
    (chrono::milliseconds((int) (((n).count() <= 0) ? 0 : \
        (drand48() * (n).count()))))

#define newRenderColor(r, g, b, a) \
    { (unsigned short)(((r) * (a)) / 255), \
      (unsigned short)(((g) * (a)) / 255), \
      (unsigned short)(((b) * (a)) / 255), \
      (unsigned short)((a) * 257) }

#define I18N(english) \
    mTranslationHelper->getTranslationOf(QString(english))

#define clampToRange(low, value, high) \
    ((max)((low), (min)((value), (high))))


/**
 * Debugging tools for timing method performance.
 */
#define INIT_TIMER(NAME) \
    chrono::high_resolution_clock::time_point NAME##_start; \
    chrono::high_resolution_clock::time_point NAME##_end; \
    long long NAME##_total_duration = 0; \
    long long NAME##_count = 0;

#define START_TIMER(NAME) \
    NAME##_start = chrono::high_resolution_clock::now();

#define END_TIMER(NAME) \
    NAME##_end = chrono::high_resolution_clock::now(); \
    NAME##_total_duration += chrono::duration_cast \
        <chrono::microseconds> (NAME##_end - NAME##_start).count(); \
    NAME##_count++;

#define REPORT_TIMER(NAME) \
    cout << "Timer [" << #NAME << "] - Runs: " << NAME##_count << \
        " | Avg: " << (NAME##_count > 0 ? \
            (double)NAME##_total_duration / NAME##_count : 0.0) << \
        " us." << endl;


// Application libraries.
#include "X11Types.h"

#include "TranslationHelper.h"
#include "TranslationHelperStrings.h"
#include "Canvas.h"

#include "Button.h"
#include "PinButton.h"
#include "QuitButton.h"
#include "MoveButton.h"
#include "SizeButton.h"

#include "ConfigDialog.h"
#include "ColorButton.h"
#include "AboutDialog.h"
#include "ConfigButton.h"

#include "StickyWindow.h"

#include "SettingsHelper.h"
#include "RecentsHelper.h"
#include "DisplayHelper.h"

#include "WinInfo.h"
#include "XHelper.h"

#include "StickyWidgetIII.h"

#include "AutoHideDelayHints.h"
#include "DesktopPreferenceHints.h"
#include "OpacityHints.h"
#include "SaturationHints.h"
#include "MaxStarSizeHints.h"

#include "ComboboxDelegate.h"
