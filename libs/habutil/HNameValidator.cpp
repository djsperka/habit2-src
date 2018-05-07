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
static const QRegExp reStimAndLabel(sreWordWithSpaces + "(/" + sreWordWithSpaces + ")?");

//static const QString sreWordWithPunctuation("(\\w[ ':;_-!@#$%\\w]*)");
static const QString sreWordWithPunctuation("(\\w[ ':;_!@#$%&*()=+,.?\\w-]*)");
static const QString sreWordWithPunctuationAndLabel(sreWordWithPunctuation + "(/" + sreWordWithPunctuation + ")?");
static const QRegExp reHabitName(sreWordWithPunctuation);
static const QRegExp reHabitNameAndLabel(sreWordWithPunctuationAndLabel);

const QString& HName::getNameREString()
{
	return sreWordWithPunctuation;
}

const QString& HName::getNameAndLabelREString()
{
	return sreWordWithPunctuationAndLabel;
}

const QRegExp& HName::getNameRE()
{
	return reHabitName;
}

const QRegExp& HName::getNameAndLabelRE()
{
	return reHabitNameAndLabel;
}


HNameRegExp::HNameRegExp()
: QRegExp(sreWordWithPunctuation)
{
}

HNameRegExp::~HNameRegExp()
{
}

HNameAndLabelRegExp::HNameAndLabelRegExp()
: QRegExp(sreWordWithPunctuationAndLabel)
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
: QRegExpValidator(reHabitName, parent)
{
}

HNameValidator::~HNameValidator()
{
}

HNameAndLabelValidator::HNameAndLabelValidator(QWidget *parent)
: QRegExpValidator(reHabitName, parent)
{
}

HNameAndLabelValidator::~HNameAndLabelValidator()
{
}



} /* namespace GUILib */
