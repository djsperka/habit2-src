/*
 * HSubjectSettingsWidget.cpp
 *
 *  Created on: Dec 2, 2014
 *      Author: Oakeslab
 */

#include "HSubjectSettingsWidget.h"
#include "ui_HSubjectSettingsForm.h"

using namespace GUILib;

HSubjectSettingsWidget::HSubjectSettingsWidget(bool bTestRun, QWidget *parent)
: QWidget(parent)
, ui(new Ui::HSubjectSettingsForm)
{
	ui->setupUi(this);
	connect(ui->cbxTestRun, SIGNAL(clicked(bool)), this, SLOT(testRunClicked(bool)));
	ui->cbxTestRun->setChecked(bTestRun);
	testRunClicked(bTestRun);
}

HSubjectSettingsWidget::~HSubjectSettingsWidget()
{
	qDebug() << "HSubjectSettingsWidget::~HSubjectSettingsWidget()";
	delete ui;
}

QString HSubjectSettingsWidget::getComments() const
{
	return ui->pteNotes->toPlainText();
}

QString HSubjectSettingsWidget::getObserverID() const
{
	return ui->lineeditObserverID->text();
}

QString HSubjectSettingsWidget::getSubjectID() const
{
	return ui->lineeditSubjectID->text();
}

bool HSubjectSettingsWidget::isTestRun() const
{
	return ui->cbxTestRun->isChecked();
}

void HSubjectSettingsWidget::testRunClicked(bool checked)
{
	ui->lineeditSubjectID->setDisabled(checked);
	ui->lineeditObserverID->setDisabled(checked);
	ui->pteNotes->setDisabled(checked);
	if (checked)
	{
		m_idSaved = ui->lineeditSubjectID->text();
		m_obsSaved = ui->lineeditObserverID->text();
		m_notesSaved = ui->pteNotes->toPlainText();

		ui->lineeditSubjectID->setText(QString("Test Subject"));
		ui->lineeditObserverID->setText(QString("Test Observer"));
		ui->pteNotes->setPlainText(QString());
	}
	else
	{
		ui->lineeditSubjectID->setText(m_idSaved);
		ui->lineeditObserverID->setText(m_obsSaved);
		ui->pteNotes->setPlainText(m_notesSaved);
	}
}



