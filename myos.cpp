#include "myos.h"
#include "./ui_myos.h"

MyOS::MyOS(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyOS)
{
    ui->setupUi(this);
    diskSpace(); specs(); graph(); fileExplorer(); settings(); taskManager();
    setFixedSize(525, 390);
    QIcon appIcon(QCoreApplication::applicationDirPath() + "/Images/icon.png"); setWindowIcon(appIcon);
    setWindowTitle("Pocket Computer Manager");
}

MyOS::~MyOS(){delete ui;}

void MyOS::diskSpace() {
    ULARGE_INTEGER total_size, free_size, used_size;
    if (!GetDiskFreeSpaceEx(NULL, &free_size, &total_size, &used_size)) {}
    used_size.QuadPart = total_size.QuadPart - free_size.QuadPart;
    int used_percent = (int)((double)used_size.QuadPart / (double)total_size.QuadPart * 100);
    QString round_used = QString::number((double) used_size.QuadPart / 1073741824, 'f', 2);
    qint64 images_size = 0, downloads_size = 0, docs_size = 0;
    ui->diskInfo->setStyleSheet("QFrame#diskInfo {border: 1px solid black;}");
    QIcon cpuicon(QCoreApplication::applicationDirPath() + "/Images/cpu.png");
    ui->label_13->setPixmap(cpuicon.pixmap(ui->label_13->size(), QIcon::Normal, QIcon::On));
    QIcon gpuicon(QCoreApplication::applicationDirPath() + "/Images/gpu.png");
    ui->label_20->setPixmap(gpuicon.pixmap(ui->label_14->size(), QIcon::Normal, QIcon::On));
    QIcon ramicon(QCoreApplication::applicationDirPath() + "/Images/ram.png");
    ui->label_14->setPixmap(ramicon.pixmap(ui->label_14->size(), QIcon::Normal, QIcon::On));
    QIcon binIcon(QCoreApplication::applicationDirPath() + "/Images/bin.png");
    ui->clearTrashButton->setIcon(binIcon);
    ui->clearTrashButton->setIconSize(ui->clearTrashButton->size());
    connect(ui->clearTrashButton, &QPushButton::clicked, this, [this, binIcon]() {
        SHEmptyRecycleBin(NULL, NULL, SHERB_NOCONFIRMATION|SHERB_NOPROGRESSUI|SHERB_NOSOUND);
        QIcon successIcon(QCoreApplication::applicationDirPath() + "/Images/success.png");
        ui->clearTrashButton->setIcon(successIcon); ui->clearTrashButton->setIconSize(ui->clearTrashButton->size());
        QCoreApplication::processEvents();
        QTimer::singleShot(3000, this, [this, binIcon]() {
            ui->clearTrashButton->setIcon(binIcon); ui->clearTrashButton->setIconSize(ui->clearTrashButton->size());
        });
    });

    QString perfText = QString("<table><td width=\"50%\" align=\"left\">Performance</td><td width=\"50%\" align=\"right\">100%</td></table>");
    QString condText = QString("<table><td width=\"50%\" align=\"left\">Condition</td><td width=\"50%\" align=\"right\">100%</td></table>");
    QString dataText = QString("<table><td width=\"50%\" align=\"left\">Data Written</td><td width=\"50%\" align=\"right\">X TB</td></table>");
    ui->storage_9->setText(perfText); ui->storage_10->setText(condText); ui->storage_13->setText(dataText);

    QDirIterator it(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
    while (it.hasNext()) {images_size += QFileInfo(it.next()).size();}
    if (images_size < 1073741824) {ui->storage_6->setText(QString::number(images_size / 1024.0 / 1024.0, 'f', 1) + " MB");}
    else {ui->storage_6->setText(QString::number(images_size / 1024.0 / 1024.0 / 1024.0, 'f', 2) + " GB");}

    QDirIterator it1(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
    while (it1.hasNext()) {downloads_size += QFileInfo(it1.next()).size();}
    if (downloads_size < 1073741824) {ui->storage_7->setText(QString::number(downloads_size / 1024.0 / 1024.0, 'f', 1) + " MB");}
    else {ui->storage_7->setText(QString::number(downloads_size / 1024.0 / 1024.0 / 1024.0, 'f', 2) + " GB");}

    QDirIterator it2(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
    while (it2.hasNext()) {docs_size += QFileInfo(it2.next()).size();}
    if (docs_size < 1073741824) {ui->label_6->setText(QString::number(docs_size / 1024.0 / 1024.0, 'f', 1) + " MB");}
    else {ui->label_6->setText(QString::number(docs_size / 1024.0 / 1024.0 / 1024.0, 'f', 2) + " GB");}
    ui->lineEdit_2->setText(it.path()); ui->lineEdit_5->setText(it1.path()); ui->lineEdit_4->setText(it2.path());

    QHBoxLayout* layout = new QHBoxLayout( this ); layout->addWidget(ui->storage_2); layout->addWidget(ui->storage_6);
    layout->setContentsMargins(5,0,5,0);
    ui->storage_2->setStyleSheet("color: #008F00;}");
    ui->pushButton_2->setLayout(layout);

    QHBoxLayout* layout2 = new QHBoxLayout( this ); layout2->addWidget(ui->storage_4); layout2->addWidget(ui->label_6);
    layout2->setContentsMargins(5,0,5,0);
    ui->storage_4->setStyleSheet("color: #6495ED;}");
    ui->pushButton_3->setLayout(layout2);

    QHBoxLayout* layout3 = new QHBoxLayout( this ); layout3->addWidget(ui->storage_3); layout3->addWidget(ui->storage_7);
    layout3->setContentsMargins(5,0,5,0);
    ui->storage_3->setStyleSheet("color: red;}");
    ui->pushButton_4->setLayout(layout3);

    float others = static_cast<float>(static_cast<qint64>(used_size.QuadPart) - downloads_size - images_size - docs_size);
    ui->usedSize->setText(QString::number(used_percent) + "%"); ui->label->setText(round_used + " GB");
    QPieSeries *series = new QPieSeries(); series->setHoleSize(0.75); series->setPieSize(1.0);
    QPieSlice *imagesSlice = series->append("Images", images_size); imagesSlice->setColor(QColor("#008000"));  imagesSlice->setBorderColor(QColor("#008000"));
    QPieSlice *docsSlice = series->append("Docs", docs_size); docsSlice->setColor(QColor("#6495ED"));  docsSlice->setBorderColor(QColor("#6495ED"));
    QPieSlice *downloadsSlice = series->append("Downloads", downloads_size); downloadsSlice->setColor(QColor("#FF0000"));  downloadsSlice->setBorderColor(QColor("#FF0000"));
    QPieSlice *othersSlice = series->append("Others", others); othersSlice->setColor(QColor("#FFA500")); othersSlice->setBorderColor(QColor("#FFA500"));
    qint64 totalSizeQint = (static_cast<qint64>(total_size.u.HighPart) << 32) | total_size.u.LowPart;
    qint64 usedSizeQint = (static_cast<qint64>(used_size.u.HighPart) << 32) | used_size.u.LowPart;
    float remainingSize = static_cast<float>(totalSizeQint - usedSizeQint);
    QPieSlice *remainingSlice = series->append("Remaining", remainingSize); remainingSlice->setColor(Qt::white); series->append(remainingSlice);
    QChart *chart = new QChart(); chart->addSeries(series); chart->setTitle(""); chart->legend()->hide(); chart->setBackgroundBrush(QColor(0, 0, 0, 0));
    QChartView *chartView = new QChartView(chart, ui->circleBar_2); chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setGeometry(0, 0, ui->circleBar_2->width(), ui->circleBar_2->height()); chartView->setParent(ui->circleBar_2);

    QString os_drive = QCoreApplication::applicationFilePath().left(1);
    for (const QStorageInfo& storage : QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady() && !storage.isReadOnly()) {
            QString name = storage.rootPath().left(1);
            qint64 bytes = storage.bytesTotal();
            QString capacity = QString::number(bytes / (1024 * 1024 * 1024.0), 'f', 2) + " GB";
            QString disk = QString("%1 - %2").arg(name, capacity);
            if (name == os_drive) {disk += " - OS";}
            ui->otherDisks->addItem(disk);
            connect(ui->otherDisks, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
                QString diskInfo = ui->otherDisks->itemText(index);
                QString name = diskInfo.section(" - ", 0, 0);
                for (const QStorageInfo& storage : QStorageInfo::mountedVolumes()) {
                    if (storage.isValid() && storage.isReady() && !storage.isReadOnly()) {
                        if (storage.rootPath().left(1) == name) {
                            QString usedCapacity = QString::number((storage.bytesTotal() - storage.bytesAvailable()) / (1024 * 1024 * 1024.0), 'f', 2) + " GB";
                            QString usedAvgPercent = QString::number((storage.bytesTotal() - storage.bytesAvailable()) / storage.bytesTotal() * 100);
                            ui->label->setText(usedCapacity); ui->usedSize->setText(usedAvgPercent + "%");
                        }
                    }
                }
            });
        }
    }
    connect(ui->menuStorage, &QMenu::aboutToShow, this, [this]() {ui->diskInfo->show();ui->specs->hide();ui->tasks->hide();ui->settings->hide();});
}
void MyOS::specs() {
    ui->specs->setStyleSheet("QFrame#specs { border: 1px solid black; }");
    ui->label_11->setText("Specification of " + QSysInfo::machineHostName());
    unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0, id = 0x80000002;
    char name[49] = {0};
    for (int i = 0; i < 3; i++) {
        __cpuid(id + i, eax, ebx, ecx, edx);
        memcpy(&name[i * 16], &eax, sizeof(eax)); memcpy(&name[i * 16 + 4], &ebx, sizeof(ebx));
        memcpy(&name[i * 16 + 8], &ecx, sizeof(ecx)); memcpy(&name[i * 16 + 12], &edx, sizeof(edx));
    }
    std::string cpuName(name);
    QString cpuType = QString::fromStdString(cpuName);
    cpuType.remove("(R)"); cpuType.remove("(TM)"); cpuType.remove(" CPU"); cpuType.replace("@", "-");
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
    DWORD returnedLength = 0;
    GetLogicalProcessorInformation(buffer, &returnedLength);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnedLength);
        if (GetLogicalProcessorInformation(buffer, &returnedLength)) {
            int numCores = 0;
            for (DWORD i = 0; i < returnedLength / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++) {
                if (buffer[i].Relationship == RelationProcessorCore) {numCores++;}
            }
            int numThreads = QThread::idealThreadCount();
            QString cpuInfo = QString("%1, %2 Cores, %3 Threads").arg(cpuType).arg(numCores).arg(numThreads);
            ui->label_12->setText(cpuInfo);
        }
    }
    MEMORYSTATUSEX status; status.dwLength = sizeof(status); GlobalMemoryStatusEx(&status);
    unsigned long long installedRAM = status.ullTotalPhys;
    double installedRAMinGB = installedRAM / (1024.0 * 1024.0 * 1024.0);
    int installedRAMint = qRound(installedRAMinGB);
    ui->label_21->setText(QString("Installed memory: %1 GB (%2 GB Useable)").arg(installedRAMint).arg(QString::number(installedRAMinGB, 'f', 1)));
    connect(ui->menuSpecs, &QMenu::aboutToShow, this, [this]() {
        ui->diskInfo->hide();ui->specs->show();ui->tasks->hide();ui->specs->move(5, 5);ui->settings->hide();
    });
    QString gpuType = getGraphicsCardType(); gpuType.remove("(R)"); ui->label_15->setText(gpuType);
}
void MyOS::graph() {
    QLineSeries *series = new QLineSeries();
    QTimer *timer = new QTimer(this); timer->start(1000);
    int i = 0; bool isRAMUsage = false;
    QLabel *label = ui->label_29;
    connect(timer, &QTimer::timeout, this, [series, &i, &isRAMUsage, label]() {
        if (i < 61) {
            if (isRAMUsage == true) {
                MEMORYSTATUSEX status; status.dwLength = sizeof(status); GlobalMemoryStatusEx(&status);
                unsigned long long totalRAM = status.ullTotalPhys; unsigned long long usedRAM = totalRAM - status.ullAvailPhys;
                double usedRAMPercent = (static_cast<double>(usedRAM) / totalRAM) * 100.0; double usedRAMinGB = usedRAM / (1024.0 * 1024.0 * 1024.0);
                label->setText(QString("RAM usage in the past minute (Current: %1% - %2 GB)").arg(usedRAMPercent, 0, 'f', 2).arg(usedRAMinGB, 0, 'f', 2));
                series->append(i, usedRAMPercent);
            } else {double randomValue = QRandomGenerator::global()->bounded(0, 101);series->append(i, randomValue);} ++i;
        } else {i = 0; series->clear();}
    });
    QChart *chart = new QChart();
    chart->addSeries(series); chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).at(0)->setRange(0, 60); chart->axes(Qt::Vertical).at(0)->setRange(0, 100);
    chart->axes(Qt::Horizontal).at(0)->setLabelsVisible(false); chart->axes(Qt::Vertical).at(0)->setLabelsVisible(false);
    chart->legend()->hide(); chart->setBackgroundBrush(QColor(0, 0, 0, 0)); chart->setMargins(QMargins(-4, -7, -3, -5));
    QChartView *chartView = new QChartView(chart); chartView->setRenderHint(QPainter::Antialiasing);
    QVBoxLayout *layout = new QVBoxLayout(ui->usage); layout->setContentsMargins(0, 0, 0, 0); layout->setSpacing(3);
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *cpuButton = new QPushButton("CPU"); QPushButton *gpuButton = new QPushButton("GPU"); QPushButton *ramButton = new QPushButton("RAM");
    buttonsLayout->setContentsMargins(5, 7, 5, 0);
    buttonsLayout->addWidget(cpuButton); buttonsLayout->addWidget(gpuButton); buttonsLayout->addWidget(ramButton);
    layout->addLayout(buttonsLayout);
    layout->addWidget(chartView);
    connect(cpuButton, &QPushButton::clicked, this, [series, &i, label, &isRAMUsage](bool) {
        i = 0; series->clear(); isRAMUsage = false; label->setText("CPU usage in the past minute");
    });
    connect(gpuButton, &QPushButton::clicked, this, [series, &i, label, &isRAMUsage](bool) {
        i = 0; series->clear(); isRAMUsage = false; label->setText("GPU usage in the past minute");
    });
    connect(ramButton, &QPushButton::clicked, this, [series, &i, &isRAMUsage](bool) {i = 0; series->clear(); isRAMUsage = true;});
}
void MyOS::fileExplorer() {
    ui->treeView->setStyleSheet("QTreeView#treeView { border: 0px solid black; background-color: transparent;}");
    QFileSystemModel* model = new QFileSystemModel; model->setRootPath(QDir::currentPath()); ui->treeView->setModel(model);
    ui->treeView->header()->resizeSection(0, 250); ui->treeView->header()->resizeSection(1, 70);
    ui->treeView->setColumnHidden(2, true); ui->treeView->setColumnHidden(3, true);
    ui->treeView->setRootIsDecorated(false);
    connect(ui->treeView, &QTreeView::doubleClicked, this, [=](const QModelIndex& index) {
        if (index.isValid()) { ui->usernameLineEdit_2->setText(model->filePath(index)); }
        if (model->isDir(index)) {model->setRootPath(model->filePath(index)); ui->treeView->setRootIndex(index);}
        else {QDesktopServices::openUrl(QUrl::fromLocalFile(model->filePath(index)));}
    });
    QMenu* fileMenu = new QMenu(ui->treeView);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    QAction* deleteAction = new QAction("Delete", ui->treeView); QAction* cutAction = new QAction("Cut", ui->treeView);
    QAction* copyAction = new QAction("Copy", ui->treeView); QAction* renameAction = new QAction("Rename", ui->treeView);
    QAction* pasteAction = new QAction("Paste", ui->treeView);
    fileMenu->addAction(deleteAction); fileMenu->addAction(cutAction); fileMenu->addAction(copyAction);
    fileMenu->addAction(renameAction); fileMenu->addAction(pasteAction);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, [=](const QPoint& pos) {
        QModelIndex index = ui->treeView->indexAt(pos);
        if (index.isValid()) {
            QPoint globalPos = ui->treeView->mapToGlobal(pos);
            deleteAction->setEnabled(true); cutAction->setEnabled(true); copyAction->setEnabled(true);
            renameAction->setEnabled(true); pasteAction->setEnabled(true); fileMenu->exec(globalPos);
        }else {
            deleteAction->setEnabled(false); cutAction->setEnabled(false); copyAction->setEnabled(false);
            renameAction->setEnabled(false); pasteAction->setEnabled(true); fileMenu->exec(ui->treeView->mapToGlobal(pos));
        }
    });
    ui->tabWidget->tabBar()->setStyleSheet("QTabBar::tab {width: 100px; height: 20px;} QTabBar::tab:!selected {padding-top: -1px; border: 0.5px solid #E5E5E5; border-bottom: none;}");
    QIcon addIcon(QCoreApplication::applicationDirPath() + "/Images/add.png");
    QIcon closeIcon(QCoreApplication::applicationDirPath() + "/Images/close.png");
    int i = 0;
    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, [=](int index) mutable {if (index == i) {ui->tabWidget->setCurrentIndex(index + 1);}});
    ui->addButton->setStyleSheet("background-color: transparent; border: none;");
    ui->addButton->setIconSize(ui->addButton->size()); ui->addButton->setIcon(addIcon);
    connect(ui->addButton, &QPushButton::clicked, this, [=]() mutable { i++;
        QWidget *newTab = new QWidget;
        ui->tabWidget->insertTab(1, newTab, tr("New Tab"));
        QPushButton *closeButton = new QPushButton("", newTab);
        closeButton->setFixedSize(16, 16);
        closeButton->setStyleSheet("background-color: transparent; border: none;");
        closeButton->setIconSize(closeButton->size()); closeButton->setIcon(closeIcon);
        ui->tabWidget->tabBar()->setTabButton(1, QTabBar::RightSide, closeButton);
        connect(closeButton, &QPushButton::clicked, this, [=]() mutable {
            int tabIndex = ui->tabWidget->indexOf(newTab);
            ui->tabWidget->removeTab(tabIndex); i--;
            QPoint addButtonPos = ui->addButton->pos(); ui->addButton->move(addButtonPos.x() - 102, addButtonPos.y());
        });
        ui->tabWidget->setCurrentIndex(i + 1);
        QPoint addButtonPos = ui->addButton->pos(); ui->addButton->move(addButtonPos.x() + 102, addButtonPos.y());
    });
    connect(deleteAction, &QAction::triggered, this, [=]() {
        QModelIndex index = ui->treeView->currentIndex();
        if (index.isValid()) {QString filePath = model->filePath(index); QFile::remove(filePath); QDir().rmdir(filePath);}
    });

    //HAVE TO DO CUT, COPY AND PASTE ACTION!

    connect(renameAction, &QAction::triggered, this, [=]() {
        QModelIndex index = ui->treeView->currentIndex();
        if (index.isValid()) {
            QString currentFilePath = model->filePath(index); QString currentFileName = model->fileName(index);
            QString newFileName = QInputDialog::getText(this, tr("Rename"), tr("File's New Name:"), QLineEdit::Normal, currentFileName);
            QString newPath = QDir(model->fileInfo(index).path()).filePath(newFileName);
            if (QFile::rename(currentFilePath, newPath)) {}
        }
    });
    ui->usernameLineEdit_2->setStyleSheet("border: 1px solid #E5E5E5;");
    connect(ui->usernameLineEdit_2, &QLineEdit::editingFinished, this, [=]() {
        QString newPath = ui->usernameLineEdit_2->text();
        QFileInfo fileInfo(newPath);
        if (fileInfo.exists() && fileInfo.isDir()) {model->setRootPath(newPath); ui->treeView->setRootIndex(model->index(newPath));}
        else if (fileInfo.exists()) {QDesktopServices::openUrl(QUrl::fromLocalFile(newPath));}
    });
    ui->usernameLineEdit_3->setStyleSheet("border: 1px solid #E5E5E5;");
    ui->pushButton->setStyleSheet("border: 1px solid #E5E5E5; background-color: #FFFFFF;");
    QStandardItemModel* searchModel;
    searchModel = new QStandardItemModel(0, 2, this);
    connect(ui->usernameLineEdit_3, &QLineEdit::returnPressed, this, [=]() {
        QString searchTerm = ui->usernameLineEdit_3->text();
        QTime startTime = QTime::currentTime();
        QThreadPool::globalInstance()->start([=]() {
            searchModel->removeRows(0, searchModel->rowCount());
            QStringList results;
            QMutex resultsMutex;
            std::function<void(const QDir&)> searchFiles;
            searchFiles = [&](const QDir& dir) {
                QDir::Filters filters = QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden;
                QDir::SortFlags sortFlags = QDir::Name;
                QFileInfoList files = dir.entryInfoList(QStringList() << "*", filters, sortFlags);
                for (const QFileInfo& file : files) {
                    if (file.fileName().contains(searchTerm, Qt::CaseInsensitive)) {QMutexLocker locker(&resultsMutex);results << file.filePath();}
                    if (file.isDir()) {searchFiles(QDir(file.filePath()));}
                }
            };
            searchFiles(QDir(model->rootPath()));
            QMetaObject::invokeMethod(this, [=]() {
                QString countText = tr("%1 results for \"%2\"").arg(results.size()).arg(searchTerm);
                QStandardItem* rootItem = searchModel->invisibleRootItem(); QStandardItem* countItem = new QStandardItem(countText);
                countItem->setSelectable(false);
                rootItem->appendRow(countItem);
                for (const QString& result : results) {
                    QFileInfo fileInfo(result);
                    QString name = fileInfo.fileName(), size = "Folder";
                    if (fileInfo.isFile()) {
                        qint64 fileSize = fileInfo.size();
                        if (fileSize < 1024 * 1024) { size = QString("%1 kB").arg(fileSize / 1024); }
                        else if (fileSize < 1024 * 1024 * 1024) { size = QString("%1 MB").arg(fileSize / 1024 / 1024); }
                        else { size = QString("%1 GB").arg(fileSize / 1024 / 1024 / 1024); }
                    }
                    QStandardItem* sizeItem = new QStandardItem(size); QStandardItem* nameItem = new QStandardItem(name);
                    nameItem->setData(result, Qt::UserRole); nameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); nameItem->setToolTip(result);
                    QList<QStandardItem*> rowItems; rowItems << nameItem << sizeItem; rootItem->appendRow(rowItems);
                }
                ui->treeView->setModel(searchModel); ui->treeView->header()->resizeSection(0, 280); ui->treeView->header()->resizeSection(1, 60);
                QTime endTime = QTime::currentTime(); int elapsedTime = startTime.secsTo(endTime);
                int hours = elapsedTime / 3600; int minutes = (elapsedTime % 3600) / 60; int seconds = elapsedTime % 60;
                QString elapsedTimeString = QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
                countItem->setText(countText + " (Elapsed Time: " + elapsedTimeString + ")");
            });
        });
    });
    connect(ui->pushButton, &QPushButton::clicked, this, [=]() {
        if (searchModel->rowCount() > 0) {
            ui->treeView->setModel(model); ui->treeView->setRootIndex(QModelIndex()); ui->treeView->setColumnHidden(2, true); ui->treeView->setColumnHidden(3, true);
            QString newPath = model->rootPath();
            ui->usernameLineEdit_2->setText(newPath); model->setRootPath(newPath);
        } else {
            QModelIndex parentIndex = ui->treeView->rootIndex().parent(); QString newPath;
            if (parentIndex.isValid()) {newPath = model->filePath(parentIndex);ui->treeView->setRootIndex(parentIndex);}
            else {newPath = model->rootPath();ui->treeView->setRootIndex(QModelIndex());}
            model->setRootPath(newPath); ui->usernameLineEdit_2->setText(newPath);
        }
    });
    QPixmap backicon(QCoreApplication::applicationDirPath() + "/Images/back_arrow.png");
    ui->pushButton->setIconSize(ui->pushButton->size()); ui->pushButton->setIcon(backicon);
}
void MyOS::taskManager() {
    ui->tasks->setStyleSheet("QFrame#tasks { border: 1px solid black; background-color: transparent;}");
    connect(ui->menuTask_Manager, &QMenu::aboutToShow, this, [this]() {
        ui->diskInfo->hide();ui->specs->hide();ui->tasks->show();ui->tasks->move(5, 5);ui->settings->hide();
    });
    DWORD processIds[1024], bytesReturned;
    if (EnumProcesses(processIds, sizeof(processIds), &bytesReturned)) {
        int numProcesses = bytesReturned / sizeof(DWORD);
        ui->taskManager->clear();
        ui->taskManager->setRowCount(0); ui->taskManager->setColumnCount(2);
        ui->taskManager->setHorizontalHeaderLabels({"Program's Name", "Memory Usage"});
        ui->taskManager->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        ui->taskManager->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        ui->taskManager->horizontalHeader()->resizeSection(0, 350); ui->taskManager->horizontalHeader()->resizeSection(1, 160);
        ui->taskManager->verticalHeader()->setVisible(false);
        ui->taskManager->setStyleSheet("QTableWidget { border: none; background-color: transparent; } QTableWidget::item { border: none; background-color: transparent; }");
        QStringList uniquePrograms;
        for (int i = 0; i < numProcesses; i++) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processIds[i]);
            if (hProcess) {
                WCHAR processName[MAX_PATH];
                DWORD size = sizeof(processName) / sizeof(WCHAR);
                if (QueryFullProcessImageName(hProcess, 0, processName, &size)) {
                    QString processNameStr = QString::fromWCharArray(processName);
                    QFileInfo fileInfo(processNameStr);
                    QString programName = fileInfo.fileName();
                    if (!programName.isEmpty() && !uniquePrograms.contains(programName)) {
                        uniquePrograms.append(programName);
                        QTableWidgetItem* nameItem = new QTableWidgetItem(programName);
                        PROCESS_MEMORY_COUNTERS_EX pmc;
                        if (GetProcessMemoryInfo(hProcess, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc))) {
                            double memoryUsageMB = static_cast<double>(pmc.PrivateUsage) / (1024.0 * 1024.0);
                            QTableWidgetItem* memoryItem = new QTableWidgetItem(QString::number(memoryUsageMB, 'f', 2) + " MB");
                            int rowCount = ui->taskManager->rowCount();
                            ui->taskManager->insertRow(rowCount); ui->taskManager->setItem(rowCount, 0, nameItem);
                            ui->taskManager->setItem(rowCount, 1, memoryItem);
                        }
                    }
                } CloseHandle(hProcess);
            }
        }
    }
}
void MyOS::settings() {
    ui->settings->setStyleSheet("QFrame#settings { border: 1px solid black; background-color: transparent;}");
    connect(ui->menuSettings, &QMenu::aboutToShow, this, [this]() {
        ui->diskInfo->hide();ui->specs->hide();ui->tasks->hide();ui->settings->show();ui->settings->move(5, 5);
    });
    connect(ui->checkBox_5, &QCheckBox::stateChanged, this, [=](int state) {
        int flags = windowFlags();
        if (state == Qt::Checked) {flags |= Qt::WindowStaysOnTopHint;} else {flags &= ~Qt::WindowStaysOnTopHint;}
        setWindowFlags(static_cast<Qt::WindowFlags>(flags)); show();
    });
    connect(ui->clearTrashButton_2, &QPushButton::clicked, this, [this]() {
        ui->checkBox_4->setChecked(false);ui->checkBox_5->setChecked(false);
        QDirIterator it(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
        QDirIterator it1(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
        QDirIterator it2(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
        ui->lineEdit_2->setText(it.path());ui->lineEdit_5->setText(it1.path());ui->lineEdit_4->setText(it2.path());
        ui->lineEdit->setText("Pictures");ui->lineEdit_3->setText("Documents");ui->lineEdit_6->setText("Downloads");
    });
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, [=]() {QString newValue = ui->lineEdit->text();ui->storage_2->setText(newValue);});
    connect(ui->lineEdit_3, &QLineEdit::editingFinished, this, [=]() {QString newValue = ui->lineEdit_3->text();ui->storage_4->setText(newValue);});
    connect(ui->lineEdit_6, &QLineEdit::editingFinished, this, [=]() {QString newValue = ui->lineEdit_6->text();ui->storage_3->setText(newValue);});
}
