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

TestMMController::TestMMController(const QStringList& names, QWidget *screens, QWidget *parent)
: QDialog(parent)
, m_stringList(names)
{
	QVBoxLayout *vbox = new QVBoxLayout;
	QStringListModel *pmodel = new QStringListModel(m_stringList);
	m_list = new QListView;
	m_list->setModel(pmodel);
	m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
	connect(m_list, SIGNAL(clicked(const QModelIndex&)), this, SLOT(itemClicked(const QModelIndex&)));


////	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal);
////	m_playButton = new QPushButton("Play");
////	m_stopButton = new QPushButton("Stop");
////	m_nextButton = new QPushButton("Next");
////	m_replayButton = new QPushButton("Replay");
//
//    buttonBox->addButton(m_playButton, QDialogButtonBox::ActionRole);
//    buttonBox->addButton(m_nextButton, QDialogButtonBox::ActionRole);
//    buttonBox->addButton(m_replayButton, QDialogButtonBox::ActionRole);
//    buttonBox->addButton(m_stopButton, QDialogButtonBox::ActionRole);
//
//    connect(m_playButton, SIGNAL(clicked()), this, SLOT(playClicked()));
//    connect(m_nextButton, SIGNAL(clicked()), this, SLOT(nextClicked()));
//    connect(m_replayButton, SIGNAL(clicked()), this, SLOT(replayClicked()));
//    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(stopClicked()));
//    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    vbox->addWidget(m_list);
    //vbox->addWidget(buttonBox);
    if (screens) vbox->addWidget(screens);
    setLayout(vbox);

    m_index = 0;
//    m_playButton->setEnabled(true);
//    m_stopButton->setEnabled(false);
//    m_nextButton->setEnabled(false);
//    m_replayButton->setEnabled(false);
}

//void TestMMController::playClicked()
//{
//	QList<QModelIndex> selected = m_list->selectionModel()->selectedIndexes();
//	QListIterator<QModelIndex> it(selected);
//	qDebug() << "playClicked";
//	if (it.hasNext())
//	{
//		// expected to have just 1
//		emit playItem(it.next().row());
//		// shouldn't have any more - ignore them all if there  are any
//	}
//
//}

void TestMMController::itemClicked(const QModelIndex& index)
{
	qDebug() << "itemClicked row " << index.row();
	//emit playItem(index.row());
}

void TestMMController::playClicked()
{
	m_index = 0;
	emit playItem(0);
	m_playButton->setEnabled(false);
	m_nextButton->setEnabled(true);
	m_stopButton->setEnabled(true);
}

void TestMMController::nextClicked()
{
	m_index++;
	if (m_index == m_stringList.size()-1)
	{
		m_nextButton->setEnabled(false);
	}
	emit playItem(m_index);
}

void TestMMController::replayClicked()
{
	qDebug() << "replay clicked";
}

void TestMMController::stopClicked()
{
	m_index = 0;
	emit playItem(0);
	m_playButton->setEnabled(true);
}

void TestMMController::stimStarted(int index)
{
	qDebug() << "started key " << index;
	QModelIndex i = m_list->model()->index(m_index, 0);
	m_list->setCurrentIndex(i);
}

