/*
 * HOrderSettingsWidget.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: Oakeslab
 */

#include "HOrderSettingsWidget.h"
#include "HTypes.h"

using namespace GUILib;

HOrderSettingsWidget::HOrderSettingsWidget(const Habit::StimuliSettings& stimuli)
{
	// create stuff
	m_pStimuliTable = new HabituationStimuliTable(stimuli);
	m_plineeditOrder = new HStimOrderLineEdit(stimuli.getStimuli().size());
//	m_plineeditOrder->setValidator(new HStimOrderValidator(stimuli.getStimuli().size()));
	m_plabelOrder = new QLabel("Stimuli Order - stimuli number(s) separated by spaces, commas, or ""|""");
	m_pcheckboxRandomize = new QCheckBox("Randomize Order");
	m_pcomboRandomizationType = new QComboBox();
	m_pcomboRandomizationType->addItem(HRandomizationType::Simple.label());
	m_pcomboRandomizationType->addItem(HRandomizationType::Block.label());
	m_plabelRandomizationType = new QLabel("Randomization Type:");

	// connections
	connect(m_pcheckboxRandomize, SIGNAL(toggled(bool)), m_pcomboRandomizationType, SLOT(setEnabled(bool)));
	m_pcomboRandomizationType->setEnabled(false);

	// layout
	QVBoxLayout* v1 = new QVBoxLayout();
	v1->addWidget(m_pStimuliTable);
	v1->addWidget(m_plabelOrder);
	v1->addWidget(m_plineeditOrder);

	QHBoxLayout* h1 = new QHBoxLayout();
	h1->addWidget(m_pcheckboxRandomize);
	h1->addWidget(m_pcomboRandomizationType);
	v1->addLayout(h1);

	setLayout(v1);
}

Habit::HOrderSettings HOrderSettingsWidget::getOrderSettings()
{
	Habit::HOrderSettings order;
	QList<int> list;
	order.setIsRandomized(m_pcheckboxRandomize->isChecked());
	order.setRandomizationType(getRandomizationType(m_pcomboRandomizationType->currentIndex()));
	m_plineeditOrder->getList(list);
	order.setList(list);
	return order;
}

void HOrderSettingsWidget::initialize(const Habit::StimuliSettings& stimuli)
{
	m_pStimuliTable->initializeTable(stimuli);
	m_plineeditOrder->setText("");
	m_plineeditOrder->setMax(stimuli.getStimuli().size());
	m_pcomboRandomizationType->setCurrentIndex(0);
	m_pcheckboxRandomize->setChecked(false);
}


QRegExp HStimOrderValidator::m_reDelimDigit("[ ,|]*(\\d+)");
QRegExp HStimOrderValidator::m_reFull("([ ,|]*(\\d+))*");

// Check input to make sure all digits are within range. If any fall outside of range
// return false, otherwise return true.
// If the list is supplied, append the parsed integers to that list.
// Assumes that the input has already passed m_reFull!
bool HStimOrderValidator::getList(QString input, int min, int max, QList<int>* plist = 0)
{
	bool b = true;
	int pos = 0;
	while (pos >= 0 && b)
	{
		pos = HStimOrderValidator::m_reDelimDigit.indexIn(input, pos);
		if (pos >= 0)
		{
			int i;
			i = HStimOrderValidator::m_reDelimDigit.cap(1).toInt();
			if (i < min || i > max) b = false;
			else if (plist) plist->append(i);
			pos += HStimOrderValidator::m_reDelimDigit.matchedLength();
		}
	}
	return b;
}

void HStimOrderLineEdit::getList(QList<int>& list) const
{
	HStimOrderValidator::getList(text(), m_min, m_max, &list);
}

QValidator::State HStimOrderValidator::validate(QString& input, int& position) const
{
	QValidator::State state = QRegExpValidator::validate(input, position);
	if (state != QValidator::Invalid)
	{
		if (!HStimOrderValidator::getList(input, m_min, m_max)) state = QValidator::Invalid;
	}
	return state;
};


