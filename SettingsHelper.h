
#pragma once

/**
 * SettingsHelper provides a permanant keyed values
 * store for user preferences in a file tied to appName.
 */
enum SettingsPropertyType {
    NONE_VALUETYPE,
    BOOL_VALUETYPE,
    COLOR_VALUETYPE,
    SLIDER_VALUETYPE,
    DIVIDER_VALUETYPE,
    COMBOBOX_VALUETYPE
};

class SettingsHelper {

    public:
        // Tab groups.
        static inline const QString GENERAL_GROUP       = "General";
        static inline const QString FROSTEDFLAKES_GROUP = "FrostedFlakes";

        /**
         * Configurable Settings strings.
         */
        static inline const QString FROSTEDFLAKES_SIZE        = "❄️ Flake Size";
        static inline const QString FROSTEDFLAKES_COUNT       = "Flake Count";

        static inline const QString FROSTEDFLAKES_COLOR_ONE   = "Flake Color One";
        static inline const QString FROSTEDFLAKES_COLOR_TWO   = "Flake Color Two";
        static inline const QString FROSTEDFLAKES_COLOR_THREE = "Flake Color Three";
        static inline const QString FROSTEDFLAKES_COLOR_FOUR  = "Flake Color Four";

        static inline const QString WIND_STRENGTH             = "Wind Strength";
        static inline const QString WIND_LENGTH               = "Wind Length";
        static inline const QString WIND_BURST_LENGTH         = "Wind Burst Length";

        static inline const QString BACKGROUND_COLOR          = "Background Color";
        static inline const QString BACKGROUND_OPACITY        = "Background Opacity";

        static inline const QString APP_LANGUAGE              = "Language";

        static inline const QString ON_TOP_INSTEAD            = "Stick to Top";
        static inline const QString ALLOW_DESKTOP_DRAG        = "Allow Desktop Drag";
        static inline const QString PREFERRED_DESKTOP         = "Preferred Desktop";
        static inline const QString DESKTOP_OVERHANG          = "Allow Desktop Overhang";

        static inline const QString SHOW_PIN_ON_WINDOW_HOVER  = "Show Pin on Window Hover";
        static inline const QString AUTOHIDE_CONTROLS         = "Auto hide Controls";
        static inline const QString AUTOHIDE_DELAY            = "Auto hide Delay";

        static inline const QString SHOW_SETTINGS_HINTS       = "Show Settings Hints";
        static inline const QString SHOW_ICONS_ON_BUTTONS     = "Show Icons on Buttons";

        /**
         * Configurable Settings hint strings.
         */
        static inline const QString FROSTEDFLAKES_SIZE_HINT        = "Select smaller to larger sized storm items.";
        static inline const QString FROSTEDFLAKES_COUNT_HINT       = "Select more or less items in the storm.";

        static inline const QString FROSTEDFLAKES_COLOR_ONE_HINT   = "Select up to four storm item colors.";
        static inline const QString FROSTEDFLAKES_COLOR_TWO_HINT   = "Select up to four storm item colors.";
        static inline const QString FROSTEDFLAKES_COLOR_THREE_HINT = "Select up to four storm item colors.";
        static inline const QString FROSTEDFLAKES_COLOR_FOUR_HINT  = "Select up to four storm item colors.";

        static inline const QString WIND_STRENGTH_HINT             = "Select a weaker or stronger wind strength.";
        static inline const QString WIND_LENGTH_HINT               = "Select shorter or longer windy periods.";
        static inline const QString WIND_BURST_LENGTH_HINT         = "Select shorter or longer wind burst periods.";

        static inline const QString BACKGROUND_COLOR_HINT          = "Select the storms background color.";
        static inline const QString BACKGROUND_OPACITY_HINT        = "Select the storms background color opacity.";

        static inline const QString APP_LANGUAGE_HINT              = "Select the language used in these dialogs.";

        static inline const QString ON_TOP_INSTEAD_HINT            = "Allow the storm to stay above other windows.";
        static inline const QString ALLOW_DESKTOP_DRAG_HINT        = "Enable or disable the ability to drag the storm window to a different desktop.";
        static inline const QString PREFERRED_DESKTOP_HINT         = "Enable the storm window on one preferred desktop, or on all of them.";
        static inline const QString DESKTOP_OVERHANG_HINT          = "Enable the storm window to extend beyond the right or bottom edges of the desktop.";

        static inline const QString SHOW_PIN_ON_WINDOW_HOVER_HINT  = "Enable or disable the Pin button icon when the storm window is hovered.";
        static inline const QString AUTOHIDE_CONTROLS_HINT         = "Enable or disable automatic hide of the storm window corner control buttons after a delay.";
        static inline const QString AUTOHIDE_DELAY_HINT            = "Select the delay for automatic hide of the storm window corner control buttons.";

        static inline const QString SHOW_SETTINGS_HINTS_HINT       = "Enable or disable display of these settings descriptions on mouse hover.";
        static inline const QString SHOW_ICONS_ON_BUTTONS_HINT     = "Enable or disable icon display in Dialog buttons.";

        // Settings property struct.
        struct SettingsProperty {
            QString group = "";
            QString name = "";
            QString hint = "";
            SettingsPropertyType valueType = NONE_VALUETYPE;
            QString initialValue = "";
            int rangeMinimum = numeric_limits<int>::min();
            int rangeMaximum = numeric_limits<int>::max();
        };

        // App configurables. Entries determine order of
        // appearance in Dialog.
        static inline const vector<SettingsProperty> PROPERTIES = {
            { .group = FROSTEDFLAKES_GROUP, .name = FROSTEDFLAKES_SIZE,
              .hint = FROSTEDFLAKES_SIZE_HINT,
              .valueType = SLIDER_VALUETYPE, .initialValue = "15",
              .rangeMinimum = 10, .rangeMaximum = 80
            },
            { .group = FROSTEDFLAKES_GROUP, .name = FROSTEDFLAKES_COUNT,
              .hint = FROSTEDFLAKES_COUNT_HINT,
              .valueType = SLIDER_VALUETYPE, .initialValue = "250",
              .rangeMinimum = 25, .rangeMaximum = 1000
            },
            { .group = FROSTEDFLAKES_GROUP, .name = "d0", .hint = "",
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = FROSTEDFLAKES_GROUP, .name = FROSTEDFLAKES_COLOR_ONE,
              .hint = FROSTEDFLAKES_COLOR_ONE_HINT,
              .valueType = COLOR_VALUETYPE, .initialValue = "#fff9d7",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            { .group = FROSTEDFLAKES_GROUP, .name = FROSTEDFLAKES_COLOR_TWO,
              .hint = FROSTEDFLAKES_COLOR_TWO_HINT,
              .valueType = COLOR_VALUETYPE, .initialValue = "#FFBF00",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            { .group = FROSTEDFLAKES_GROUP, .name = FROSTEDFLAKES_COLOR_THREE,
              .hint = FROSTEDFLAKES_COLOR_THREE_HINT,
              .valueType = COLOR_VALUETYPE, .initialValue = "#ff7b08",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            { .group = FROSTEDFLAKES_GROUP, .name = FROSTEDFLAKES_COLOR_FOUR,
              .hint = FROSTEDFLAKES_COLOR_FOUR_HINT,
              .valueType = COLOR_VALUETYPE, .initialValue = "#ff1170",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            { .group = FROSTEDFLAKES_GROUP, .name = "d1", .hint = "",
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = FROSTEDFLAKES_GROUP, .name = WIND_STRENGTH,
              .hint = WIND_STRENGTH_HINT,
              .valueType = SLIDER_VALUETYPE, .initialValue = "100",
              .rangeMinimum = 0, .rangeMaximum = 300
            },
            { .group = FROSTEDFLAKES_GROUP, .name = WIND_LENGTH,
              .hint = WIND_LENGTH_HINT,
              .valueType = SLIDER_VALUETYPE, .initialValue = "30",
              .rangeMinimum = 3, .rangeMaximum = 50
            },
            { .group = FROSTEDFLAKES_GROUP, .name = WIND_BURST_LENGTH,
              .hint = WIND_BURST_LENGTH_HINT,
              .valueType = SLIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = 5, .rangeMaximum = 10
            },
            { .group = FROSTEDFLAKES_GROUP, .name = "d2", .hint = "",
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GENERAL_GROUP, .name = BACKGROUND_COLOR,
              .hint = BACKGROUND_COLOR_HINT,
              .valueType = COLOR_VALUETYPE, .initialValue = "#0055ff",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            { .group = GENERAL_GROUP, .name = BACKGROUND_OPACITY,
              .hint = BACKGROUND_OPACITY_HINT,
              .valueType = SLIDER_VALUETYPE, .initialValue = "50",
              .rangeMinimum = 0, .rangeMaximum = 255
            },
            { .group = GENERAL_GROUP, .name = "d3", .hint = "",
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GENERAL_GROUP, .name = APP_LANGUAGE,
              .hint = APP_LANGUAGE_HINT,
              .valueType = COMBOBOX_VALUETYPE, .initialValue = "en",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            { .group = GENERAL_GROUP, .name = "d4", .hint = "",
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GENERAL_GROUP, .name = ON_TOP_INSTEAD,
              .hint = ON_TOP_INSTEAD_HINT,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            { .group = GENERAL_GROUP, .name = ALLOW_DESKTOP_DRAG,
              .hint = ALLOW_DESKTOP_DRAG_HINT,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            { .group = GENERAL_GROUP, .name = PREFERRED_DESKTOP,
              .hint = PREFERRED_DESKTOP_HINT,
              .valueType = SLIDER_VALUETYPE, .initialValue = "-1",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            { .group = GENERAL_GROUP, .name = DESKTOP_OVERHANG,
              .hint = DESKTOP_OVERHANG_HINT,
              .valueType = BOOL_VALUETYPE, .initialValue = "false",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            { .group = GENERAL_GROUP, .name = "d5", .hint = "",
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GENERAL_GROUP, .name = SHOW_PIN_ON_WINDOW_HOVER,
              .hint = SHOW_PIN_ON_WINDOW_HOVER_HINT,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            { .group = GENERAL_GROUP, .name = AUTOHIDE_CONTROLS,
              .hint = AUTOHIDE_CONTROLS_HINT,
              .valueType = BOOL_VALUETYPE, .initialValue = "false",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },
            { .group = GENERAL_GROUP, .name = AUTOHIDE_DELAY,
              .hint = AUTOHIDE_DELAY_HINT,
              .valueType = SLIDER_VALUETYPE, .initialValue = "4",
              .rangeMinimum = 1, .rangeMaximum = 9
            },
            { .group = GENERAL_GROUP, .name = "d6", .hint = "",
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GENERAL_GROUP, .name = SHOW_SETTINGS_HINTS,
              .hint = SHOW_SETTINGS_HINTS_HINT,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = std::numeric_limits<int>::min(),
              .rangeMaximum = std::numeric_limits<int>::max()
            },
            { .group = GENERAL_GROUP, .name = SHOW_ICONS_ON_BUTTONS,
              .hint = SHOW_ICONS_ON_BUTTONS_HINT,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = std::numeric_limits<int>::min(),
              .rangeMaximum = std::numeric_limits<int>::max()
            }
        };

        SettingsHelper();
        ~SettingsHelper();

        /**
         * Getters for window minimum Width & Height.
         */
        double getWindowMinimumWidth();
        double getWindowMinimumHeight();

        /**
         * Setters for window minimum Width & Height.
         */
        void setWindowMinimumWidth(const double width);
        void setWindowMinimumHeight(const double height);

        /**
         * Getters for window x & y, w & h.
         */
        double getWindowXPos();
        double getWindowYPos();
        double getWindowWidth();
        double getWindowHeight();

        /**
         * Setters for window x & y, w & h.
         */
        void setWindowXPos(const double xPos);
        void setWindowYPos(const double yPos);
        void setWindowWidth(const double width);
        void setWindowHeight(const double height);

        /**
         * Getters for canvas x & y, w & h.
         */
        double getCanvasXPos();
        double getCanvasYPos();
        double getCanvasWidth();
        double getCanvasHeight();

        /**
         * Setters for canvas x & y, w & h.
         */
        void setCanvasXPos(const double xPos);
        void setCanvasYPos(const double yPos);
        void setCanvasWidth(const double width);
        void setCanvasHeight(const double height);

        /**
         * Getters & setters of window config mode.
         */
        bool getConfigMode();

        void setConfigMode(const bool state);

        /**
         * Getters & setters for user configurable bool settings.
         */
        bool getBoolSetting(const QString setting);

        void setBoolSetting(const QString setting, const bool value);

        /**
         * Getters & setters for user configurable int settings.
         */
        int getIntSetting(const QString setting);

        void setIntSetting(const QString setting, const int value);

        /**
         * Getter for user configurable XRenderColor settings.
         */
        XRenderColor getColorSetting(const QString setting);

        /**
         * Getters & setters for user configurable string settings.
         */
        QString getStringSetting(const QString setting);

        void setStringSetting(const QString setting,
            const QString value);

        /**
         * Each runtime start we ensure Settings keys & default values
         * are flushed to .ini file for ConfigDialog to load & modify.
         */
        void ensureSettingsAreConfigurable();

        /**
         * Return the value type of a Setting by key.
         */
        SettingsPropertyType getSettingsValueType(const QString key);

        /**
         * Return the default value of a Setting by key.
         */
        QString getSettingsDefaultValue(const QString key);

        /**
         * Get a Minimum int value to load a UI widget.
         */
        int getSettingsIntRangeMinimum(const QString key);

        /**
         * Get a Maximum int value to load a UI widget.
         */
        int getSettingsIntRangeMaximum(const QString key);

        /**
         * Helper to return a new QSettings object for pref
         * access based on our appName.
         */
        QSettings* getQSettings();

    private:
        /**
         * Helper to return a QSettings filename from appName.
         */
        QString getQSettingsFile();

        /**
         * Members.
         */
        QString mSettingsApp = "";

        QSettings* mQSettings = nullptr;

};
