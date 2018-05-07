#include "subjectinformation.h"

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

namespace GUILib {

SubjectInformation::SubjectInformation(const Habit::SubjectSettings& ss, QWidget* w)
	: QDialog(w)
	, settings_(ss)
{
	createComponents();
	makeConnections();
	doLayout();
	setAlignment();
	initialize();
}

SubjectInformation::~SubjectInformation()
{}

void GUILib::SubjectInformation::createComponents()
{
	createLabelsSection();
	createEditsSection();
}

void GUILib::SubjectInformation::makeConnections()
{
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelClick()));
	connect(okButton_, SIGNAL(clicked()), this, SLOT(onOkClick()));
	connect(clearSubjectInfo_, SIGNAL(clicked()), this, SLOT(onClearSubjectInfo()));
}

void GUILib::SubjectInformation::doLayout()
{
	mainLayout_ = new QVBoxLayout(this);
	grid_ = new QGridLayout();
	mainLayout_->addLayout(grid_);
	doLabelsLayout();
	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	buttonsLayout->addWidget(clearSubjectInfo_, 0, Qt::AlignRight);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(cancelButton_);
	buttonsLayout->addWidget(okButton_);
	mainLayout_->addLayout(buttonsLayout);
}

void GUILib::SubjectInformation::createLabelsSection()
{
	name_ = new QLabel(tr("Subject Name:"));
	birthdate_ = new QLabel(tr("Subject Birthdate:"));
	testdate_ = new QLabel(tr("Test Date:"));
	condition_ = new QLabel(tr("Condition:"));
	observer_ = new QLabel(tr("Observer:"));
	cellNo_ = new QLabel(tr("Cell Number:"));
	comments_ = new QLabel(tr("Comments:"));
	clearSubjectInfo_ = new QPushButton(tr("Clear Subject Info"));
	cancelButton_ = new QPushButton(tr("Cancel"));
	okButton_ = new QPushButton(tr("OK"));
}

void GUILib::SubjectInformation::createEditsSection()
{
	nameEdit_ = new QLineEdit();
	birthdateEdit_ = new QDateTimeEdit();
	birthdateEdit_->setCalendarPopup(true);
	testdateEdit_ = new QDateTimeEdit();
	testdateEdit_->setCalendarPopup(true);
	conditionEdit_ = new QLineEdit();
	observerEdit_ = new QLineEdit();
	cellNoEdit_ = new QLineEdit();
	cellNoEdit_->setValidator(new QRegExpValidator(QRegExp("[+]?[()0-9]*"), this));
	commentsEdit_ = new QTextEdit();
}

void GUILib::SubjectInformation::doLabelsLayout()
{
	grid_->addWidget(name_, 0, 0);
	grid_->addWidget(nameEdit_, 0, 1) ;
	grid_->addWidget(birthdate_, 1, 0);
	grid_->addWidget(birthdateEdit_, 1,1);
	grid_->addWidget(testdate_, 2, 0);
	grid_->addWidget(testdateEdit_, 2, 1);
	grid_->addWidget(condition_, 3, 0);
	grid_->addWidget(conditionEdit_, 3, 1);
	grid_->addWidget(observer_, 4, 0);
	grid_->addWidget(observerEdit_, 4, 1);
	grid_->addWidget(cellNo_, 5, 0);
	grid_->addWidget(cellNoEdit_, 5, 1);
	grid_->addWidget(comments_, 6, 0);
	grid_->addWidget(commentsEdit_, 6, 1);
}

void GUILib::SubjectInformation::setAlignment()
{
	name_->setAlignment(Qt::AlignRight);
	birthdate_->setAlignment(Qt::AlignRight);
	testdate_->setAlignment(Qt::AlignRight);
	condition_->setAlignment(Qt::AlignRight);
	observer_->setAlignment(Qt::AlignRight);
	cellNo_->setAlignment(Qt::AlignRight);
	comments_->setAlignment(Qt::AlignRight);
}

void GUILib::SubjectInformation::onClearSubjectInfo() {
	nameEdit_->clear();
	birthdateEdit_->clear();
	testdateEdit_->clear();
	conditionEdit_->clear();
	observerEdit_->clear();
	cellNoEdit_->clear();
	commentsEdit_->clear();
}

void GUILib::SubjectInformation::onCancelClick() {
	reject();
}

void GUILib::SubjectInformation::onOkClick() {
	if(validate()) {
		accept();
	}
}

bool GUILib::SubjectInformation::validate() {
	bool t = false;
	if(nameEdit_->text().trimmed().isEmpty()) {
		QMessageBox::warning(this, "Name is empty", "Subject Name is required field");
	} else
	if(observerEdit_->text().trimmed().isEmpty()) {
		QMessageBox::warning(this, "Observer name is empty", "Observer name is required field");
	} else
	if(birthdateEdit_->dateTime() >= testdateEdit_->dateTime()) {
		QMessageBox::warning(this, "Testdate is smaller than birthdate", "Birthdate must be smaller than test-date");
	} else {
		t = true;
	}
	return t;
}

void GUILib::SubjectInformation::initialize() {
	nameEdit_->setText(settings_.getSubjectName());
	birthdateEdit_->setDateTime(settings_.getBirthDate());
	testdateEdit_->setDateTime(settings_.getTestDate());
	conditionEdit_->setText(settings_.getCondition());
	observerEdit_->setText(settings_.getObserver());
	cellNoEdit_->setText(settings_.getCellNumber());
	commentsEdit_->setText(settings_.getComments());
}

void GUILib::SubjectInformation::setConfigurationObject(const Habit::SubjectSettings& settings) {
	settings_ = settings;
	initialize();
}

Habit::SubjectSettings GUILib::SubjectInformation::getConfigurationObject() {
	Habit::SubjectSettings ss;
	ss.setId(settings_.getId());
	ss.setSubjectName(nameEdit_->text());
	ss.setBirthDate(birthdateEdit_->dateTime());
	ss.setTestDate(testdateEdit_->dateTime());
	ss.setCondition(conditionEdit_->text());
	ss.setObserver(observerEdit_->text());
	ss.setCellNumber(cellNoEdit_->text());
	ss.setComments(commentsEdit_->toPlainText());
	return ss;
}

} // namespace GUILib
