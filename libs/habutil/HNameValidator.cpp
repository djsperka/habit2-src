/*
 * HNameValidator.cpp
 *
 *  Created on: Jan 31, 2017
 *      Author: dan
 */

#include "HNameValidator.h"
#include <QWidget>

namespace GUILib {


static const QString sreWordWithSpaces("((\\w)|(\\w[ '_-\\w]*\\w))");
static const QRegularExpression reStimAndLabel(sreWordWithSpaces + "(/" + sreWordWithSpaces + ")?");

//static const QString sreWordWithPunctuation("(\\w[ ':;_-!@#$%\\w]*)");
static const QString sreWordWithPunctuation("(\\w[ ':;_!@#$%&*()=+,.?\\w-]*)");
static const QString sreWordWithPunctuationAndLabel(sreWordWithPunctuation + "(/" + sreWordWithPunctuation + ")?");
static const QRegularExpression reHabitName(sreWordWithPunctuation);
static const QRegularExpression reHabitNameAndLabel(sreWordWithPunctuationAndLabel);

const QString& HName::getNameREString()
{
	return sreWordWithPunctuation;
}

const QString& HName::getNameAndLabelREString()
{
	return sreWordWithPunctuationAndLabel;
}

const QRegularExpression& HName::getNameRE()
{
	return reHabitName;
}

const QRegularExpression& HName::getNameAndLabelRE()
{
	return reHabitNameAndLabel;
}


HNameRegExp::HNameRegExp()
: QRegularExpression(sreWordWithPunctuation)
{
}

HNameRegExp::~HNameRegExp()
{
}

HNameAndLabelRegExp::HNameAndLabelRegExp()
: QRegularExpression(sreWordWithPunctuationAndLabel)
{
}

HNameAndLabelRegExp::~HNameAndLabelRegExp()
{
}


const QString& HNameAndLabelValidator::getNameAndLabelRE()
{
	return sreWordWithPunctuationAndLabel;
}

const QString& HNameValidator::getNameRE()
{
	return sreWordWithPunctuation;
}

HNameValidator::HNameValidator(QWidget *parent)
: QRegularExpressionValidator(reHabitName, parent)
{
}

HNameValidator::~HNameValidator()
{
}

HNameAndLabelValidator::HNameAndLabelValidator(QWidget *parent)
: QRegularExpressionValidator(reHabitName, parent)
{
}

HNameAndLabelValidator::~HNameAndLabelValidator()
{
}



} /* namespace GUILib */
