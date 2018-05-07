/*
 * ExperimentNameDialog.cpp
 *
 *  Created on: Jan 31, 2017
 *      Author: dan
 */

#include "HExperimentNameDialog.h"
#include <QRegExpValidator>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QtDebug>

namespace GUILib {

HExperimentNameDialog::HExperimentNameDialog(const QStringList& list, const QString& defaultName, QWidget *parent)
: QDialog(parent)
{
	m_pvalidator = new HExperimentListValidator(list, this);
	m_plist = new HExperimentListWidget(this, false, true);
	m_ple = new QLineEdit;

	qDebug() << "have default name " << defaultName;
	m_ple->setText(defaultName);
	m_ple->setValidator(m_pvalidator);

	// label and line edit for name
	QHBoxLayout *hbox1 = new QHBoxLayout;
	hbox1->addWidget(new QLabel("Name:"));
	hbox1->addWidget(m_ple);

	// checkbox to use template
	m_pcheck = new QCheckBox("Use a template experiment");
	m_pcheck->setToolTip("Check to make a copy of a template experiment");

	// vbox for hbox, checkbox and list
	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addLayout(hbox1);
	vbox->addWidget(m_pcheck);
	vbox->addWidget(m_plist);

	m_pbbox = new QDialogButtonBox(
			QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
			Qt::Vertical);
	connect(m_pbbox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_pbbox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_pcheck, SIGNAL(clicked(bool)), m_plist, SLOT(setEnabled(bool)));
    connect(m_ple, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));

    m_pcheck->setChecked(false);
    m_plist->setEnabled(false);

    // hbox for vbox above and buttons, side by side
    QHBoxLayout *hbox2 = new QHBoxLayout;
    hbox2->addLayout(vbox);
    hbox2->addWidget(m_pbbox);
	this->setLayout(hbox2);

	// make sure ok button correctly en/disabled
	textChanged(defaultName);
}

HExperimentNameDialog::~HExperimentNameDialog()
{
}

void HExperimentNameDialog::textChanged(const QString& text)
{
	QPushButton *button = m_pbbox->button(QDialogButtonBox::Ok);
	if (!button) return;
	button->setEnabled(text.size() > 0);
}


QString HExperimentNameDialog::getNewValue()
{
	return m_ple->text();
}

bool HExperimentNameDialog::isTemplateChosen()
{
	return m_pcheck->isChecked();
}

QString HExperimentNameDialog::getTemplateChosen()
{
	return m_plist->selectedExperiment();
}


} /* namespace GUILib */
