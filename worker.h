/*!
\file
\brief Заголовочный файл с описанием класса, выполняющего обработку исходных данных АЦП.

Данный файл содержит в себе определения основных
сигналов, слотов и атрибутов, используемых при
обработке данных АЦП. Обработка заключается в преобразовании исходных кодов в напряжение.
*/

#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include "functions.h"

struct MyData {
    QVector<float> data1; ///< Данные с первого канала АПЦ
    QVector<float> data2; ///< Данные со второго канала АПЦ
    typedef QList<float>::const_iterator const_iterator; ///< Итератор
};

class Worker : public QObject
{
    Q_OBJECT

public:
    Worker();
    size_t getProcessProgress();
    //double STEP;

public slots:
    void processFile();
    void pause();
    void stop();
    void setupStep(double step);
    void setupMode(QString str);
    void setupSubName(QString str);
    void serupIn(QString in);
    void serupOut(QString out);
    void setup(QMap<QString,QString> settings);
//    MyData partsProcess(const QByteArray &chunk);
//    double STEP;

signals:
    void progress(int value);///< Сигнал для передачи прогресса выполнения обработки файла \warning Не используется.
    void finished();///< Сигнал для передачи окончания обработки файла
    void setFileIn(QString in);///< Сигнал для передачи пути входного файла \warning Не используется.
    void setFileOut(QString out);///< Сигнал для передачи директории выходных файлова \warning Не используется.
    void elapsedTimer(QString str);///< Сигнал для передачи времени выполнения обработки файла \warning Не используется.

private slots:
    float convertFunc(float data);
//    void serupIn(QString in);
//    void serupOut(QString out);



private:
    QString inFilePath;///< Глобальная переменная для хранения пути к входному файлу
    QString outDir;///< Глобальная переменная для хранения пути к выходным файлам
    int STEP;///< Глобальная переменная для хранения шага чтения файла
    QString MODE;///< Глобальная переменная для хранения режима чтения файла
                ///< \warning не испольхуется
    QString subN;///< Глобальная переменная для хранения пути к входному файлу
    bool paused;
    bool stopped;
    QString outFile1Name;
    QString outFile1Suffix;
    QString outFile2Name;
    QString outFile2Suffix;
    QString format;

    static void setProgress(int data);
};

#endif // WORKER_H
