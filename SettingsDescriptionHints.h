
#pragma once

/**
 * Tooltip hints for Settings that explain their function.
 */
class SettingsDescriptionHints : public QObject {

    public:
        /**
         * Constructor.
         */
        SettingsDescriptionHints(ConfigDialog* configDialog,
            const QString english, QObject* parent = nullptr) :
            mEnglishText(english), QObject(parent) {

            mConfigDialog = configDialog;
        }

    protected:
        /**
         * Catch events to trigger hover info.
         */
        bool eventFilter(QObject* setting, QEvent* event) override {
            const bool SHOW_SETTINGS_HINTS = mSettingsHelper->
                getBoolSetting(SettingsHelper::SHOW_SETTINGS_HINTS);

            if (SHOW_SETTINGS_HINTS) {
                if (event->type() == QEvent::Enter) {
                    const QString TRANSLATED_ENGLISH = I18N(mEnglishText);
                    QToolTip::showText(QCursor::pos(), TRANSLATED_ENGLISH,
                        qobject_cast<QWidget*>(setting));
                }
                else if (event->type() == QEvent::Leave) {
                    QToolTip::hideText();
                }
            }

            return QObject::eventFilter(setting, event);
        }

    private:
        // Members.
        ConfigDialog* mConfigDialog = nullptr;
        QString mEnglishText;

};
