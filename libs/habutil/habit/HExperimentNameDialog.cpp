/*
 * ExperimentNameDialog.cpp
 *
 *  Created on: Jan 31, 2017
 *      Author: dan
 */

#include "HExperimentNameDialog.h"
#include <QDialogButtonBox>
#include <QRegExpValidator>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>

namespace GUILib {

HExperimentNameDialog::HExperimentNameDialog(const QStringList& list, const QString& defaultName, QWidget *parent)
: QDialog(parent)
, m_pvalidator(new HExperimentListValidator(list, parent))
, m_ple(new QLineEdit)
{
	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget(new QLabel(tr("Experiment name:")));

	m_ple->setPlaceholderText(defaultName);
	m_ple->setValidator(m_pvalidator);
	vbox->addWidget(m_ple);
	QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
		| QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	vbox->addWidget(buttonBox);
	this->setLayout(vbox);
}

HExperimentNameDialog::~HExperimentNameDialog()
{
}

QString HExperimentNameDialog::getNewValue()
{
	return m_ple->text();
}

} /* namespace GUILib */
