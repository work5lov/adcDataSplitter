#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QVector>
#include <cmath>
#include <QtMath>
#include <cmath>
#include <algorithm>
#include <complex>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

QVector<double> apply_envelope_filter(const QVector<double>& y);
QVector<double> applyFilter(const QVector<double>& signal);
QVector<double> calculateMovingAverage(const QVector<double>& signal, int windowSize);
QVector<double> logDetector(const QVector<double>& signal);
QVector<double> extractEnvelope(const QVector<double>& signal, int windowSize);
double calculateMedian(const QVector<double>& signal);
double medianAbsoluteDeviation(const QVector<double>& signal);
QVector<double> generateMeander(const QVector<double>& envelope);
QVector<double> calculate_envelope(const QVector<double>& y);
QVector<double> ortogonal_signal(const QVector<double>& y);
double calculatePercentile(const QVector<double>& signal, double percentile);
double calculateLocalMean(const QVector<double>& signal, int index, int windowSize);
double calculateLocalStdDev(const QVector<double>& signal, int index, int windowSize);
QVector<double> generateMeander2(const QVector<double>& envelope);

double calculateMiddle(const QVector<double>& signal);
QVector<double> badEvelope(const QVector<double>& signal);
QVector<double> normalizeSignal(const QVector<double>& signal);
QVector<double> calculateLocalMaxima(const QVector<double>& signal, int windowSize);
QVector<double> calculateEMA(const QVector<double>& signal, int n, double k);
QVector<double> lowPassFilter(const QVector<double>& signal, double alpha);
QVector<double> matchedFilter(const QVector<double>& signal, double lambda, double delta);
QVector<double> cubicSplineInterpolation(const QVector<double>& x, const QVector<double>& y, int numPoints);
QVector<double> calculateInstantaneousAmplitude(const QVector<double>& signal, int windowSize);
QVector<double> findEnvelope(const QVector<double>& signal);
double standardDeviation (const QVector<double>& signal);
float convertF(float data);

#endif // FUNCTIONS_H
