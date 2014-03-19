/*
 * HDBUtil.h
 *
 *  Created on: Aug 8, 2013
 *      Author: Oakeslab
 */

#ifndef HDBUTIL_H_
#define HDBUTIL_H_

#include <QString>
#include <QFileInfo>
#include <QtSql/QSqlDatabase>

bool selectDB(bool bExisting);
void closeDB();
bool openDB();
int getDBVersion();
bool getDBTableExists(QString name);
bool updateDBVersion(QSqlDatabase& db, const QFileInfo& fileinfo);

#endif /* HDBUTIL_H_ */
