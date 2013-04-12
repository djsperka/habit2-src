#ifndef MONITORMANAGER_H
#define MONITORMANAGER_H

namespace System
{
    class MonitorManager;
}

class System::MonitorManager
{
public:
    MonitorManager();
    ~MonitorManager();

public:
    unsigned int monitorsCount() const;

private:

};

#endif // MONITORMANAGER_H
