#include <dxgi1_4.h>
#include <QtCharts>
#include <psapi.h>
#ifndef MYOS_H
#define MYOS_H

QT_BEGIN_NAMESPACE
namespace Ui { class MyOS; }
QT_END_NAMESPACE

class MyOS : public QMainWindow {
    Q_OBJECT
    public:
        MyOS(QWidget *parent = nullptr);
        ~MyOS();
    private slots:
        void diskSpace();
        void specs();
        void graph();
        void fileExplorer();
        void settings();
        void taskManager();
        void newPath(const QString& newPath, QFileSystemModel* model);
        void saveShortcuts();
        void loadShortcuts();
    private:
        Ui::MyOS *ui;
        float cpuLoad() {
            FILETIME idleTime, kernelTime, userTime;
            if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {}
            ULARGE_INTEGER lastIdleTime, lastKernelTime, lastUserTime;
            static ULARGE_INTEGER prevIdleTime, prevKernelTime, prevUserTime;
            lastIdleTime.LowPart = idleTime.dwLowDateTime; lastIdleTime.HighPart = idleTime.dwHighDateTime; lastKernelTime.LowPart = kernelTime.dwLowDateTime;
            lastKernelTime.HighPart = kernelTime.dwHighDateTime; lastUserTime.LowPart = userTime.dwLowDateTime; lastUserTime.HighPart = userTime.dwHighDateTime;
            float idleTimeDiff = (lastIdleTime.QuadPart - prevIdleTime.QuadPart), kernelTimeDiff = (lastKernelTime.QuadPart - prevKernelTime.QuadPart);
            float userTimeDiff = (lastUserTime.QuadPart - prevUserTime.QuadPart), totalTimeDiff = kernelTimeDiff + userTimeDiff, load = 100 * (totalTimeDiff - idleTimeDiff) / totalTimeDiff;
            prevIdleTime = lastIdleTime; prevKernelTime = lastKernelTime; prevUserTime = lastUserTime;
            return load;
        }
};
#endif // MYOS_H
