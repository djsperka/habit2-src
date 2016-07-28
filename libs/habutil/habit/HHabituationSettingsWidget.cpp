/*
 * HHabituationSettingsWidget.cpp
 *
 *  Created on: Jul 26, 2016
 *      Author: dan
 */

#include <HHabituationSettingsWidget.h>
#include <QtDebug>

namespace GUILib {

HHabituationSettingsWidget::HHabituationSettingsWidget(const Habit::HabituationSettings& habituationSettings, QWidget *parent)
: QWidget(parent)
, m_settings(habituationSettings)
{
	m_pTypeWidget = new HHabituationTypeWidget(m_settings.getHabituationType(), this);
	m_pCriteriaWidget = new HHabituationCriteriaWidget(m_settings.getNTrials(), m_settings.getCriterionSettings(), m_settings.getTotalLookLengthToEnd(), this);
	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(m_pTypeWidget);
	vbox->addWidget(m_pCriteriaWidget);
	setLayout(vbox);

	// Initialize page for  criteria widget. Type widget is initialized automatically.
	qDebug() << "m_pCriteriaWidget->setCurrentIndex(" << m_settings.getHabituationType().number() << ")" << m_settings.getHabituationType().name();
	m_pCriteriaWidget->setCurrentIndex(m_settings.getHabituationType().number());
	connect(m_pTypeWidget, SIGNAL(currentIndexChanged(int)), m_pCriteriaWidget, SLOT(setCurrentIndex(int)));
}

HHabituationSettingsWidget::~HHabituationSettingsWidget() {
	// TODO Auto-generated destructor stub
}

Habit::HabituationSettings HHabituationSettingsWidget::getHabituationSettings()
{
	Habit::HabituationSettings hs;
	hs.setId(m_settings.getId());
	hs.setHabituationType(m_pTypeWidget->getHabituationType());
	hs.setNTrials(m_pCriteriaWidget->getNTrials());
	hs.setCriterionSettings(m_pCriteriaWidget->getCriterionSettings());
	hs.setTotalLookLengthToEnd(m_pCriteriaWidget->getTotalLookLength());
	return hs;
}


} /* namespace GUILib */
