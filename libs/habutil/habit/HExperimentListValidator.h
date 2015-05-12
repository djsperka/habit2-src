/*
 * HExperimentListValidator.h
 *
 *  Created on: Nov 12, 2014
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTLISTVALIDATOR_H_
#define HEXPERIMENTLISTVALIDATOR_H_

#include <QValidator>
#include <QStringList>

namespace GUILib
{
	class HExperimentListValidator: public QValidator
	{
		Q_OBJECT
	private:
		QStringList m_list;
	public:
		HExperimentListValidator(QStringList list, QObject *parent=0): QValidator(parent), m_list(list) {};
		virtual ~HExperimentListValidator() {};
		QValidator::State validate(QString& input, int& pos) const;
	};
};


#endif /* HEXPERIMENTLISTVALIDATOR_H_ */
