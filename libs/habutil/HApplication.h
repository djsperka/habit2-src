/*
 * HApplication.h
 *
 *  Created on: Jun 12, 2013
 *      Author: Oakeslab
 */

#ifndef HAPPLICATION_H_
#define HAPPLICATION_H_

#include <QApplication>

class HApplication : public QApplication
{
	Q_OBJECT
public:
	HApplication(int argc, char **argv) : QApplication(argc, argv) {};
	~HApplication() {};
protected:
	bool event(QEvent* event);
signals:
	void showResultsFile(QString filename);
};

#endif /* HAPPLICATION_H_ */
