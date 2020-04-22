/*
 * GstspControlDialog.cpp
 *
 *  Created on: Dec 18, 2019
 *      Author: dan
 */


#include "GstspControlDialog.h"
#include "ui_gstspcontroldialog.h"
#include <QtDebug>
#include <QPushButton>

GstspControlDialog::GstspControlDialog(const QStringList& list, const QList<hmm::HMMStimID>& idlist, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gstspcontroldialog),
	idList(idlist)
{
    ui->setupUi(this);

    // Create model
    model = new QStringListModel(this);

    // Populate our model
    model->setStringList(list);

    // Glue model and view together
    ui->listView->setModel(model);

    // Add preroll button
    QPushButton *button = ui->buttonBox->addButton("Preroll", QDialogButtonBox::ActionRole);
    connect(button, &QPushButton::clicked, this, &GstspControlDialog::onPrerollClicked);
    QPushButton *button1 = ui->buttonBox->addButton("Play", QDialogButtonBox::ActionRole);
    connect(button1, &QPushButton::clicked, this, &GstspControlDialog::onPlayClicked);
    QPushButton *button2 = ui->buttonBox->addButton("Dump", QDialogButtonBox::ActionRole);
    connect(button2, &QPushButton::clicked, this, &GstspControlDialog::onDumpClicked);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

}

GstspControlDialog::~GstspControlDialog()
{

}

void GstspControlDialog::onDumpClicked()
{
	emit dump();
}

void GstspControlDialog::onPrerollClicked()
{
	QModelIndexList selected = ui->listView->selectionModel()->selectedIndexes();
	qDebug() << "preroll clicked";
	if (selected.size() > 0)
	{
		for (auto index: selected)
		{
			qDebug() << "row " << index.row() << " id " << idList.at(index.row()).c_str();
			emit preroll(QString(idList.at(index.row()).c_str()));
		}
	}
	else
	{
		emit dump();
	}
}

void GstspControlDialog::onPlayClicked()
{
	QModelIndexList selected = ui->listView->selectionModel()->selectedIndexes();
	qDebug() << "play clicked";
	if (selected.size() > 0)
	{
		for (auto index: selected)
		{
			bool b;
			unsigned int iid = ui->lineEditIID->text().toUInt(&b);
			if (b)
			{
				qDebug() << "emit play(" << iid << ")";
				emit play(iid);
			}
		}
	}
	else
	{
		emit dump();
	}
}
