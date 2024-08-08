#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QFile>
#include <QFileInfo>
#include <QtMath>
#include <QTextStream>
#include <QString>
#include <QDialog>
#include <QFileDialog>
#include <QDate>
#include <QDebug>
#include <QtEndian>
#include <QProgressBar>
#include <QMessageBox>
#include <QThread>
#include <QVector>
#include <cmath>
#include <algorithm>
#include <QTransform>
#include <complex>
#include <QElapsedTimer>
#include <QMetaType>

#include "worker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    void handleFinished();

private slots:

    void paintEvent(QPaintEvent *event);
    void uiTab();

    void on_tabWidget_tabBarClicked(int index);

    void on_checkBox_stateChanged(int arg1);

    void on_openButton_clicked();

    void on_choseButton_clicked();

    void on_convertButton_clicked();

private:
    Ui::MainWindow *ui;

    QThread *thread;
    Worker *worker;

    QString inDir;
    QString outDir;
};
#endif // MAINWINDOW_H
