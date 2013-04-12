
#ifndef SYSTEM_SLEEPER_H
#define SYSTEM_SLEEPER_H

#include <QtCore/QThread>

namespace System {

class sleeper : public QThread
{
public:
	static void msleep(unsigned long ms) {
		QThread::msleep(ms);
	}
};

} // namespace System

#endif // SYSTEM_SLEEPER_H
