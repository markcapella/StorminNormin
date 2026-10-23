
#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QSettings>
#include <QTabWidget>
#include <QVBoxLayout>

// x11 libraries.
#include <X11/Xutil.h>

// Application libraries.
#include "X11Types.h"

/**
 * Simple class to represent a ConfigDialog.
 */
class ConfigDialog : public QDialog {
    Q_OBJECT

    public:
        static inline const int CONFIG_DIALOG_WIDTH = 575;
        static inline const int CONFIG_DIALOG_HEIGHT = 500;

        static inline const int FORM_TOP_BOTTOM_SPACING = 15;
        static inline const int FORM_LAYOUT_ROW_SPACING = 8;

        // Constructor.
        explicit ConfigDialog(QWidget* parent = nullptr);

        /**
         * Gettters / Setters for window.
         */
        Window getWindow() const {
            return mWindow;
        }
        void setWindow(const Window window) {
            mWindow = window;
        }

        /**
         * Translate Settings to desired language for display.
         */
        void translateConfigDialog();

        /**
         * Load UI form with values from .Ini.
         */
        void loadConfigDialog();

    private:
        /**
         * Load dialog with DEFAULT settings values.
         */
        void loadConfigDialogWithDefaults();

    public:
        /**
         * Update any runtime dialog controls, range settings, etc.
         */
        void updateConfigDialog();

    private:
        /**
         * Init per-tab Form-Row Helper getters and setters. These
         * get & set Settings by row in each of the Dialog tab-forms.
         */
        void initFormRowGetterSetters();

        /**
         * This helper called from createConfigDialog() adds a setting
         * Edit Widget or a Divider to the form that contains the row #.
         */
        void addFormRow(const int row, const QString key,
            QWidget* widget);

        /**
         * This helper called from createConfigDialog() is used to
         * provide translated floating hints for settings in each row.
         */
        QWidget* getLabelWidgetFromFormRow(QWidget* widget,
            const int row);

        /**
         * This helper called from loadConfigDialog(),
         * loadConfigDialogWithDefaults() & acceptConfigDialog()
         * retrieves an Edit Widget from the form that contains the
         * row #, for inspection and modification.
         */
        QWidget* getFieldRoleWidgetFromFormRow(const int row);

        /**
         * This helper called from translateConfigDialog() is used to
         * change displayed text of an Edit Widget from the form that
         * contains the row # when user changes preferred app language.
         */
        QLayoutItem* getLabelRoleLayoutFromFormRow(const int row);

        /**
         * Build the UI form layout.
         */
        void createConfigDialog();

        /**
         * Called on Ok button of Dialog clicked.
         */
        void okConfigDialog();

        /**
         * Called on Accept button of Dialog clicked.
         */
        void acceptConfigDialog();

        /**
         * Send an event to the X11 thread telling it to update
         * with new user config settings.
         */
        void sendConfigDialogUpdatedEvent(
            const bool canvasNeedsRedraw);

        /**
         * Show this apps "About" dialog.
         */
        void showAboutDialog();

        /**
         * Members.
         */
        Window mWindow = X11_NONE;

        QVBoxLayout* mMainLayout = nullptr;

        QTabWidget* mTabWidget;

        QFormLayout* mSnowFlakesForm = nullptr;
        int mSnowFlakesRowCount = 0;

        QFormLayout* mAdvancedForm = nullptr;
        int mAdvancedFormRowCount = 0;

        QHBoxLayout* mButtonLayout = nullptr;

        QPushButton* mResetButton = nullptr;
        QPushButton* mAboutButton = nullptr;
        QPushButton* mOkButton = nullptr;
        QPushButton* mApplyButton = nullptr;
        QPushButton* mCancelButton = nullptr;

        QList<bool> mSettingChanges;

        QDialog* mAboutDialog = nullptr;

};
