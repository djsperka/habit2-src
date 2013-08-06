#include "subjectinformationwidget.h"
#include <QMessageBox>

SubjectInformationWidget::SubjectInformationWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.cellNumberEdit->setValidator(new QRegExpValidator(QRegExp("[+]?[()0-9]*"), this));
	onClearInfo();
}

SubjectInformationWidget::~SubjectInformationWidget()
{

}

void SubjectInformationWidget::onClearInfo()
{
	ui.subjectNameEdit->clear();
	ui.birthDateTimeEdit->setDateTime(QDateTime::fromString("01/01/2000", "MM/dd/yyyy"));
	ui.testDateTimeEdit->setDateTime(QDateTime::currentDateTime());
	ui.observerEdit->clear();
	ui.cellNumberEdit->clear();
	ui.conditionEdit->clear();
	ui.commentEdit->clear();
}

void SubjectInformationWidget::setConfigurationObject(const Habit::SubjectSettings& settings)
{
	settings_ = settings;
	ui.subjectNameEdit->setText(settings_.getSubjectName());
	ui.birthDateTimeEdit->setDateTime(settings_.getBirthDate());
	ui.testDateTimeEdit->setDateTime(settings_.getTestDate());
	ui.observerEdit->setText(settings_.getObserver());
	ui.cellNumberEdit->setText(settings_.getCellNumber());
	ui.conditionEdit->setText(settings_.getCondition());
	ui.commentEdit->setPlainText(settings_.getComments());
}

Habit::SubjectSettings SubjectInformationWidget::getConfigurationObject()
{
	Habit::SubjectSettings ss;
	ss.setId(settings_.getId());
	ss.setSubjectName(ui.subjectNameEdit->text());
	ss.setBirthDate(ui.birthDateTimeEdit->dateTime());
	ss.setTestDate(ui.testDateTimeEdit->dateTime());
	ss.setCondition(ui.conditionEdit->text());
	ss.setObserver(ui.observerEdit->text());
	ss.setCellNumber(ui.cellNumberEdit->text());
	ss.setComments(ui.commentEdit->toPlainText());
	return ss;
}

bool SubjectInformationWidget::validate() {
	bool t = false;
	if(ui.subjectNameEdit->text().trimmed().isEmpty()) {
		QMessageBox::warning(this, "Name is empty", "Subject Name is required field");
	} else
	if(ui.observerEdit->text().trimmed().isEmpty()) {
		QMessageBox::warning(this, "Observer name is empty", "Observer name is required field");
	} else
	if(ui.birthDateTimeEdit->dateTime() >= ui.testDateTimeEdit->dateTime()) {
		QMessageBox::warning(this, "Testdate is smaller than birthdate", "Birthdate must be smaller than test-date");
	} else {
		t = true;
	}
	return t;
}
