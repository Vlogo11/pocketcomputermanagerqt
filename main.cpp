#include "myos.h"
#include "./ui_myos.h"

MyOS::MyOS(QWidget *parent):QMainWindow(parent), ui(new Ui::MyOS) {
    ui->setupUi(this);
    diskSpace(); specs(); graph(); fileExplorer(); settings(); taskManager(); cpuLoad(); loadShortcuts();
    setFixedSize(589, 373);
    QIcon appIcon(QCoreApplication::applicationDirPath() + "/Images/icon.png"); setWindowIcon(appIcon);
    setWindowTitle("Pocket Computer Manager");
}
int main(int argc, char *argv[]) {QApplication a(argc, argv); MyOS w; w.show(); return a.exec();}
MyOS::~MyOS(){delete ui;}
void setLabelIcon(QLabel* label, const QString& imagePath) {QIcon icon(QCoreApplication::applicationDirPath() + imagePath); label->setPixmap(icon.pixmap(label->size()));}
void updateStorageInfo(const QString& path, qint64& size, QLabel* label, QLineEdit* lineEdit) {
    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories); lineEdit->setText(path);
    while (it.hasNext()) {size += QFileInfo(it.next()).size();}
    if (size < 1073741824) {label->setText(QString::number(static_cast<double>(size) / (1024.0 * 1024.0), 'f', 1) + " MB");}
    else {label->setText(QString::number(static_cast<double>(size) / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " GB");}
}
void MyOS::diskSpace() {
    ULARGE_INTEGER total_size, free_size, used_size;
    if (!GetDiskFreeSpaceEx(NULL, &free_size, &total_size, &used_size)) {}
    used_size.QuadPart = total_size.QuadPart - free_size.QuadPart;
    setLabelIcon(ui->label_13, "/Images/cpu.png"); setLabelIcon(ui->label_20, "/Images/gpu.png"); setLabelIcon(ui->label_14, "/Images/ram.png");
    QIcon binIcon(QCoreApplication::applicationDirPath() + "/Images/bin.png"); ui->clearTrashButton->setIcon(binIcon.pixmap(ui->clearTrashButton->size()));
    connect(ui->clearTrashButton, &QPushButton::clicked, this, [this, binIcon]() {
        SHEmptyRecycleBin(NULL, NULL, SHERB_NOCONFIRMATION|SHERB_NOPROGRESSUI|SHERB_NOSOUND);
        QIcon successIcon(QCoreApplication::applicationDirPath() + "/Images/success.png"); ui->clearTrashButton->setIcon(successIcon.pixmap(ui->clearTrashButton->size()));
        QTimer::singleShot(3000, this, [this, binIcon]() {ui->clearTrashButton->setIcon(binIcon.pixmap(ui->clearTrashButton->size()));});
    });

    qint64 images_size = 0, downloads_size = 0, docs_size = 0;
    updateStorageInfo(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), images_size, ui->storage_6, ui->lineEdit_2);
    updateStorageInfo(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), downloads_size, ui->storage_7, ui->lineEdit_5);
    updateStorageInfo(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), docs_size, ui->label_6, ui->lineEdit_4);
    QHBoxLayout* layout = new QHBoxLayout; layout->addWidget(ui->storage_2); layout->addWidget(ui->storage_6); layout->setContentsMargins(5, 0, 5, 0);
    ui->storage_2->setStyleSheet("color: #008F00;"); ui->pushButton_2->setLayout(layout);
    QHBoxLayout* layout2 = new QHBoxLayout; layout2->addWidget(ui->storage_4); layout2->addWidget(ui->label_6); layout2->setContentsMargins(5, 0, 5, 0);
    ui->storage_4->setStyleSheet("color: #6495ED;"); ui->pushButton_3->setLayout(layout2);
    QHBoxLayout* layout3 = new QHBoxLayout; layout3->addWidget(ui->storage_3); layout3->addWidget(ui->storage_7); layout3->setContentsMargins(5, 0, 5, 0);
    ui->storage_3->setStyleSheet("color: red;");ui->pushButton_4->setLayout(layout3);

    ui->usedSize->setText(QString::number((int)((double)used_size.QuadPart / (double)total_size.QuadPart * 100)) + "%"); ui->label->setText(QString::number((double) used_size.QuadPart / 1073741824, 'f', 2) + " GB");
    QPieSeries *series = new QPieSeries(); series->setHoleSize(0.75); series->setPieSize(1.0);
    QPieSlice *imagesSlice = series->append("Images", images_size); imagesSlice->setColor(QColor(0, 128, 0)); imagesSlice->setBorderColor(QColor(0, 128, 0));
    QPieSlice *docsSlice = series->append("Docs", docs_size); docsSlice->setColor(QColor(100, 149, 237)); docsSlice->setBorderColor(QColor(100, 149, 237));
    QPieSlice *downloadsSlice = series->append("Downloads", downloads_size); downloadsSlice->setColor(QColor(255, 0, 0)); downloadsSlice->setBorderColor(QColor(255, 0, 0));
    QPieSlice *othersSlice = series->append("Others", used_size.QuadPart - images_size - docs_size - downloads_size); othersSlice->setColor(QColor(255, 165, 0)); othersSlice->setBorderColor(QColor(255, 165, 0));
    QPieSlice *remainingSlice = series->append("Remaining", static_cast<float>(total_size.QuadPart - used_size.QuadPart));remainingSlice->setColor(Qt::white); series->append(remainingSlice);
    QChart *chart = new QChart(); chart->addSeries(series); chart->legend()->hide(); chart->setBackgroundBrush(QColor(0, 0, 0, 0));
    QChartView *chartView = new QChartView(chart, ui->circleBar_2); chartView->setGeometry(0, 0, ui->circleBar_2->width(), ui->circleBar_2->height());
    chartView->setRenderHint(QPainter::Antialiasing);
    for (const QStorageInfo& storage : QStorageInfo::mountedVolumes()) {
        QString disks = storage.rootPath().left(1) + " - " + QString::number(storage.bytesTotal() / (1024 * 1024 * 1024.0), 'f', 2) + " GB";
        if (storage.rootPath().left(1) == QCoreApplication::applicationFilePath().left(1)) {disks += " - OS";} ui->otherDisks->addItem(disks);
        connect(ui->otherDisks, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
            for (const QStorageInfo& storage : QStorageInfo::mountedVolumes()) {
                if (storage.rootPath().left(1) == ui->otherDisks->itemText(index).section(" - ", 0, 0)) {
                    double usedGB = (storage.bytesTotal() - storage.bytesAvailable()) / 1073741824.0, totalGB = storage.bytesTotal() / 1073741824.0;
                    ui->label->setText(QString::number(usedGB, 'f', 2) + " GB"); ui->usedSize->setText(QString::number((usedGB / totalGB) * 100) + "%");
                }
            }
        });
    }
    connect(ui->menuStorage, &QMenu::aboutToShow, this, [this]() {ui->diskInfo->show();ui->specs->hide();ui->tasks->hide();ui->settings->hide();});
}
void MyOS::specs() {
    ui->label_11->setText("Specification of " + QSysInfo::machineHostName());
    QProcess process; process.start("cmd", QStringList() << "/C" << "wmic cpu get Name,NumberOfCores,NumberOfLogicalProcessors & wmic memorychip get Capacity,Speed"); process.waitForFinished();
    QString output = process.readAllStandardOutput(), cpuType, gpuDescription; QStringList lines = output.split('\n');
    MEMORYSTATUSEX status; status.dwLength = sizeof(status); GlobalMemoryStatusEx(&status);
    double installedRAMinGB = status.ullTotalPhys / (1024.0 * 1024.0 * 1024.0), numCores = 0, numThreads = 0, slotsUsed = 0, totalSlotSize = 0, speed = 0, count = 0;
    foreach (QString line, lines) {
        QStringList details = line.trimmed().split(QRegularExpression("\\s{2,}")); // Split by two or more spaces
        if (details.size() == 3 && !details[0].contains("Name")) {cpuType = details[0]; numCores = details[1].toInt(); numThreads = details[2].toInt();}
        details = line.trimmed().split(QRegularExpression("\\s+"));
        if (details.size() == 2 && details[0].toLongLong() > 0) {slotsUsed++; totalSlotSize += details[0].toDouble() / (1024.0 * 1024.0 * 1024.0); speed = details[1].toInt();}
    } cpuType.remove("(R)"); cpuType.remove("(TM)"); cpuType.remove(" CPU"); cpuType.replace("@", "-");
    QString cpuInfo = QString("%1, %2 Cores, %3 Threads").arg(cpuType).arg(numCores).arg(numThreads); ui->label_12->setText(cpuInfo);
    ui->label_21->setText(QString("Installed memory: %1 GB (%2 * %3 GB %4 MHz - %5 GB Useable)").arg(qRound(installedRAMinGB)).arg(slotsUsed).arg(totalSlotSize / slotsUsed).arg(speed).arg(QString::number(installedRAMinGB, 'f', 1)));
    connect(ui->menuSpecs, &QMenu::aboutToShow, this, [this]() {ui->diskInfo->hide();ui->specs->show();ui->tasks->hide();ui->specs->move(0, 0);ui->settings->hide();});
    IDXGIFactory4* pFactory = nullptr; IDXGIAdapter1* pAdapter = nullptr;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory)); if (FAILED(hr)){}
    for (UINT i = 0; pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC1 adapterDesc; hr = pAdapter->GetDesc1(&adapterDesc); if (FAILED(hr)) {continue;}
        QString adapterName = QString::fromWCharArray(adapterDesc.Description);
        if (adapterName.contains("Driver", Qt::CaseInsensitive)) {continue;}
        if (count > 0) { gpuDescription.append(" + "); } gpuDescription.append(adapterName); ++count;
        if (adapterDesc.DedicatedVideoMemory / (1024 * 1024) > 128) {
            if (adapterDesc.DedicatedVideoMemory / (1024 * 1024) % 1024 >= 500) {qCeil(adapterDesc.DedicatedVideoMemory / (1024.0 * 1024.0 * 1024.0));}
            gpuDescription.append(QString(" %1 GB").arg(adapterDesc.DedicatedVideoMemory / (1024.0 * 1024.0 * 1024.0), 0, 'f', adapterDesc.DedicatedVideoMemory / (1024 * 1024) % 1024 >= 512 ? 0 : 1));
        }
    } gpuDescription.remove("(R)"); ui->label_15->setText(gpuDescription);
    QTimer *timer2 = new QTimer(this);
    connect(timer2, &QTimer::timeout, this, [&]() {
        QProcess process; process.start("cmd.exe", QStringList() << "/c" << "cd C:\\Program Files\\NVIDIA Corporation\\NVSMI && nvidia-smi --query-gpu=temperature.gpu,utilization.gpu,memory.used,memory.total --format=csv,noheader,nounits");
        if (!process.waitForFinished()) {}
        QByteArray output = process.readAllStandardOutput(); QList<QString> values = QString(output).trimmed().split(',');
        QString temperature = values[0], utilization = values[1], usedVRAM = QString::number(values[2].toFloat() / 1024, 'f', 2), maxVRAM = QString::number(values[3].toFloat() / 1024, 'f', 2);
        ui->label_15->setText(QString("%1 °C, %4%, %2/%3 GB").arg(temperature, usedVRAM, maxVRAM, utilization));
    }); timer2->start(1000);
}
void MyOS::graph() {
    QLineSeries *series = new QLineSeries();
    QTimer *timer = new QTimer(); timer->start(1000);
    int i = 0; bool isRAMUsage = false; bool isCPULoad = true; bool isGPUUsage = false;
    QPushButton *cpuButton = new QPushButton("CPU"); QPushButton *gpuButton = new QPushButton("GPU"); QPushButton *ramButton = new QPushButton("RAM");
    connect(timer, &QTimer::timeout, this, [this, cpuButton, ramButton, series, &i, &isRAMUsage, &isCPULoad, &isGPUUsage]() {
        float getCpuLoad = cpuLoad();
        MEMORYSTATUSEX status; status.dwLength = sizeof(status); GlobalMemoryStatusEx(&status);
        unsigned long long totalRAM = status.ullTotalPhys, usedRAM = totalRAM - status.ullAvailPhys;
        double usedRAMPercent = (static_cast<double>(usedRAM) / totalRAM) * 100.0, usedRAMinGB = usedRAM / (1024.0 * 1024.0 * 1024.0);
        cpuButton->setText(QString("CPU (Usage: X%, Load: %1%)").arg(QString::number(getCpuLoad, 'f', 1)));
        ramButton->setText(QString("RAM (%1% - %2 GB)").arg(usedRAMPercent, 0, 'f', 2).arg(usedRAMinGB, 0, 'f', 2));
        if (i < 61) {
            if (isGPUUsage) {double randomValue = QRandomGenerator::global()->bounded(0, 101);series->append(i, randomValue);}
            else if (isRAMUsage) {series->append(i, usedRAMPercent);}
            else { series->append(i, getCpuLoad);} ++i;
        } else {i = 0;series->clear();}
    });

    connect(cpuButton, &QPushButton::clicked, [series, &i, &isCPULoad, &isRAMUsage, &isGPUUsage]{i = 0; series->clear(); isRAMUsage = false; isCPULoad = true; isGPUUsage = false;});
    connect(gpuButton, &QPushButton::clicked, [series, &i, &isCPULoad, &isGPUUsage, &isRAMUsage]{i = 0; series->clear(); isRAMUsage = false; isCPULoad = false; isGPUUsage = true;});
    connect(ramButton, &QPushButton::clicked, [series, &i, &isRAMUsage, &isCPULoad, &isGPUUsage](bool) {i = 0; series->clear(); isRAMUsage = true; isCPULoad = false; isGPUUsage = false;});

    QChart *chart = new QChart(); chart->addSeries(series); chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).at(0)->setRange(0, 60); chart->axes(Qt::Vertical).at(0)->setRange(0, 100);
    chart->axes(Qt::Horizontal).at(0)->setLabelsVisible(false); chart->axes(Qt::Vertical).at(0)->setLabelsVisible(false);
    chart->legend()->hide(); chart->setBackgroundBrush(QColor(0, 0, 0, 0)); chart->setMargins(QMargins(-10, -7, -3, -5));
    QChartView *chartView = new QChartView(chart); chartView->setRenderHint(QPainter::Antialiasing);
    QVBoxLayout *layout = new QVBoxLayout(ui->usage); layout->setContentsMargins(4, 0, 0, 0); layout->setSpacing(5);
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setContentsMargins(0, 7, 5, 0); buttonsLayout->addWidget(cpuButton); buttonsLayout->addWidget(gpuButton); buttonsLayout->addWidget(ramButton);
    layout->addLayout(buttonsLayout); layout->addWidget(chartView);
}
void MyOS::newPath(const QString& newPath, QFileSystemModel* model) {
    QFileInfo fileInfo(newPath);
    if (fileInfo.exists() && fileInfo.isDir()) {model->setRootPath(newPath); ui->treeView->setRootIndex(model->index(newPath));}
    else if (fileInfo.exists()) {QDesktopServices::openUrl(QUrl::fromLocalFile(newPath));}
}
void MyOS::fileExplorer() {
    ui->explorerFrame->setStyleSheet("QFrame#explorerFrame {background-color: #F9F9F9;}");
    QPixmap backicon(QCoreApplication::applicationDirPath() + "/Images/back_arrow.png"); ui->pushButton->setIconSize(ui->pushButton->size()); ui->pushButton->setIcon(backicon);
    ui->treeView->setStyleSheet("QTreeView#treeView { border: 0px solid black; background-color: transparent;}");
    QFileSystemModel* model = new QFileSystemModel; model->setRootPath(QDir::currentPath()); ui->treeView->setModel(model);
    ui->treeView->header()->resizeSection(0, 310); ui->treeView->header()->resizeSection(1, 70);
    ui->treeView->setColumnHidden(2, true); ui->treeView->setColumnHidden(3, true); ui->treeView->setRootIsDecorated(false);
    connect(ui->treeView, &QTreeView::doubleClicked, this, [=](const QModelIndex& index) {
        ui->usernameLineEdit_2->setText(model->filePath(index));
        if (model->isDir(index)) {model->setRootPath(model->filePath(index)); ui->treeView->setRootIndex(index);}
        else {QDesktopServices::openUrl(QUrl::fromLocalFile(model->filePath(index)));}
    });
    QMenu* fileMenu = new QMenu(ui->treeView); ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    QAction* deleteAction = new QAction("Delete", ui->treeView); QAction* cutAction = new QAction("Cut", ui->treeView); QAction* pasteAction = new QAction("Paste", ui->treeView);
    QAction* copyAction = new QAction("Copy", ui->treeView); QAction* renameAction = new QAction("Rename", ui->treeView);
    fileMenu->addAction(deleteAction); fileMenu->addAction(cutAction); fileMenu->addAction(copyAction);
    fileMenu->addAction(renameAction); fileMenu->addAction(pasteAction);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, [=](const QPoint& pos) {
        if (ui->treeView->indexAt(pos).isValid()) {
            deleteAction->setEnabled(true); cutAction->setEnabled(true); copyAction->setEnabled(true);
            renameAction->setEnabled(true); pasteAction->setEnabled(true); fileMenu->exec(ui->treeView->mapToGlobal(pos));
        }
        deleteAction->setEnabled(false); cutAction->setEnabled(false); copyAction->setEnabled(false);
        renameAction->setEnabled(false); pasteAction->setEnabled(true); fileMenu->exec(ui->treeView->mapToGlobal(pos));
    });
    connect(deleteAction, &QAction::triggered, this, [=]() {QString filePath = model->filePath(ui->treeView->currentIndex()); QDir dir(filePath); dir.removeRecursively();});
    connect(renameAction, &QAction::triggered, this, [=]() {
        QModelIndex index = ui->treeView->currentIndex(); QLineEdit *lineEdit = new QLineEdit(this);
        lineEdit->setText(model->fileName(index)); ui->treeView->setIndexWidget(index, lineEdit); lineEdit->selectAll();
        connect(lineEdit, &QLineEdit::editingFinished, this, [=]() {QFile::rename(model->filePath(index), QDir(model->fileInfo(index).path()).filePath(lineEdit->text()));});
    });
    connect(ui->usernameLineEdit_2, &QLineEdit::editingFinished, this, [=]() {newPath(ui->usernameLineEdit_2->text(), model);});
    connect(ui->pushButton_2, &QPushButton::clicked, this, [=]() {newPath(ui->lineEdit_2->text(), model); ui->usernameLineEdit_2->setText(ui->lineEdit_2->text());});
    connect(ui->pushButton_3, &QPushButton::clicked, this, [=]() {newPath(ui->lineEdit_4->text(), model); ui->usernameLineEdit_2->setText(ui->lineEdit_4->text());});
    connect(ui->pushButton_4, &QPushButton::clicked, this, [=]() {newPath(ui->lineEdit_5->text(), model); ui->usernameLineEdit_2->setText(ui->lineEdit_5->text());});
    ui->usernameLineEdit_2->setStyleSheet("border: 1px solid #E5E5E5;"); ui->usernameLineEdit_3->setStyleSheet("border: 1px solid #E5E5E5;");
    ui->pushButton->setStyleSheet("border: 1px solid #E5E5E5; background-color: #FFFFFF;");
    QStandardItemModel* searchModel = new QStandardItemModel(0, 2);
    connect(ui->usernameLineEdit_3, &QLineEdit::returnPressed, this, [=]() {
        QString searchTerm = ui->usernameLineEdit_3->text(); QTime startTime = QTime::currentTime();
        QThreadPool::globalInstance()->start([=]() {
            searchModel->removeRows(0, searchModel->rowCount()); QStringList results; QMutex resultsMutex; std::function<void(const QDir&)> searchFiles;
            searchFiles = [&](const QDir& dir) {
                QDir::Filters filters = QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden;
                QFileInfoList files = dir.entryInfoList(QStringList() << "*", filters, QDir::Name);
                for (const QFileInfo& file : files) {
                    if (file.fileName().contains(searchTerm, Qt::CaseInsensitive)) {QMutexLocker locker(&resultsMutex); results << file.filePath();}
                    if (file.isDir()) {searchFiles(QDir(file.filePath()));}
                }
            }; searchFiles(QDir(model->rootPath()));
            QMetaObject::invokeMethod(this, [=]() {
                QString countText = tr("%1 results for \"%2\"").arg(results.size()).arg(searchTerm);
                QStandardItem* countItem = new QStandardItem(countText);
                countItem->setSelectable(false); searchModel->invisibleRootItem()->appendRow(countItem);
                for (const QString& result : results) {
                    QFileInfo fileInfo(result);
                    QString size = (fileInfo.size() < 1024 * 1024) ? QString("%1 kB").arg(fileInfo.size() / 1024) : (fileInfo.size() < 1024 * 1024 * 1024) ? QString("%1 MB").arg(fileInfo.size() / 1024 / 1024) : QString("%1 GB").arg(fileInfo.size() / 1024 / 1024 / 1024);
                    QStandardItem* sizeItem = new QStandardItem(size); QStandardItem* nameItem = new QStandardItem(fileInfo.fileName());
                    nameItem->setData(result, Qt::UserRole); nameItem->setToolTip(result);
                    QList<QStandardItem*> rowItems; rowItems << nameItem << sizeItem; searchModel->invisibleRootItem()->appendRow(rowItems);
                } ui->treeView->setModel(searchModel); ui->treeView->header()->resizeSection(0, 340); ui->treeView->header()->resizeSection(1, 60);
                QTime elapsed = QTime(0, 0).addSecs(startTime.secsTo(QTime::currentTime())); countItem->setText(countText + " (Elapsed Time: " + elapsed.toString("hh:mm:ss") + ")");
            });
        });
    });
    connect(ui->pushButton, &QPushButton::clicked, this, [=]() {
        QModelIndex rootIndex = ui->treeView->rootIndex(); QString newPath = model->filePath(rootIndex.parent());
        ui->treeView->setModel(model); ui->treeView->setRootIndex(rootIndex.parent()); ui->treeView->setColumnHidden(2, true); ui->treeView->setColumnHidden(3, true);
        model->setRootPath(newPath); ui->usernameLineEdit_2->setText(newPath);
    });
}
void MyOS::taskManager() {
    connect(ui->menuTask_Manager, &QMenu::aboutToShow, this, [this]() {ui->diskInfo->hide(); ui->specs->hide(); ui->tasks->show(); ui->tasks->move(0, 0); ui->settings->hide();});
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        DWORD processIds[1024], bytesReturned;
        if (EnumProcesses(processIds, sizeof(processIds), &bytesReturned)) {
            int numProcesses = bytesReturned / sizeof(DWORD), sections[] = {251, 80, 80, 80, 80};
            for (int i = 0; i < 5; ++i) {ui->taskManager->horizontalHeader()->resizeSection(i, sections[i]);}
            QStringList uniquePrograms; double sum = 0;
            ui->taskManager->clearContents(); ui->taskManager->setRowCount(0); ui->taskManager->insertRow(0);
            for (int i = 0; i < numProcesses; i++) {
                WCHAR processName[MAX_PATH]; DWORD size = sizeof(processName) / sizeof(WCHAR);
                if (QueryFullProcessImageName(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processIds[i]), 0, processName, &size)) {
                    QFileInfo fileInfo(QString::fromWCharArray(processName)); QString programName = fileInfo.fileName(); uniquePrograms.append(programName);
                    QTableWidgetItem* nameItem = new QTableWidgetItem(programName); PROCESS_MEMORY_COUNTERS_EX pmc;
                    if (GetProcessMemoryInfo(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processIds[i]), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc))) {
                        sum += pmc.PeakPagefileUsage / (1024.0 * 1024.0);
                        ui->taskManager->setItem(0, 0, new QTableWidgetItem("Overall")); ui->taskManager->setItem(0, 1, new QTableWidgetItem(QString::number(sum, 'f', 1) + " MB"));
                        QTableWidgetItem* memoryItem = new QTableWidgetItem(QString::number(pmc.PeakPagefileUsage / (1024.0 * 1024.0), 'f', 1) + " MB");
                        int rc = ui->taskManager->rowCount();
                        ui->taskManager->insertRow(rc); ui->taskManager->setItem(rc, 0, nameItem); ui->taskManager->setItem(rc, 1, memoryItem);
                    }
                }
            }
        }
    }); timer->start(1000);
}
void MyOS::settings() {
    connect(ui->menuSettings, &QMenu::aboutToShow, this, [this]() {ui->diskInfo->hide();ui->specs->hide();ui->tasks->hide();ui->settings->show();ui->settings->move(0, 0);});
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, [this]() {QString newValue = ui->lineEdit->text(); ui->storage_2->setText(newValue);});
    connect(ui->lineEdit_3, &QLineEdit::editingFinished, this, [this]() {QString newValue = ui->lineEdit_3->text(); ui->storage_4->setText(newValue);});
    connect(ui->lineEdit_6, &QLineEdit::editingFinished, this, [this]() {QString newValue = ui->lineEdit_6->text(); ui->storage_3->setText(newValue);});
    connect(ui->lineEdit_2, &QLineEdit::editingFinished, this, [this]() {saveShortcuts();});
    connect(ui->lineEdit_4, &QLineEdit::editingFinished, this, [this]() {saveShortcuts();});
    connect(ui->lineEdit_5, &QLineEdit::editingFinished, this, [this]() {saveShortcuts();});
}
void MyOS::saveShortcuts() {
    QFile file(QCoreApplication::applicationDirPath() + "/shortcuts.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {return;}
    QTextStream out(&file); out << ui->lineEdit_2->text() << "\n" << ui->lineEdit_4->text() << "\n" << ui->lineEdit_5->text() << "\n";
}
void MyOS::loadShortcuts() {
    QFile file(QCoreApplication::applicationDirPath() + "/shortcuts.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {saveShortcuts(); return;}
    QTextStream in(&file); QString line;
    if (!(line = in.readLine()).isEmpty()) {ui->lineEdit_2->setText(line);}
    if (!(line = in.readLine()).isEmpty()) {ui->lineEdit_4->setText(line);}
    if (!(line = in.readLine()).isEmpty()) {ui->lineEdit_5->setText(line);}
}
