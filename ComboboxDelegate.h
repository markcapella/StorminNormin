
#pragma once

#include <QStyledItemDelegate>
#include <QPainter>


/**
 * Styling class so QDropdown hovered item is light blue,
 *
 * Default light gray is almost invisible.
 */
class ComboboxDelegate : public QStyledItemDelegate {

    public:
        using QStyledItemDelegate::QStyledItemDelegate;

        void paint(QPainter* painter, const QStyleOptionViewItem& option,
            const QModelIndex& index) const override {

            QStyleOptionViewItem opt = option;
            initStyleOption(&opt, index);

            if (opt.state & QStyle::State_MouseOver) {
                opt.state |= QStyle::State_Selected;
                opt.palette.setColor(QPalette::Highlight, QColor("#429afd"));
                opt.palette.setColor(QPalette::HighlightedText, Qt::white);
            } else {
                opt.state &= ~QStyle::State_Selected;
                opt.palette.setColor(QPalette::Text,
                    opt.palette.color(QPalette::WindowText));
            }

            QStyledItemDelegate::paint(painter, opt, index);
        }
};
