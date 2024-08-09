#include "worker.h"
#include <QFile>
#include <QTextStream>
#include <QtEndian>
#include <QDebug>
#include <QDate>
#include <QElapsedTimer>
#include <windows.h>
#include <iostream>

Worker::Worker(){
//    connect(this, SIGNAL(setFileIn(QString)), this, SLOT(serupIn(QString)));
    //    connect(this, SIGNAL(setFileOut(QString)), this, SLOT(serupOut(QString)));
}

QString inFilePath1; ///< Глобальная переменная для хранения пути к входному файлу
size_t readedData = 0;
size_t totalSize;

size_t Worker::getProcessProgress()
{
    qDebug() << readedData << totalSize << (readedData * 100 )/ totalSize << (readedData / totalSize) * 100;
    return static_cast<int>((static_cast<double>(readedData) / totalSize) * 100);
}


/// Структура для сохранения обработанных данных АЦП
//struct MyData {
//    QVector<float> data1; ///< Данные с первого канала АПЦ
//    QVector<float> data2; ///< Данные со второго канала АПЦ
//    typedef QList<float>::const_iterator const_iterator; ///< Итератор
//};

/*!
\brief Разделяет исходный файл на части
\param[in] fileName Путь к исходному файлу
\return Список частей типа QByteArray
*/
QList<QByteArray> readAndSplitFile(const QString& fileName, int step)
{
    QElapsedTimer timer;///<
    timer.start();
    QList<QByteArray> parts; ///< Выходная переменная
    const int blockSize = 1024; ///< Переменная, задающая размер блока
    qint64 counter = 0; ///< Счетчик, подсчитывающий номер читаемого блока
    QFile file(fileName);
//    qDebug()<<"filename"<<fileName;
    if (!file.open(QIODevice::ReadOnly))///< Открываем файл для чтения и проверяем можем ли прочитать
    {
        qDebug() << "Error: Failed to open input file for reading!";
    }
    // Читаем файл блоками по 1024 байта
    while(!file.atEnd()){
        QByteArray block = file.read(blockSize);///< Читаем блок
        counter++;
        if(counter%step==0){
            //записываем каждый 10 блок в вектор
//            readedData = counter * 1024;
            parts.append(block);///< Если значение счетчика кратно 10, то записываем данные в список
//            qDebug()<<counter;
        }
    }
    file.close();
    qDebug() << "Time per byte:" << file.size()/(timer.elapsed()/1024) << "b/s"<<timer.elapsed();
    return parts;
}

QList<QByteArray> splitData(const QByteArray &data, uint8_t parts) {
    QList<QByteArray> result;
    int totalSize = data.size();

    // Если количество частей больше размера данных, просто добавим их как есть
    if (parts <= 0 || totalSize == 0) {
        return result;
    }

    int partSize = totalSize / parts;
    int remainder = totalSize % parts;

    int offset = 0;
    for (int i = 0; i < parts; ++i) {
        int currentPartSize = partSize + (i < remainder ? 1 : 0); // Распределение остатка
        result.append(data.mid(offset, currentPartSize));
        offset += currentPartSize;
    }

    return result;
}

QList<QByteArray> readAndSplitFile(const QByteArray &data, int partSizeInMB) {
    QList<QByteArray> parts;
    int partSizeInBytes = partSizeInMB * 1024 * 1024;  // перевод размера в мегабайтах в байты
    int totalSize = data.size();

    for (int i = 0; i < totalSize; i += partSizeInBytes) {
        parts.append(data.mid(i, partSizeInBytes));
    }
//    qDebug() << parts;

    return parts;
}

/*!
\brief Обработчик части файла
\param[in] chunk Часть данных АЦП
\return Преобразованные данные из кода напряжения в напряжение
*/
MyData partsProcess(const QByteArray &chunk){
    MyData d_out;
    readedData += chunk.size();
    for (int i = 0; i < chunk.size(); i += 4) {///< Читаем по 4 байта
        // Extract the two 2-byte values from the chunk

//        qDebug() << readedData << totalSize << readedData/totalSize;
//        Worker::progress(readedData/totalSize);
        int val1 = (chunk.at(i) << 8) | chunk.at(i + 1);
        int val2 = (chunk.at(i + 2) << 8) | chunk.at(i + 3);
        val1 = val1 & 0x00000000000ffff;
        int16_t b1 = qToBigEndian<qint16>(val1);
        int16_t b2 = qToBigEndian<qint16>(val2);
        // Convert the values to volts
        float volts1 = convertF(b1);
        float volts2 = convertF(b2);
//        qDebug()<<volts1;
        // Add the volt values to the corresponding lists
//        data.insert(volts1,volts2);
        d_out.data1.append(volts1);
        d_out.data2.append(volts2);
    }
    return d_out;
}

/*!
\brief Обработчик части файла
\param[in] w Значения АЦП одной части исходного файла
\param[out] result Собранные данные АЦП всех обработанных частей
*/
void reduce(MyData &result, const MyData &w){
    result.data1.append( w.data1 );
    result.data2.append( w.data2 );
}

// Функция для подбора оптимального размера части файла
size_t calculateOptimalChunkSize(size_t freeMemoryMB, size_t numThreads, size_t fileSizeMB) {
    // Пример вычисления оптимального размера куска. Это можно настроить по мере необходимости.
    return std::min(fileSizeMB / numThreads, freeMemoryMB / numThreads);
}

/*!
\brief Основной цикл работы обработчика файлов

Данный метод выполняет основную работу обработчика файлов:
разделение исходного файла на части,
параллельная обработка частей,
синхронизация обработки и сбора частей в единую струкутуру
с целью получения корректных выходных файлов.

Исходный файл имеет формат .bin. Данные записаны подряд со следующим чередованием:
АЦП_1_КаналАЦП_2_Канал...
Итоговые файлы имеют формат .bin. Данные записаны в столбец

*/
void Worker::processFile()
{
    if(STEP == 100){
        qDebug()<<"Директория входного файла"<<inFilePath1;
        /// \brief Проверка существования входного файла.
        if (!QFile::exists(inFilePath1)) {
            qDebug() << "Error: Input file does not exist!";
            return;
        }

        QFile file(inFilePath1);

        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open file" << inFilePath1;
    //        return parts;
        }

        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof(statex);
        DWORDLONG freePhysicalMemory;

        if (GlobalMemoryStatusEx(&statex)) {
            // Получение объема свободной физической памяти и вывод в мегабайтах
            freePhysicalMemory = statex.ullAvailPhys;
    //            std::cout << "Free physical memory: " << freePhysicalMemory  << " MB" << std::endl;
        } else {
            std::cerr << "Error getting memory status: " << GetLastError() << std::endl;
        }

    //        std::cout << "Free physical memory: " << freePhysicalMemory / (1024 * 1024) << " MB" << std::endl;

        static const int THREAD_COUNT = QThread::idealThreadCount();
    //        std::cout << "Ideal thread count: " << THREAD_COUNT << std::endl;
        // Делим на 3
        size_t dividedMemory = freePhysicalMemory / 3;

        // Константа для 1 мегабайта
        const size_t oneMB = 1024 * 1024;

        // Доводим до ближайшего числа, кратного 1 МБ, в большую сторону
        size_t adjustedMemory = ((dividedMemory + oneMB - 1) / oneMB) * oneMB;

    //        std::cout << "Free physical memory: " << freePhysicalMemory / (1024 * 1024) << " MB" << std::endl;
    //        std::cout << "Adjusted memory size: " << adjustedMemory / (1024 * 1024) << " MB" << std::endl;
    //        std::cout << "Chunk size: " << 6144 / (6144  / (adjustedMemory / (1024 * 1024) / 10)) << " MB" << std::endl;

        size_t freeMemoryMB = adjustedMemory / (1024 * 1024);  // Пример: 6 ГБ свободной памяти
        size_t numThreads = THREAD_COUNT - 2;       // Пример: 4 потока
        size_t fileSizeMB = file.size() / (1024 * 1024);   // Пример: 20 ГБ файл

        totalSize = file.size();

        size_t optimalChunkSizeMB = calculateOptimalChunkSize(freeMemoryMB, numThreads, fileSizeMB);

        size_t partSizeInBytes = optimalChunkSizeMB * 1024 * 1024;  // размер части в байтах
        size_t readChunkSize = partSizeInBytes * (THREAD_COUNT - 2);  // размер считываемого куска в байтах
    //    qDebug() << "readChunkSize" << readChunkSize;

        while(!file.atEnd()){
            /// \brief Инициализация синхронизатора для ожидания завершения выполнения задач.
            QFutureSynchronizer<void> synchronizer;

            size_t readSize = 512 ;//1*1024

//            progress(fileSizeMB);

            QByteArray data = file.read(readSize*1024*1024);

            /// \brief Чтение и разделение входного файла на массив байт.
            QList<QByteArray> BAlist = readAndSplitFile(data, readSize/10);
    //        qDebug()<<BAlist.size();

            /// \brief Создание и запуск задачи в фоновом режиме для обработки данных.
            QFuture<MyData> future = QtConcurrent::mappedReduced(BAlist,partsProcess,reduce);
            // Дождаться окончания выполнения
            synchronizer.addFuture(future);
            synchronizer.waitForFinished();

            /// \brief Получение результата выполнения задачи.
            const MyData result = future.result();

            // Вычисление текущего прогресса
            /*qint64 processedSize = file.pos(); // Размер уже обработанных данных
            int progressT = static_cast<int>((static_cast<double>(processedSize) / totalSize) * 100);

            // Обновление прогресса
            emit progress(progressT)*/;

            /// \brief Создаем объект QFileInfo, используя путь к файлу
            QFileInfo fileInfo(inFilePath1);

            /// \brief Извлекаем имя файла без расширения
            QString fileNameWithoutExtension = fileInfo.baseName();

            /// \brief Генерация путей к выходным файлам с уникальными временными метками.
            QString outFile1Path = outDir + "/" + outFile1Name + fileNameWithoutExtension + "." + format;
            QString outFile2Path = outDir + "/" + outFile2Name + fileNameWithoutExtension + "." + format;

            /// \brief Создание выходных файлов.
            QFile outFile1(outFile1Path);
            QFile outFile2(outFile2Path);

            /// \brief Создание потоков данных для записи в выходные файлы в формате Little Endian.
            QDataStream outStream1(&outFile1);
            QDataStream outStream2(&outFile2);
            outStream1.setByteOrder(QDataStream::LittleEndian);
            outStream2.setByteOrder(QDataStream::LittleEndian);

            /// \brief Проверка успешного открытия выходного файла 1 для записи.
            if (!outFile1.open(QIODevice::Append)) {
                qDebug() << "Error: Failed to open output file 1 for writing!";
                return;
            }

            /// \brief Проверка успешного открытия выходного файла 2 для записи.
            if (!outFile2.open(QIODevice::Append)) {
                qDebug() << "Error: Failed to open output file 2 for writing!";
                return;
            }
            /// \brief Запись данных в выходные файлы.
            if (format == "bin") {
                // Запись в бинарные файлы с использованием QDataStream
                QDataStream outStream1(&outFile1);
                QDataStream outStream2(&outFile2);

                outStream1.setByteOrder(QDataStream::LittleEndian);
                outStream2.setByteOrder(QDataStream::LittleEndian);

                for (float volts : qAsConst(result.data1)) {
                    outStream1.writeRawData(reinterpret_cast<const char*>(&volts), sizeof(float));
                }

                for (float volts : qAsConst(result.data2)) {
                    outStream2.writeRawData(reinterpret_cast<const char*>(&volts), sizeof(float));
                }
            } else {
                // Запись в текстовые файлы с использованием QTextStream
                QTextStream outStream1(&outFile1);
                QTextStream outStream2(&outFile2);


                for (float volts : qAsConst(result.data1)) {
                    outStream1 << volts << "\n";
                }

                outFile1.flush();

                for (float volts : qAsConst(result.data2)) {
                    outStream2 << volts << "\n";
                }

                outFile1.flush();
            }

            /// \brief Закрытие выходных файлов.
            outFile1.close();
            outFile2.close();
        }
    }
    else
    {
        /// \brief Вывод информации о директории входного файла.
        qDebug()<<"Директория входного файла"<<inFilePath1;

        /// \brief Проверка существования входного файла.
        if (!QFile::exists(inFilePath1)) {
            qDebug() << "Error: Input file does not exist!";
            return;
        }

        QFile file(inFilePath1);

        totalSize = file.size();

        /// \brief Инициализация синхронизатора для ожидания завершения выполнения задач.
        QFutureSynchronizer<void> synchronizer;

        /// \brief Чтение и разделение входного файла на массив байт.
        QList<QByteArray> BAlist = readAndSplitFile(inFilePath1, 100/STEP);
        qDebug()<<BAlist.size();

        /// \brief Создание и запуск задачи в фоновом режиме для обработки данных.
        QFuture<MyData> future = QtConcurrent::mappedReduced(BAlist,partsProcess,reduce);
        // Дождаться окончания выполнения
        synchronizer.addFuture(future);
        synchronizer.waitForFinished();

        /// \brief Получение результата выполнения задачи.
        const MyData result = future.result();

        /// \brief Создаем объект QFileInfo, используя путь к файлу
        QFileInfo fileInfo(inFilePath1);

        /// \brief Извлекаем имя файла без расширения
        QString fileNameWithoutExtension = fileInfo.baseName();

        /// \brief Генерация путей к выходным файлам с уникальными временными метками.
        QString outFile1Path = outDir + "/" + outFile1Name + fileNameWithoutExtension + "." + format;
        QString outFile2Path = outDir + "/" + outFile2Name + fileNameWithoutExtension + "." + format;

        /// \brief Создание выходных файлов.
        QFile outFile1(outFile1Path);
        QFile outFile2(outFile2Path);

        /// \brief Создание потоков данных для записи в выходные файлы в формате Little Endian.
        QDataStream outStream1(&outFile1);
        QDataStream outStream2(&outFile2);
        outStream1.setByteOrder(QDataStream::LittleEndian);
        outStream2.setByteOrder(QDataStream::LittleEndian);

        /// \brief Проверка успешного открытия выходного файла 1 для записи.
        if (!outFile1.open(QIODevice::WriteOnly)) {
            qDebug() << "Error: Failed to open output file 1 for writing!";
            return;
        }

        /// \brief Проверка успешного открытия выходного файла 2 для записи.
        if (!outFile2.open(QIODevice::WriteOnly)) {
            qDebug() << "Error: Failed to open output file 2 for writing!";
            return;
        }
        /// \brief Запись данных в выходные файлы.
        if (format == "bin") {
            // Запись в бинарные файлы с использованием QDataStream
            QDataStream outStream1(&outFile1);
            QDataStream outStream2(&outFile2);

            outStream1.setByteOrder(QDataStream::LittleEndian);
            outStream2.setByteOrder(QDataStream::LittleEndian);

            for (float volts : qAsConst(result.data1)) {
                outStream1.writeRawData(reinterpret_cast<const char*>(&volts), sizeof(float));
            }

            for (float volts : qAsConst(result.data2)) {
                outStream2.writeRawData(reinterpret_cast<const char*>(&volts), sizeof(float));
            }
        } else {
            // Запись в текстовые файлы с использованием QTextStream
            QTextStream outStream1(&outFile1);
            QTextStream outStream2(&outFile2);

            for (float volts : qAsConst(result.data1)) {
                outStream1 << volts << "\n";
            }

            outFile1.flush();

            for (float volts : qAsConst(result.data2)) {
                outStream2 << volts << "\n";
            }

            outFile1.flush();
        }

        /// \brief Закрытие выходных файлов.
        outFile1.close();
        outFile2.close();
    }

    /// \brief Сигнал о завершении выполнения операции.
    readedData = 0;
    emit finished();
}

/*!
\brief Метод для остановки преобразования на паузу

\warning На данный момент не используется

*/
void Worker::pause()
{
    paused = true;
}

/*!
\brief Метод для прекращения преобразования

\warning На данный момент не используется

*/
void Worker::stop()
{
    stopped = true;
}

/*!
\brief Метод для перевода значения из кода в напряжение

Алгоритм преобразования основан на следующих данных кодов напряжений:
0 - "-1 В" на входе
8192 - "0 В" на входе
16383 - "1 В" на входе

\param[in] data Код АЦП
\result Напряжение АЦП в виде числа с плавающей точкой

*/
float Worker::convertFunc(float data){
    qint16 c = 8192;
    if (data != c ) {
        data = (data-c)*0.000123;
    }
    else data = 0;

    return data;
}

/*!
\brief Метод для приема пути к исходному файлу

Принимает путь к исходному файлу и записывает его в глобальную переменную

\param[in] in Путь к исхоному файлу

*/
void Worker::serupIn(QString in)
{
    inFilePath1 = in.trimmed();
    qDebug() << "Received file path in Worker:" << inFilePath1;
}

/*!
\brief Метод для приема пути к директории для записи итоговых файлов

Принимает путь к директории, в которую будут записаны выходные файлы

\param[in] out Путь к директории выходных файлов

*/
void Worker::serupOut(QString out)
{
    outDir = out;
}

/*!
\brief Метод для приема шага считываниея файла

\param[in] step Шаг считывания файла в значениях или блоках

\warning На данный момент не используется

*/
void Worker::setupStep(double step)
{
    STEP = static_cast<int>(step)+1;
    qDebug()<<"current step"<<STEP;
}

/*!
\brief Метод для приема режима считывания файла

Предусмотрены 2 режима считывания: по значениям и блоками

\param[in] str Режим считывания

\warning На данный момент не используется

*/
void Worker::setupMode(QString str)
{
    MODE = str;
}

/*!
\brief Метод для приема суфикса имени файла

Получет на выход строку, которая приписывается в конец имени выходных файлов

\param[in] str Суффикс имени

\warning На данный момент не используется

*/
void Worker::setupSubName(QString str)
{
    subN = str;
}

void Worker::setup(QMap<QString,QString> settings)
{
    for (auto i = settings.constBegin(); i != settings.constEnd(); ++i) {
        qDebug() << "Key:" << i.key() << "Value:" << i.value();
    }
    STEP = settings["STEP"].toInt();
    inFilePath1 = settings["inputFile"];
    outDir = settings["outDir"];
    outFile1Name = settings["file1Name"];
    outFile2Name = settings["file2Name"];
    outFile1Suffix = settings["file1Suffix"];
    outFile2Suffix = settings["file2Suffix"];
    format = settings["format"];
}

void Worker::setProgress(int data){
//    progress(data);
}
