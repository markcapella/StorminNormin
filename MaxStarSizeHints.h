
#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

struct MaxStarSizeHints : public QObject {

    public:
        MaxStarSizeHints(QSlider* slider) : QObject(slider),
            s(slider) {}

        bool eventFilter(QObject* o, QEvent* e) override {
            // Show the immediate millisecond the mouse
            // crosses into the slider.
            if (e->type() == QEvent::Enter) {
                if (!s->isSliderDown()) {
                    const int VALUE = s->value();
                    const QString TOOLTIP_TEXT = QString::number(VALUE) +
                        " " + I18N("pixels");
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT, s);
                }
                return false;
            }

            // Show also on interaction.
            if (e->type() == QEvent::ToolTip ||
                e->type() == QEvent::MouseMove) {
                if (!s->isSliderDown()) {
                    const int VALUE = s->value();
                    const QString TOOLTIP_TEXT = QString::number(VALUE) +
                        " " + I18N("pixels");
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT, s);
                }
            }

            return false;
        }

    private:
        /**
         * Members.
         */
        QSlider* s;

};

#pragma GCC diagnostic pop
