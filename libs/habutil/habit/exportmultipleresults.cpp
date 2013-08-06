#include "exportmultipleresults.h"

#ifndef USING_QT5
#include <QtGui>
#else
#include <QtWidgets>
#endif

namespace GUILib {
ExportMultipleResults::ExportMultipleResults(QWidget* w)
	: QDialog(w)
{
	createComponents();
	makeConnections();
	doLayout();
}

ExportMultipleResults::~ExportMultipleResults()
{
}

void ExportMultipleResults::createComponents()
{
	headerText_ = new QLabel(tr("Please choose a list of results files "
								"to export to tab-delimited text."));
	addFilesButton_ = new QPushButton(tr("Add File(s) to List..."));
	removeFilesButton_ = new QPushButton(tr("Remove Selected From List"));
	removeFilesButton_->setEnabled(false);
	clearListButton_ = new QPushButton(tr("Clear List"));
	clearListButton_->setEnabled(false);
	moveUpButton_ = new QPushButton(tr("Move Selected Up"));
	moveUpButton_->setEnabled(false);
	moveDownButton_ = new QPushButton(tr("Move Selected Down"));
	moveDownButton_->setEnabled(false);
	list_ = new QListWidget();
	cancelButton_ = new QPushButton(tr("Cancel"));
	exportButton_ = new QPushButton(tr("Export"));

}

void ExportMultipleResults::makeConnections()
{
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(close()));
	connect(addFilesButton_, SIGNAL(clicked()), this, SLOT(onAddFiles()));
	connect(removeFilesButton_, SIGNAL(clicked()), this, SLOT(onRemoveFiles()));
	connect(clearListButton_, SIGNAL(clicked()), this, SLOT(onClearList()));
	connect(moveUpButton_, SIGNAL(clicked()), this, SLOT(onMoveUp()));
	connect(moveDownButton_, SIGNAL(clicked()), this, SLOT(onMoveDown()));
	connect(list_, SIGNAL(itemSelectionChanged () ), this, SLOT(onSelectionChanged()));
	connect(exportButton_, SIGNAL(clicked()), this, SLOT(accept()));
}

void ExportMultipleResults::doLayout()
{
	mainLayout_ = new QVBoxLayout(this);
	mainLayout_->addWidget(headerText_, 0, Qt::AlignCenter);
	QHBoxLayout* hbox = new QHBoxLayout();
	mainLayout_->addLayout(hbox);
	QVBoxLayout* leftLayout = new QVBoxLayout();
	QVBoxLayout* rightLayout = new QVBoxLayout();
	hbox->addLayout(leftLayout);
	hbox->addLayout(rightLayout);
	leftLayout->addWidget(addFilesButton_);
	leftLayout->addWidget(removeFilesButton_);
	leftLayout->addWidget(clearListButton_);
	leftLayout->addSpacing(30);
	leftLayout->addWidget(moveUpButton_);
	leftLayout->addWidget(moveDownButton_);
	rightLayout->addWidget(list_);
	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(cancelButton_);
	buttonsLayout->addWidget(exportButton_);
	mainLayout_->addLayout(buttonsLayout);
}

void ExportMultipleResults::onAddFiles()
{
	QStringList files = QFileDialog::getOpenFileNames(
                         this,
                         "Select one or more files to add to list",
                         "",
                         "Experiment Results File (*.res)");
	for (int i = 0; i < files.count(); i++)
	{
		QListWidgetItem * item = new QListWidgetItem(list_);
		item->setData(Qt::UserRole, files[i]);
		item->setText(QFileInfo(files[i]).fileName());
		list_->addItem(item);
	}
	if (list_->count() > 0)
		clearListButton_->setEnabled(true);
	else
		clearListButton_->setEnabled(false);
}

void ExportMultipleResults::onRemoveFiles()
{
	int index = list_->currentRow();
	if (index >= 0 && index < list_->count())
	{
		QListWidgetItem* item = list_->takeItem(index);
		delete item;
	}
	if (list_->count() > 0)
		clearListButton_->setEnabled(true);
	else
		clearListButton_->setEnabled(false);
}

void ExportMultipleResults::onClearList()
{
	list_->clear();
	if (list_->count() > 0)
		clearListButton_->setEnabled(true);
	else
		clearListButton_->setEnabled(false);
}

void ExportMultipleResults::onMoveUp()
{
	int index = list_->currentRow();
	if (index > 0)
	{
		QListWidgetItem* itemA = list_->takeItem(index);
		QListWidgetItem* itemB = list_->takeItem(index-1);
		list_->insertItem(index-1, itemA);
		list_->insertItem(index, itemB);
		list_->setCurrentRow(index-1);
	}
}

void ExportMultipleResults::onMoveDown()
{
	int index = list_->currentRow();
	if ( index >= 0 && index < list_->count()-1)
	{
		QListWidgetItem* itemA = list_->takeItem(index);
		list_->insertItem(index+1, itemA);
		
		list_->setCurrentRow(index+1);
	}
}

void ExportMultipleResults::onSelectionChanged()
{
	
	QList<QListWidgetItem*> lst = list_->selectedItems();
	if (lst.count() == 0)
	{
		removeFilesButton_->setEnabled(false);
		moveUpButton_->setEnabled(false);
		moveDownButton_->setEnabled(false);
	}
	else
	{
		removeFilesButton_->setEnabled(true);
		int index = list_->currentRow();
		if (index < 0 && index >= list_->count())
		{
			moveUpButton_->setEnabled(false);
			moveDownButton_->setEnabled(false);
		}
		if (index > 0 )
		{
			moveUpButton_->setEnabled(true);
		}
		if (index < list_->count()-1)
		{
			moveDownButton_->setEnabled(true);
		}

	}
}

QStringList ExportMultipleResults::getFiles()
{
	QStringList listFiles;
	for (int i =0; i < list_->count(); i++)
	{
		listFiles.append(list_->item(i)->data(Qt::UserRole).toString());
	}
	return listFiles;
	
}

}
