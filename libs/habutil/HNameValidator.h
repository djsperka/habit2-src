/*
 * HNameValidator.h
 *
 *  Created on: Jan 31, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HNAMEVALIDATOR_H_
#define LIBS_HABUTIL_HNAMEVALIDATOR_H_

#include <QRegularExpressionValidator>
#include <QRegularExpression>

namespace GUILib {

class HName
{
public:

	static const QString& getNameREString();
	static const QString& getNameAndLabelREString();
	static const QRegularExpression& getNameRE();
	static const QRegularExpression& getNameAndLabelRE();

};

class HNameRegExp: public QRegularExpression
{
public:
	HNameRegExp();
	virtual ~HNameRegExp();
};

class HNameAndLabelRegExp: public QRegularExpression
{
public:
	HNameAndLabelRegExp();
	virtual ~HNameAndLabelRegExp();
};

class HNameValidator: public QRegularExpressionValidator {
public:
	HNameValidator(QWidget *parent=NULL);
	static const QString& getNameRE();
	virtual ~HNameValidator();
};

class HNameAndLabelValidator: public QRegularExpressionValidator {
public:
	HNameAndLabelValidator(QWidget *parent=NULL);
	static const QString& getNameAndLabelRE();
	virtual ~HNameAndLabelValidator();
};

} /* namespace GUILib */

#endif /* LIBS_HABUTIL_HNAMEVALIDATOR_H_ */
