/*
 * HTrialKey.h
 *
 *  Created on: Mar 10, 2016
 *      Author: Oakeslab
 */

#ifndef HTRIALKEY_H_
#define HTRIALKEY_H_

#include <QString>

class HTrialKey {
	QString m_phase;
	int m_trial;
	int m_repeat;
public:
	HTrialKey();
	HTrialKey(const HTrialKey& k);
	HTrialKey(const QString& phase, int trial, int repeat);
	virtual ~HTrialKey();
	void setPhase(const QString& phase);
	const QString& phase() const;
	void setTrial(int trial);
	int trial() const;
	void setRepeat(int repeat);
	int repeat() const;

	HTrialKey& operator=(const HTrialKey& rhs);
	bool operator<(const HTrialKey& rhs);
};

#endif /* HTRIALKEY_H_ */
