/*
 * HPhaseListWidget.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: dan
 */

#include "HPhaseListWidget.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QStringListModel>
#include <QLabel>

using namespace GUILib;

GUILib::HPhaseListWidget::HPhaseListWidget(const QStringList& phaseNames, QWidget* parent)
: QWidget(parent)
{
	components();
	m_pListView->setModel(new QStringListModel(phaseNames));
	connections();
}

GUILib::HPhaseListWidget::~HPhaseListWidget()
{
}

void  GUILib::HPhaseListWidget::components()
{
	m_pListView = new QListView(this);

	m_pPhaseToolBar = new QToolBar();
	m_actionNewPhase = new QAction(QIcon(":/resources/plus.png"), "Add new phase", this);
	m_actionDelPhase = new QAction(QIcon(":/resources/delete.png"), "Delete selected phase", this);
	m_pPhaseToolBar->addAction(m_actionNewPhase);
	connect(m_actionNewPhase, SIGNAL(triggered()), this, SLOT(addPhase()));
	m_pPhaseToolBar->addAction(m_actionDelPhase);
	connect(m_actionDelPhase, SIGNAL(triggered()), this, SLOT(delPhase()));


	QVBoxLayout *pvbox = new QVBoxLayout;
	pvbox->addWidget(new QLabel("Phases"));
	pvbox->addWidget(m_pListView);
	pvbox->addWidget(m_pPhaseToolBar);
	setLayout(pvbox);
}

void HPhaseListWidget::connections()
{

}

void GUILib::HPhaseListWidget::addPhase()
{
	QMessageBox::warning(this, "AddPhase", QString("Add Phase"));
}

void GUILib::HPhaseListWidget::delPhase()
{
	QMessageBox::warning(this, "DelPhase", QString("Del Phase"));

	// delete this phase?
}
