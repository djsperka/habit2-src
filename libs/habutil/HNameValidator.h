/*
 * HNameValidator.h
 *
 *  Created on: Jan 31, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HNAMEVALIDATOR_H_
#define LIBS_HABUTIL_HNAMEVALIDATOR_H_

#include <QRegExpValidator>
#include <QRegExp>

namespace GUILib {

class HName
{
public:

	static const QString& getNameREString();
	static const QString& getNameAndLabelREString();
	static const QRegExp& getNameRE();
	static const QRegExp& getNameAndLabelRE();

};

class HNameRegExp: public QRegExp
{
public:
	HNameRegExp();
	virtual ~HNameRegExp();
};

class HNameAndLabelRegExp: public QRegExp
{
public:
	HNameAndLabelRegExp();
	virtual ~HNameAndLabelRegExp();
};

class HNameValidator: public QRegExpValidator {
public:
	HNameValidator(QWidget *parent=NULL);
	static const QString& getNameRE();
	virtual ~HNameValidator();
};

class HNameAndLabelValidator: public QRegExpValidator {
public:
	HNameAndLabelValidator(QWidget *parent=NULL);
	static const QString& getNameAndLabelRE();
	virtual ~HNameAndLabelValidator();
};

} /* namespace GUILib */

#endif /* LIBS_HABUTIL_HNAMEVALIDATOR_H_ */
