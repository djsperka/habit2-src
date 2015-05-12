/*
 * HStimulusOrderListItemDelegate.cpp
 *
 *  Created on: Apr 20, 2015
 *      Author: Oakeslab
 */


#include "HStimulusOrderListItemDelegate.h"
#include "HStimulusOrder.h"
#include <QLineEdit>
#include <QtDebug>
using namespace GUILib;
using namespace Habit;

QWidget *HStimulusOrderListItemDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const

{
	QLineEdit *editor = new QLineEdit(parent);
	editor->setPlaceholderText(QString("Enter label for this stimulus"));
	qDebug() << "HStimulusOrderListItemDelegate::createEditor";
    return editor;
}

void HStimulusOrderListItemDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
	QLineEdit* linedit = qobject_cast<QLineEdit *>(editor);
	QVariant vtext = index.data(Qt::DisplayRole);
	qDebug() << "HStimulusOrderListItemDelegate::setEditorData - model data is currently " << vtext.toString();
	linedit->setText(HStimulusOrder::getLabel(vtext.toString()));
	qDebug() << "HStimulusOrderListItemDelegate::setEditorData to " << HStimulusOrder::getLabel(vtext.toString());
}

void HStimulusOrderListItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
	QString stim = HStimulusOrder::getStim(index.data(Qt::DisplayRole).toString());
	qDebug() << "HStimulusOrderListItemDelegate::setModelData - stim is " << stim;
	QLineEdit* linedit = qobject_cast<QLineEdit *>(editor);
	qDebug() << "HStimulusOrderListItemDelegate::setModelData to " << HStimulusOrder::formatStimLabel(stim, linedit->text());
	model->setData(index, HStimulusOrder::formatStimLabel(stim, linedit->text()));
	qDebug() << "Model data is now " << index.data(Qt::DisplayRole).toString();
}




#if 0


void StarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    if (index.data().canConvert<StarRating>()) {
        StarRating starRating = qvariant_cast<StarRating>(index.data());

        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());

        starRating.paint(painter, option.rect, option.palette,
                         StarRating::ReadOnly);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
//! [0]
}

//! [1]
QSize StarDelegate::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    if (index.data().canConvert<StarRating>()) {
        StarRating starRating = qvariant_cast<StarRating>(index.data());
        return starRating.sizeHint();
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}
//! [1]

//! [2]
QWidget *StarDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const

{
    if (index.data().canConvert<StarRating>()) {
        StarEditor *editor = new StarEditor(parent);
        connect(editor, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return editor;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}
//! [2]

//! [3]
void StarDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
    if (index.data().canConvert<StarRating>()) {
        StarRating starRating = qvariant_cast<StarRating>(index.data());
        StarEditor *starEditor = qobject_cast<StarEditor *>(editor);
        starEditor->setStarRating(starRating);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}
//! [3]

//! [4]
void StarDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    if (index.data().canConvert<StarRating>()) {
        StarEditor *starEditor = qobject_cast<StarEditor *>(editor);
        model->setData(index, QVariant::fromValue(starEditor->starRating()));
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
//! [4]

//! [5]
void StarDelegate::commitAndCloseEditor()
{
    StarEditor *editor = qobject_cast<StarEditor *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
//! [5]


#endif



