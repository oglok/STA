/* 
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.
 
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.
 
 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org. 
 */

/*
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Annalisa Riccardi  -------------------------------------------------
 ------------------ E-mail: (nina1983@gmail.com) ----------------------------
 */

#include <QItemDelegate>
#include <QtGui/QTreeWidget>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QStyle>
#include <QtGui/QPainter>
#include <QLineEdit>

class SheetDelegate: public QItemDelegate
{
public:
    SheetDelegate(QTreeWidget* widget);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const;

private:
    QTreeWidget *m_widget;
};


SheetDelegate::SheetDelegate(QTreeWidget* widget) : QItemDelegate(widget),
      m_widget(widget)
{
}

void SheetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QAbstractItemModel *model = index.model();
    Q_ASSERT(model);

    if (!model->parent(index).isValid()) {
        // this is a top-level item.
        QStyleOptionButton buttonOption;

        buttonOption.state = option.state;
#ifdef Q_WS_MAC
        buttonOption.state |= QStyle::State_Raised;
#endif

        buttonOption.rect = option.rect;
        buttonOption.palette = option.palette;
        buttonOption.features = QStyleOptionButton::None;
        m_widget->style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter, m_widget);

        QStyleOption branchOption;
        static const int i = 9; 
        QRect r = option.rect;
        branchOption.rect = QRect(r.left() + i/2, r.top() + (r.height() - i)/2, i, i);
        branchOption.palette = option.palette;
        branchOption.state = QStyle::State_Children;

        if (m_widget->isExpanded(index))
            branchOption.state |= QStyle::State_Open;

        m_widget->style()->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, painter, m_widget);

        // draw text
        QRect textrect = QRect(r.left() + i*2, r.top(), r.width() - ((5*i)/2), r.height());
        QString text = elidedText(option.fontMetrics, textrect.width(), Qt::ElideMiddle, model->data(index, Qt::DisplayRole).toString());
        m_widget->style()->drawItemText(painter, textrect, Qt::AlignCenter,
            option.palette, m_widget->isEnabled(), text);

    } else {
        QItemDelegate::paint(painter, option, index);
    }
}

QSize SheetDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
    QSize sz = QItemDelegate::sizeHint(opt, index);
    return sz;
}
