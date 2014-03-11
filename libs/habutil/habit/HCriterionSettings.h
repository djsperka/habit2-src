/*
 * HCriterionSettings.h
 *
 *  Created on: Mar 11, 2014
 *      Author: Oakeslab
 */

#ifndef HCRITERIONSETTINGS_H_
#define HCRITERIONSETTINGS_H_

#include "HTypes.h"
#include <QDataStream>

namespace Habit
{

	class HCriterionSettings
	{
	public:
		HCriterionSettings(const HCriterionBasisType& basis = HCriterionBasisType::HCriterionBasisFirstN, unsigned int percent=50, unsigned int windowSize=3, const HCriterionWindowType& windowType = HCriterionWindowType::HCriterionWindowFixed, bool bExcludeBasisWindow = false, bool bRequireMinBasisValue = false, unsigned int uiMinBasisValue = 0);
		HCriterionSettings(const HCriterionSettings& c);
		~HCriterionSettings();


		const HCriterionBasisType& getBasis() const;
		void setBasis(const HCriterionBasisType& basis);
		unsigned int getPercent() const;
		void setPercent(unsigned int persent);
		unsigned int getWindowSize() const;
		void setWindowSize(unsigned int windowSize);
		const HCriterionWindowType& getWindowType() const;
		void setWindowType(const HCriterionWindowType& windowType);
		void setExcludeBasisWindow(bool b);
		bool getExcludeBasisWindow() const;
		void setRequireMinBasisValue(bool b);
		bool getRequireMinBasisValue() const;
		void setMinBasisValue(unsigned int v);
		unsigned int getMinBasisValue() const;

	private:
		const HCriterionBasisType* pbasis_;
		unsigned int percent_;
		unsigned int windowSize_;
		const HCriterionWindowType* pwindowType_;
		bool m_bExcludeBasisWindow;
		bool m_bRequireMinBasisValue;
		unsigned int m_uiMinBasisValue;
	};

	QDataStream & operator<< (QDataStream& stream, Habit::HCriterionSettings d);
	QDataStream & operator>> (QDataStream& stream, Habit::HCriterionSettings& d);
	bool operator==(const Habit::HCriterionSettings& lhs, const Habit::HCriterionSettings& rhs);

} // namespace
#endif /* HCRITERIONSETTINGS_H_ */
