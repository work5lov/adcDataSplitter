#include "mainwindow.h"
#include "./ui_mainwindow.h"

/// \brief Таймер для измерения времени выполнения функций.
QElapsedTimer timerF;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Установка иконок для вкладок
    QIcon homeIconActive(":/resources/images/homeBlack.png");
    QIcon settingsIconPassive(":/resources/images/settingsBlack.png");
    ui->tabWidget->setTabIcon(0, homeIconActive);
    ui->tabWidget->setTabIcon(1, settingsIconPassive);
    ui->convertProgress->hide();

    // Установка иконки приложения
    QIcon mainIcon(":/resources/images/mainIcon.png");
    this->setWindowIcon(mainIcon);

    ui->CurrentWorkTime_2->hide();

    ui->checkBox->setCheckState(Qt::Checked);
    ui->checkBox_2->setCheckState(Qt::Checked);

    ui->fileDir->setAcceptDrops(true);

    uiTab();

    thread = new QThread();
    worker = new Worker();
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &Worker::processFile);
    connect(worker, &Worker::finished, this, &MainWindow::handleFinished);
    connect(worker, &Worker::finished, thread, &QThread::quit);
    connect(worker, &Worker::progress, this, &MainWindow::setProgressValue);
    //connect(worker, &Worker::finished, worker, &Worker::deleteLater);
    //connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    qRegisterMetaType<QVector<double>>("QVector<double>");

    connect(&progressUpdater, SIGNAL( timeout()), SLOT( updateProgressBar() ) );

    loadSettings();

}

MainWindow::~MainWindow()
{
    delete ui;
}

/// \brief Обработчик события рисования (paintEvent).
///
/// Этот метод рисует главное окно с закругленными углами и сигнализирует о его размерах и положении.
///
/// \param event Событие рисования.
///
void MainWindow::paintEvent(QPaintEvent *event)
{
    // Создание объекта QPainter для рисования
    QPainter painter(this);
    // Установка параметров рендеринга для сглаживания
    painter.setRenderHint(QPainter::Antialiasing);
    // Установка кисти для заливки фона синим цветом
    painter.setBrush(QBrush(Qt::white));
    // Установка пера для границы (но в данном случае делаем её невидимой)
    painter.setPen(Qt::transparent);

    // Получение размеров виджета и создание прямоугольника с закругленными углами
    QRect rect = this->rect();
    rect.setWidth(rect.width());
    rect.setHeight(rect.height());
    // Рисование закругленного прямоугольника
    painter.drawRoundedRect(rect, 0, 0);

    // Вызов родительского метода paintEvent
    QWidget::paintEvent(event);
}

/// \brief Настройка вкладок пользовательского интерфейса.
///
/// Метод для настройки стилей вкладок в пользовательском интерфейсе.
///
void MainWindow::uiTab()
{
    // Установка текущей вкладки и объектных имен для вкладок
    ui->tabWidget->setCurrentWidget(ui->mainTab);
    ui->mainTab->setObjectName("mainTab");
    ui->settingsTab->setObjectName("settingsTab");

    // Настройка стилей для вкладок и их содержимого
    ui->tabWidget->setStyleSheet(
    "QTabBar::tab {"
    "   height: 40px;"
    "   border: none;"
    "   border-top-left-radius: 0px;"
    "   border-top-right-radius: 0px;"
    "   border-bottom-left-radius: 0px;"
    "   border-bottom-right-radius: 0px;"
//        "   padding: 10px;"
    "   background-color: qrgba(0, 0, 0, 0);"
//    "    border: 1px solid;"
//    "    border-color: black;"
    "   icon-size: 40px; /* Установите желаемый размер иконки */"
    "   color: white;"
    "}"

    "QTabBar::tab:hover {"
    "   border-top-left-radius: 0px;"
    "   border-top-right-radius: 0px;"
    "   border-bottom-left-radius: 0px;"
    "   border-bottom-right-radius: 0px;"
    "   background-color: rgb(240, 240, 240);"
    "}"

    "QTabBar::tab:selected {"
    "   background-color: white;"
    "   border-top-right-radius: 0px;"
            "   background-image: url(:/images/SettingsBlue.png);"
    "}"

    "QTabWidget::pane {"
    "   background-color: qrgba(0, 0, 0, 0);"
    "font-family: Trebuchet MS, sans-serif;"
    "}"

    "#mainTab {"
    "   background: white;"
    "font-family: Trebuchet MS, sans-serif;"
    "}"
    "#settingsTab {"
    "   background: white;"
    "font-family: Trebuchet MS, sans-serif;"
    "}"
    "#graphTab {"
    "   background: white;"
    "font-family: Trebuchet MS, sans-serif;"
    "}"
    "#mainTab::tab:selected {"
    "   image: url(qrc:/images/SettingsWhite.png);"
    "}"

    // Настройка стилей для QPushButton
    "QPushButton {"
      "    background-color: white;"
//      "    border-width: 2px;"
      "    border: 1px solid;"
      "    border-color: blue;"
      "    color: black;"
//          "    padding: 10px 20px;"
      "    text-align: center;"
      "    text-decoration: none;"
//                  "    display: inline-block;"
      "    font-size: 16px;"
      "    margin: 4px 2px;"
//                  "    cursor: pointer;"
      "    border-radius: 5px;"
      "font-family: Trebuchet MS, sans-serif;"
      "}"
    "QPushButton::hover {"
            "background-color: rgb(240, 240, 240);"
    "}"

    // Настройка стилей для QLineEdit
    "QLineEdit {"
//          "    padding: 10px;"
    "    border: 1px solid;"
    "border-color: blue;"
    "    font-size: 16px;"
    "    border-radius: 5px;"
//                  "background-color: rgba(255, 255, 255, 100);"
    "}"

    // Настройка стилей для QTextEdit
    "QTextEdit {"
//          "    padding: 10px;"
    "    border: 2px solid;"
    "    border-color: blue;"
    "    font-size: 16px;"
    "    border-radius: 5px;"
//                  "background-color: rgba(255, 255, 255, 100);"
    "}"

    // Настройка стилей для QLabel
    "QLabel {"
            "font-family: Trebuchet MS, sans-serif;"
            "}"

    // Настройка стилей для QComboBox
    "QComboBox {"
        "border: 1px solid blue;"
        "border-radius: 5px;"
        "padding: 2px 0px 2px 3px;"
        "min-width: 6em;"
        "background-color: white;"
        "font-family: Trebuchet MS, sans-serif;"
    "}"
    "QComboBox:hover { "
      "border: 1px solid gray; "
    "}"
    "QComboBox::drop-down {"
        "subcontrol-origin: padding;"
        "subcontrol-position: right;"
        "width: 32px;"
        "border-left-width: 1px;"
//            "border-left-color: gray;"
        "border-left-style: solid;"
        "border-top-right-radius: 5px;"
        "border-bottom-right-radius: 5px;"
        "border-radius: 5px;"
        "background-color: white;"
    "}"
    "QComboBox::down-arrow {"
        "image: url(:/resources/images/comboOpen .png);"
    "}"
    "QComboBox::down-arrow:hover {"
//            "image: url(:/resoruces/images/settingsBlue.png);"
//        "   background-color: qrgba(211, 211, 211, 128);"
    "}"
    "QComboBox::down-arrow:on {"
        "image: url(:/resources/images/comboClose.png);"
    "}"
    "QComboBox QAbstractItemView {"
        "border: 1px solid blue;"
        "background-color: white;"
        "padding-top:4px;padding-bottom:4px;"
        "border-radius: 5px;"
    "}"

    // Настройка стилей для QScrollBar
    "QScrollBar:vertical { background-color: white; "
        "width: 32px; "
        "margin: 15px 0px 15px 0px; "
        "border: none; "
        "border-radius: 5px; "
    "}"
    "QScrollBar::handle:vertical { "
        "background-color: blue; "
        "min-height: 25px; "
        "border-radius: 5px; "
    "}"
    "QScrollBar::sub-line:vertical { "
//            "margin: 3px 0px 3px 0px; "
        "border-image: url(:/resouces/images/comboClose.png); "
        "height: 10px; width: 32px; "
        "subcontrol-position: top; "
        "subcontrol-origin: margin; "
    "}"
    "QScrollBar::add-line:vertical { "
        "margin: 3px 0px 3px 0px; "
        "border-image: url(:/resouces/images/comboOpen .png); "
        "height: 10px; width: 32px; "
        "subcontrol-position: bottom; "
        "subcontrol-origin: margin; "
    "}"
    "QScrollBar::sub-line:vertical:hover,QScrollBar::sub-line:vertical:on { "
//            "border-image: url(); "
        "height: 10px; width: 32px; "
        "subcontrol-position: top; "
        "subcontrol-origin: margin; "
        "background-color: qrgba(211, 211, 211, 128);"
    "}"
    "QScrollBar::add-line:vertical:hover, QScrollBar::add-line:vertical:on { "
//            "border-image: url(:/resouces/images/comboOpen .png); "
        "background-color: qrgba(211, 211, 211, 128);"
        "height: 10px; width: 32px; "
        "subcontrol-position: bottom; "
        "subcontrol-origin: margin; }"
    "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { background: none; }"
    "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }"

    // Настройка стилей для QGroupBox
    "QGroupBox {\
        border: 1px solid black;\
        border-radius: 5px;\
        margin-top: 1em;\
    }\
    \
    QGroupBox::title {\
        subcontrol-origin: margin;\
        subcontrol-position: top left;\
        padding: 0 3px;\
        font-family: 'Trebuchet MS', sans-serif;\
        font-size: 12px;\
        font-weight: bold;\
    }\
    \
    QGroupBox::indicator {\
        width: 12px;\
        height: 12px;\
    }\
    \
    QGroupBox::indicator:checked {\
        image: url(:/resouces/images/homeBlue.png);\
    }\
    \
    QGroupBox::indicator:unchecked {\
        image: url(:/resouces/images/settingsWhite.png);\
    }"

    // Настройка стилей для QCheckBox
    "QCheckBox {\
//        background-color: #fff;\
        border: 1px solid #ccc;\
        border-radius: 1px;\
        color: #333;\
        padding: 5px;\
        margin: 5px;\
        width: 100px;\
        height: 20px;\
        font-size: 14px;\
    }"
    );
}



void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    QIcon settingsIconActive(":/resources/images/settingsBlack.png");
    QIcon settingsIconPassive(":/resources/images/settingsBlack.png");
    QIcon homeIconActive(":/resources/images/homeBlack.png");
    QIcon homeIconPassive(":/resources/images/homeBlack.png");

    if(index == 0){
        ui->tabWidget->setTabIcon(0,homeIconActive);
        ui->tabWidget->setTabIcon(1,settingsIconPassive);
    }

    if(index == 1){
        ui->tabWidget->setTabIcon(0,homeIconPassive);
        ui->tabWidget->setTabIcon(1,settingsIconActive);
    }
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Unchecked)
    {
        ui->choseButton->setEnabled(true);
        ui->outDir->setEnabled(true);
        ui->outDir->setStyleSheet("background-color: white; border-color: blue;");
        ui->choseButton->setStyleSheet("background-color: white; border-color: blue;");
    }
    else
    {
        ui->choseButton->setEnabled(false);
        ui->outDir->setEnabled(false);
        ui->outDir->setStyleSheet("background-color: gray; border-color: gray;");
        ui->choseButton->setStyleSheet("background-color: gray; border-color: gray;");
    }
}


void MainWindow::on_openButton_clicked()
{
    inDir = QFileDialog::getOpenFileName(0,"Выберите файл для открытия","","*.bin");
    ui->fileDir->setText(inDir);
    if(ui->checkBox->isChecked()){
        if (!inDir.isEmpty()) {
            QFileInfo fileInfo(inDir);
            QString directory = fileInfo.absolutePath();
            ui->outDir->setText(directory);
            outDir = directory;
            qDebug() << "Директория файла:" << directory;
        }
    }
}

void MainWindow::on_choseButton_clicked()
{
    outDir = QFileDialog::getExistingDirectory(0," Выберите папку, в которую хотите сохранить проект","");
    ui->outDir->setText(outDir);
}

void MainWindow::on_convertButton_clicked()
{
    QString file1 = ui->firstChanelFileName->text();
    QString file2 = ui->secondChanelFileName->text();
    QString file1Name, file2Name, file1Suffix, file2Suffix;
    if(file1.contains('/'))
    {
        QStringList strs = file1.split('/');
        file1Name = strs[0];
        file1Suffix = strs[1];
    }
    else if(!file1.contains('/')){
        file1Name = file1;
    }
    if(file2.contains('/'))
    {
        QStringList strs = file2.split('/');
        file2Name = strs[0];
        file2Suffix = strs[1];
    }
    else if(!file2.contains('/')){
        file2Name = file2;
    }
    QMap<QString, QString> settings;
    settings.insert("STEP", ui->stepComboBox->currentText());
    settings.insert("inputFile", inDir);
    settings.insert("outDir", outDir);
    settings.insert("file1Name", file1Name);
    settings.insert("file2Name", file2Name);
    settings.insert("file1Suffix", file1Suffix);
    settings.insert("file2Suffix", file2Suffix);

    QString fullString;

    if(ui->comboBox->currentIndex()==0){
        fullString = "bin";
    }
    else{
        fullString = "txt";
    }

    settings.insert("format", fullString);

    worker->setup(settings);
    timerF.start();
    ui->convertProgress->show();
//    ui->CurrentWorkTime_2->show();
    ui->convertProgress->setValue(0);
//    worker->serupIn(inDir);
//    worker->serupOut(outDir);
    progressUpdater.start(500);
    thread->start();
}

/// \brief Обработчик завершения работы потока, отвечающего за обработку файла.
///
/// Этот слот вызывается по завершении работы потока и выполняет необходимые действия,
/// такие как вывод информации о времени выполнения операции, обновление интерфейса и
/// отображение сообщения о завершении преобразования.
///
void MainWindow::handleFinished()
{
    qDebug() << "The slow operation took" << timerF.elapsed() << "milliseconds";
    progressUpdater.stop();
}

void MainWindow::updateProgressBar()
{
    size_t data = worker->getProcessProgress();
    qDebug() << data;
    ui->convertProgress->setValue(data);
}

/// \brief Обработчик события закрытия окна.
///
/// Обработчик события закрытия окна сохраняет настройки программы и завершает работу приложения.
///
/// \param event - событие закрытия окна.
///
void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings(); // Сохранение настроек программы
//    qDebug() << "Программа закрыта";
    event->accept();
}

/// \brief Сохранение настроек программы в файл JSON.
///
/// Функция сохраняет текущие значения QComboBox, QLineEdit и QCheckBox в файл JSON,
/// а также информацию о положении окна, его размере, номере монитора и режиме полноэкранного отображения.
void MainWindow::saveSettings()
{
    QString fileName = "settings.json";

    // Создаем QJsonObject для хранения данных QComboBox, QLineEdit и QCheckBox
    QJsonObject comboBoxData;
    QJsonObject lineEditData;
    QJsonObject checkBoxData;

    // Получаем список всех виджетов на вкладке настроек
    QList<QWidget*> widgets = ui->settingsTab->findChildren<QWidget*>();

    for (int i = 0; i < widgets.size(); ++i) {
        // Проверяем, является ли текущий виджет объектом QComboBox
        if (QComboBox* comboBox = qobject_cast<QComboBox*>(widgets[i])) {
            // Добавляем данные в QJsonObject для QComboBox
            comboBoxData[comboBox->objectName()] = comboBox->currentText();
        }

        // Проверяем, является ли текущий виджет объектом QLineEdit
        if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widgets[i])) {
            // Добавляем данные в QJsonObject для QLineEdit
            lineEditData[lineEdit->objectName()] = lineEdit->text();
        }

        // Проверяем, является ли текущий виджет объектом QCheckBox
        if (QCheckBox* checkBox = qobject_cast<QCheckBox*>(widgets[i])) {
            // Добавляем данные в QJsonObject для QCheckBox
            checkBoxData[checkBox->objectName()] = checkBox->isChecked();
        }
    }

    // Создаем общий QJsonObject для всех данных
    QJsonObject allData;
    allData["ComboBoxData"] = comboBoxData;
    allData["LineEditData"] = lineEditData;
    allData["CheckBoxData"] = checkBoxData;

    // Записываем данные в файл JSON
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // Преобразуем QJsonObject в QJsonDocument и записываем в файл
        QJsonDocument jsonDocument(allData);
        out << jsonDocument.toJson();

        file.close();
    }
}

/// \brief Загрузка настроек программы из файла JSON.
///
/// Функция загружает значения виджетов из файла JSON и устанавливает их в соответствующие виджеты.
///
void MainWindow::loadSettings()
{
    QString fileName = "settings.json";

    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QByteArray jsonData = file.readAll();
        QString jsonString = QString::fromLocal8Bit(jsonData);
//        qDebug() << jsonString;
        file.close();

        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8());

        if (jsonDocument.isNull()) {
            qDebug() << "Ошибка: JSON-документ не удалось распарсить.";
        }

        // Получение QJsonObject из QJsonDocument
        QJsonObject allData = jsonDocument.object();

        // Преобразуем JSON в строку для вывода в консоль
        QString jsonString1 = jsonDocument.toJson(QJsonDocument::Indented);

        // Выводим содержимое JSON в консоль
//        qDebug() << jsonString1;

        // Загрузка данных для QComboBox и установка их в виджеты
        QJsonObject comboBoxData = allData["ComboBoxData"].toObject();
        for (auto it = comboBoxData.begin(); it != comboBoxData.end(); ++it) {
            QString comboBoxName = it.key();
            QString comboBoxText = it.value().toString();

            // Поиск виджета QComboBox по имени
            QComboBox* comboBox = ui->settingsTab->findChild<QComboBox*>(comboBoxName);
            if (comboBox) {
                comboBox->setCurrentText(comboBoxText);
            }
        }

        // Загрузка данных для QLineEdit и установка их в виджеты
        QJsonObject lineEditData = allData["LineEditData"].toObject();
        for (auto it = lineEditData.begin(); it != lineEditData.end(); ++it) {
            QString lineEditName = it.key();
            QString lineEditText = it.value().toString();

            // Поиск виджета QLineEdit по имени
            QLineEdit* lineEdit = ui->settingsTab->findChild<QLineEdit*>(lineEditName);
            if (lineEdit) {
                lineEdit->setText(lineEditText);
            }
        }

        // Загрузка данных для QCheckBox и установка их в виджеты
        QJsonObject checkBoxData = allData["CheckBoxData"].toObject();
        for (auto it = checkBoxData.begin(); it != checkBoxData.end(); ++it) {
            QString checkBoxName = it.key();
            bool checkBoxChecked = it.value().toBool();

            // Поиск виджета QCheckBox по имени
            QCheckBox* checkBox = ui->settingsTab->findChild<QCheckBox*>(checkBoxName);
            if (checkBox) {
                checkBox->setChecked(checkBoxChecked);
            }
        }
    }

    // qDebug() << "Настройки загружены!";
}

void MainWindow::setProgressValue(int value){
    ui->convertProgress->setValue(value);
}
