#include <cpuid.h>
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
        QString gpuType() {
            IDXGIFactory4* pFactory = nullptr; IDXGIAdapter1* pAdapter = nullptr;
            QString gpuDescription;
            int count = 0;
            HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));if (FAILED(hr)){}
            for (UINT i = 0; pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
                DXGI_ADAPTER_DESC1 adapterDesc;
                hr = pAdapter->GetDesc1(&adapterDesc);if (FAILED(hr)) {continue;}
                QString adapterName = QString::fromWCharArray(adapterDesc.Description);
                unsigned long long videoMemoryInBytes = adapterDesc.DedicatedVideoMemory;
                int videoMemoryInMB = static_cast<int>(videoMemoryInBytes / (1024 * 1024));
                bool isIntegrated = false;
                QStringList integratedKeywords = {"Intel HD", "HD Graphics", "UHD", "Iris", "Vega"};
                if (adapterName.contains("Driver", Qt::CaseInsensitive)) {continue;}
                for (const QString& keyword : integratedKeywords) {if (adapterName.contains(keyword, Qt::CaseInsensitive)) {isIntegrated = true; break;}}
                if (count > 0) { gpuDescription.append(" + "); }
                gpuDescription.append(adapterName); ++count;
                if (!isIntegrated) {
                    if (videoMemoryInMB >= 900 && videoMemoryInMB < 1024) {gpuDescription.append(" 1 GB");}
                    else if (videoMemoryInMB >= 1024) {
                        double videoMemoryInGB = videoMemoryInBytes / (1024.0 * 1024.0 * 1024.0);
                        if (videoMemoryInMB % 1024 >= 500) {videoMemoryInGB = qCeil(videoMemoryInGB);}
                        else {videoMemoryInGB = qFloor(videoMemoryInGB);}
                        gpuDescription.append(QString(" %1 GB").arg(videoMemoryInGB, 0, 'f', videoMemoryInMB % 1024 >= 512 ? 0 : 1));
                    } else {gpuDescription.append(QString(" %1 MB").arg(videoMemoryInMB));}
                }
            } return gpuDescription;
        }
};
#endif // MYOS_H
