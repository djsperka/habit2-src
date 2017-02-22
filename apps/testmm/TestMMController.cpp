/*
 * TestMMController.cpp
 *
 *  Created on: Feb 22, 2017
 *      Author: dan
 */

#include "TestMMController.h"
#include <QListView>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QtDebug>
#include <QModelIndex>
#include <QList>

TestMMController::TestMMController(const QStringList& names, QWidget *parent)
: QDialog(parent)
{
	QVBoxLayout *vbox = new QVBoxLayout;

	QStringListModel *pmodel = new QStringListModel(names);
	m_list = new QListView;
	m_list->setModel(pmodel);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal);
	m_playButton = new QPushButton("Play");
	m_stopButton = new QPushButton("Stop");

    buttonBox->addButton(m_playButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_stopButton, QDialogButtonBox::ActionRole);

    connect(m_playButton, SIGNAL(clicked()), this, SLOT(playClicked()));
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(stopClicked()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    vbox->addWidget(m_list);
    vbox->addWidget(buttonBox);
    setLayout(vbox);
}

void TestMMController::playClicked()
{
	QList<QModelIndex> selected = m_list->selectionModel()->selectedIndexes();
	QListIterator<QModelIndex> it(selected);
	if (it.hasNext())
	{
		// expected to have just 1
		emit playItem(it.next().row());
		// shouldn't have any more - ignore them all if there  are any
	}

}

void TestMMController::stopClicked()
{
	emit stopItem();
}
