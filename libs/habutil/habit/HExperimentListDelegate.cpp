#include "HExperimentListDelegate.h"
#include "HExperimentListValidator.h"
#include <QLineEdit>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
#include <QString>
#include <QValidator>
#include <QStringListModel>
#include <iostream>

GUILib::HExperimentListDelegate::HExperimentListDelegate(QObject *parent)
:QItemDelegate(parent)
{
}



QWidget *GUILib::HExperimentListDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
	QLineEdit* editor = new QLineEdit(parent);
	const QStringListModel *pmodel = static_cast<const QStringListModel*>(index.model());
	editor->setValidator(new GUILib::HExperimentListValidator(pmodel->stringList(), editor));
	return editor;
}



void GUILib::HExperimentListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QLineEdit *lineedit = static_cast<QLineEdit*>(editor);
	QString value = index.model()->data(index, Qt::EditRole).toString();
	lineedit->setText(value);
}

void GUILib::HExperimentListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  QLineEdit *lineedit = static_cast<QLineEdit*>(editor);
  model->setData(index, lineedit->text(), Qt::EditRole);
}

void GUILib::HExperimentListDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
  editor->setGeometry(option.rect);
}

#if 0
void GUILib::HExperimentListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QStyleOptionViewItemV4 myOption = option;
  QString text = Items[index.row()].c_str();

  myOption.text = text;

  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
}
#endif
