#include "reliabilitiesform.h"

namespace GUILib {

ReliabilitiesForm::ReliabilitiesForm(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	onClearInfo();
}

ReliabilitiesForm::~ReliabilitiesForm()
{

}

void ReliabilitiesForm::onClearInfo()
{
	ui.observerEdit->clear();
	ui.commentEdit->clear();
	ui.dateEdit->setDate(QDate::currentDate());
}

QString ReliabilitiesForm::getObserver()
{
	return ui.observerEdit->text();
}

QDate	ReliabilitiesForm::getDate()
{
	return ui.dateEdit->date();
}

QString	ReliabilitiesForm::getComment()
{
	return ui.commentEdit->toPlainText();
}

}