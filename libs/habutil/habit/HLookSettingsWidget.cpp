/*
 * HLookSettingsWidget.cpp
 *
 *  Created on: Feb 26, 2014
 *      Author: Oakeslab
 */

#include "HLookSettingsWidget.h"
using namespace GUILib;
using namespace Habit;

HLookSettingsWidget::HLookSettingsWidget()
: m_settings()
{
	createComponents();
	setValidators();
	makeConnections();
	doLayout();
	initialize();
}

HLookSettingsWidget::HLookSettingsWidget(const HLookSettings& settings)
: m_settings(settings)
{
	createComponents();
	setValidators();
	makeConnections();
	doLayout();
	initialize();
}

void HLookSettingsWidget::setHLookSettings(const HLookSettings& settings)
{
	m_settings = settings;
	initialize();
}

const Habit::HLookSettings& HLookSettingsWidget::getHLookSettings()
{
	m_settings.setMinLookTime(m_peditLook->text().toUInt());
	m_settings.setMinLookAwayTime(m_peditLookAway->text().toUInt());
	return m_settings;
}


void HLookSettingsWidget::initialize()
{
	m_peditLook->setText(QString("%1").arg(m_settings.getMinLookTime()));
	m_peditLookAway->setText(QString("%1").arg(m_settings.getMinLookAwayTime()));
}

void HLookSettingsWidget::createComponents()
{
	m_pgb = new QGroupBox("Look Settings");
	m_pgbLook = new QGroupBox("Minimum Look Time");
	m_pgbLookAway = new QGroupBox(tr("Minimum Look-Away Time"));
	m_peditLook = new QLineEdit();
	m_peditLookAway = new QLineEdit();
}

void HLookSettingsWidget::setValidators()
{
	m_peditLook->setValidator(new QIntValidator(0, 9999999, this));
	m_peditLookAway->setValidator(new QIntValidator(0, 9999999, this));
}

void HLookSettingsWidget::makeConnections()
{
	return;
}

void packBoxWithLabel(QGroupBox* box, QWidget* w, QLabel* label)
{
	QHBoxLayout* hb = new QHBoxLayout;
	hb->addWidget(w);
	hb->addWidget(label);
	box->setLayout(hb);
}

void HLookSettingsWidget::doLayout()
{
#ifdef USE_ORIGINAL
	packBoxWithLabel(m_pgbLook, m_peditLook, new QLabel("ms"));
	packBoxWithLabel(m_pgbLookAway, m_peditLookAway, new QLabel("ms"));
	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(m_pgbLook);
	layout->addWidget(m_pgbLookAway);
	m_pgb->setLayout(layout);

	// this widget object will have a single layout with the main group box in it
	QVBoxLayout* main = new QVBoxLayout();
	main->addWidget(m_pgb);
	setLayout(main);

#else
	QLabel* pMLT = new QLabel("Minimum looking time");
	QLabel* pMS1 = new QLabel("ms");
	QLabel* pMLAT = new QLabel("Minimum looking-away time");
	QLabel* pMS2 = new QLabel("ms");
	QFont f;
	f.setPointSize(10);
	pMLT->setFont(f);
    pMS1->setFont(f);
	pMLAT->setFont(f);
	pMS2->setFont(f);

	QHBoxLayout *hboxMLT = new QHBoxLayout();
	hboxMLT->addWidget(pMLT);
	hboxMLT->addWidget(m_peditLook);
	hboxMLT->addWidget(pMS1);

	QHBoxLayout *hboxMLAT = new QHBoxLayout();
	hboxMLAT->addWidget(pMLAT);
	hboxMLAT->addWidget(m_peditLookAway);
	hboxMLAT->addWidget(pMS2);

//	QVBoxLayout* boxGB = new QVBoxLayout();
	QHBoxLayout* boxGB = new QHBoxLayout();
	boxGB->addLayout(hboxMLT);
	boxGB->addLayout(hboxMLAT);
	m_pgb->setLayout(boxGB);

	// this widget object will have a single layout with the main group box in it
	QVBoxLayout* main = new QVBoxLayout();
	main->addWidget(m_pgb);
	setLayout(main);

	#endif
}
