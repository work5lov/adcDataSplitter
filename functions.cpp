#include "functions.h"

QVector<double> apply_envelope_filter(const QVector<double>& y){
    QVector<double> envelope(y.size());

    for (int i = 0; i < y.size(); ++i) {
        double analytic_signal = 0.0;
        for (int j = 0; j < y.size(); ++j) {
            double angle = 2.0 * M_PI * i * j / y.size();
            analytic_signal += y[j] * std::cos(angle);
        }
        envelope[i] = std::abs(analytic_signal);
    }

    return envelope;
}

const double alpha = 0.1;
const double beta = 1.0 - alpha;

QVector<double> applyFilter(const QVector<double>& signal){
    QVector<double> filteredSignal(signal.size());

    // Apply the filter
    filteredSignal[0] = signal[0];
    for (int i = 1; i < signal.size(); i++) {
        filteredSignal[i] = alpha * signal[i]+ beta * filteredSignal[i-1];
    }

    return filteredSignal;
}

QVector<double> calculateMovingAverage(const QVector<double>& signal, int windowSize) {
    QVector<double> smoothedSignal(signal.size());

    for (int i = 0; i < signal.size(); i++) {
        double sum = 0.0;
        int count = 0;

        for (int j = i - windowSize + 1; j <= i; j++) {
            if (j >= 0) {
                sum += signal[j];
                count++;
            }
        }

        smoothedSignal[i] = sum / count;
    }

    return smoothedSignal;
}

QVector<double> calculateEMA(const QVector<double>& signal, int n, double k)
{
    QVector<double> ema(signal.size());
    double multiplier = k / (n + 1);
    ema[0] = signal[0];

    for (int i = 1; i < signal.size(); i++) {
        ema[i] = (signal[i] - ema[i - 1]) * multiplier + ema[i - 1];
    }

    return ema;
}

QVector<double> lowPassFilter(const QVector<double>& signal, double alpha)
{
    QVector<double> filtered(signal.size());
    filtered[0] = signal[0];

    for (int i = 1; i < signal.size(); i++) {
        // Избегайте повторного вычисления (1 - alpha)
        double oneMinusAlpha = 1.0 - alpha;

        // Избегайте повторного доступа к элементам signal[i]
        double currentSignal = signal[i];

        filtered[i] = alpha * currentSignal + oneMinusAlpha * filtered[i - 1];
    }

    return filtered;
}

QVector<double> matchedFilter(const QVector<double>& signal, double lambda, double delta)
{
    QVector<double> filtered(signal.size());
    QVector<double> w(signal.size(), 0.0);
    double P = 1.0 / delta;

    for (int i = 0; i < signal.size(); i++) {
        double y = signal[i];
        double x = i > 0 ? filtered[i - 1] : 0.0;
        double e = y - w[i] * x;
        double k = P * x / (lambda + x * P * x);
        w[i] += k * e;
        P = 1.0 / lambda * (P - k * x * P);
        filtered[i] = w[i] * x;
    }

    return filtered;
}

QVector<double> logDetector(const QVector<double>& signal) {
    QVector<double> output(signal.size());

    double maxVal = *std::max_element(signal.constBegin(), signal.constEnd());
    double logMaxVal = qFabs(log10(qFabs(maxVal)));
    qDebug()<<logMaxVal;

    for (int i = 0; i < signal.size(); i++) {
        output[i] = qFabs(log10(qFabs(signal[i]+2.0)));
        qDebug()<<"signal"<<signal[i]<<"output"<<output[i];
    }

    return output;
}

QVector<double> calculateInstantaneousAmplitude(const QVector<double>& signal, int windowSize) {
    // Step 1: Obtain the signal
    QVector<double> envelope(signal.size());

    // Step 2: Apply envelope detection
    for (int i = 0; i < signal.size(); i++) {
        double sum = 0;
        for (int j = i - 10; j <= i + 10; j++) {
            if (j >= 0 && j < signal.size()) {
                sum += signal[j] * signal[j];
            }
        }
        envelope[i] = sqrt(sum / windowSize);
    }

    // Step 3: Rectify the signal
    for (int i = 0; i < envelope.size(); i++) {
        envelope[i] = qAbs(envelope[i]);
    }

    // Step 4: Smooth the signal
    QVector<double> smoothed(envelope.size());
    for (int i = 0; i < smoothed.size(); i++) {
        double sum = 0;
        for (int j = i - 10; j <= i + 10; j++) {
            if (j >= 0 && j < envelope.size()) {
                sum += envelope[j];
            }
        }
        smoothed[i] = sum / windowSize;
    }

    // Step 5: Calculate the instantaneous amplitude
    QVector<double> amplitude(smoothed.size());
    for (int i = 0; i < amplitude.size(); i++) {
        amplitude[i] = smoothed[i];
    }

    return amplitude;
}

QVector<double> extractEnvelope(const QVector<double>& signal, int windowSize) {
    QVector<double> envelope(signal.size());

    // Step 2: Apply envelope detection
    for (int i = 0; i < signal.size(); i++) {
        double sum = 0;
        for (int j = i - 10; j <= i + 10; j++) {
            if (j >= 0 && j < signal.size()) {
                sum += signal[j] * signal[j];
            }
        }
        envelope[i] = sqrt(sum / windowSize);
    }
    return envelope;
}

double calculateMiddle(const QVector<double>& signal) {
    double minValue = *std::min_element(signal.constBegin(), signal.constEnd());
    double maxValue = *std::max_element(signal.constBegin(), signal.constEnd());
    qDebug()<<"min"<<minValue<<"max"<<maxValue;
    return minValue+((maxValue-minValue)/2);
}

QVector<double> generateMeander(const QVector<double>& envelope) {
    QVector<double> meander(envelope.size());

    // Calculate median absolute deviation of the envelope
    double mid = calculateMiddle(envelope);
    double maxValue = *std::max_element(envelope.constBegin(), envelope.constEnd());

    // Generate meander waveform using adaptive threshold
    bool state = false;
    for (int i = 0; i < envelope.size(); i++) {
        //double threshold = mad*2.0; // Adjust the factor as needed
        if (envelope[i] > mid) {
            state = !state;
        }
        meander[i] = (state) ? maxValue : mid;
    }

    return meander;
}

QVector<double> badEvelope(const QVector<double>& signal){
    QVector<double> evelope(signal.size());
    QVector<double> evelope1(signal.size());
    double mid = calculateMiddle(signal);
    double maxVal = *std::max_element(signal.constBegin(), signal.constEnd());
    double minVal = *std::min_element(signal.constBegin(), signal.constEnd());
    double logMaxVal = qFabs(log10(qFabs(maxVal)));
    double diff;
    double factor = (maxVal - minVal) / log10(maxVal / minVal);
    diff=qFabs(log10(qFabs(maxVal)))-maxVal;
//    for(int i=0;i<signal.size();i++){
//        if(signal[i]<0) evelope1[i] = qFabs(signal[i]);
//        else if (signal[i]<mid&&signal[i]>0) evelope1[i] = (mid+(mid-signal[i]));
//        else evelope1[i] = signal[i];
//        qDebug()<<i<<'.'<<"signal:"<<signal[i]<<"middle"<<mid<<"; evelope"<<evelope1[i]<<"logDet"<<(qFabs(log10(qFabs(signal[i]*1000))/logMaxVal))/100<<"разница от середины"<<evelope1[i]-((qFabs(log10(qFabs(evelope1[i]*1000))/logMaxVal))/100);
//    }
    for(int i=0;i<signal.size();i++){

        evelope[i]=qFabs(log10(qFabs(signal[i])));
        qDebug()<<i<<'.'<<"signal:"<<signal[i]<<"middle"<<mid<<"evelope1"<<evelope1[i]<<"evelope"<<evelope[i]<<"logDet"<<(qFabs(log10(qFabs(signal[i]*1000))/logMaxVal))/100<<"разница от середины"<<evelope[i]-(qFabs(log10(qFabs(evelope1[i]))));
    }


    return evelope;
}

QVector<double> normalizeSignal(const QVector<double>& signal)
{
    QVector<double> normalized(signal.size());

    double max = signal[0];
    double min = signal[0];

    // Find the maximum and minimum values of the signal
    for (int i = 1; i < signal.size(); i++) {
        if (signal[i] > max) {
            max = signal[i];
        }
        if (signal[i] < min) {
            min = signal[i];
        }
    }

    // Calculate the factor to multiply each signal value by
    double factor = (max - min) / log10(max / min);

    // Multiply each signal value by the factor
    for (int i = 0; i < signal.size(); i++) {
        normalized[i] = signal[i] * factor;
    }

    // Take the logarithm of the signal values
    for (int i = 0; i < signal.size(); i++) {
        normalized[i] = log10(normalized[i]);
    }

    double maxLog = normalized[0];
    double minLog = normalized[0];

    // Find the maximum and minimum values of the logarithmic signal
    for (int i = 1; i < signal.size(); i++) {
        if (normalized[i] > maxLog) {
            maxLog = normalized[i];
        }
        if (normalized[i] < minLog) {
            minLog = normalized[i];
        }
    }

    // Stretch the logarithmic values to the original range
    for (int i = 0; i < signal.size(); i++) {
        normalized[i] = (normalized[i] - minLog) / (maxLog - minLog) * (max - min) + min;
    }

    return normalized;
}

QVector<double> calculate_envelope(const QVector<double>& y){
    QVector<double> envelope(y.size());
    QVector<double> evelope1(y.size());
//    double mid = calculateMiddle(y);
    double maxVal = *std::max_element(y.constBegin(), y.constEnd());
    for (int i = 0; i < y.size(); ++i) {
        if(y[i]==0) envelope[i] = 0;
        else envelope[i]= maxVal;
    }
    return envelope;
}

QVector<double> calculateLocalMaxima(const QVector<double>& signal, int windowSize)
{
    QVector<double> localMaxima(signal.size());

    for (int i = 0; i < signal.size(); i++) {
        double currentMax = signal[i];

        // Избегайте повторного вычисления i - windowSize и i + windowSize
        int startIdx = std::max(0, i - windowSize);
        int endIdx = std::min(static_cast<int>(signal.size()) - 1, i + windowSize);

        // Избегайте повторного доступа к элементам signal[j]
        for (int j = startIdx; j <= endIdx; j++) {
            currentMax = std::max(currentMax, signal[j]);
        }

        localMaxima[i] = currentMax;
    }

    return localMaxima;
}


QVector<double> ortogonal_signal(const QVector<double>& y){
    QVector<double> envelope(y.size());
    QVector<double> A(y.size());
    envelope[0]=y[0];
    for(int i = 1;i<y.size();i++){
        envelope[i]=-(envelope[i-1]*y[i-1]/y[i]);
    }
    for(int i = 0;i<y.size();i++){
        A[i]=qSqrt(qPow(y[i],2)+qPow(envelope[i],2));
    }
    return A;
}

double calculatePercentile(const QVector<double>& signal, double percentile) {
    QVector<double> sortedSignal = signal;
    std::sort(sortedSignal.begin(), sortedSignal.end());

    int n = sortedSignal.size();
    int index = static_cast<int>(std::round(n * percentile / 100.0)) - 1;
    return sortedSignal[std::max(0, std::min(n - 1, index))];
}

double calculateLocalMean(const QVector<double>& signal, int index, int windowSize) {
    double sum = 0.0;
    int count = 0;

    for (int i = index - windowSize + 1; i <= index + windowSize - 1; i++) {
        if (i >= 0 && i < signal.size()) {
            sum += signal[i];
            count++;
        }
    }

    return sum / count;
}

double calculateLocalStdDev(const QVector<double>& signal, int index, int windowSize) {
    double mean = calculateLocalMean(signal, index, windowSize);
    double sum = 0.0;
    int count = 0;

    for (int i = index - windowSize + 1; i <= index + windowSize - 1; i++) {
        if (i >= 0 && i < signal.size()) {
            sum += pow(signal[i] - mean, 2.0);
            count++;
        }
    }

    return sqrt(sum / count);
}

QVector<double> generateMeander2(const QVector<double>& envelope) {
    QVector<double> meander(envelope.size());

    // Calculate percentile threshold for lower values
    double percentile = 10.0; // Adjust the percentile as needed
    double threshold = calculatePercentile(envelope, percentile);

    // Generate meander waveform using hybrid threshold
    bool state = false;
    for (int i = 0; i < envelope.size(); i++) {
        if (envelope[i] < threshold) {
            meander[i] = 0.0;
        } else {
            double localMean = calculateLocalMean(envelope, i, 10); // Adjust the window size as needed
            double localStdDev = calculateLocalStdDev(envelope, i, 10); // Adjust the window size as needed
            double adaptiveThreshold = localMean + 2.0 * localStdDev; // Adjust the factor as needed
            if (envelope[i] > adaptiveThreshold) {
                state = !state;
            }
            meander[i] = (state) ? threshold : 0.0;
        }
    }

    return meander;
}

QVector<double> findEnvelope(const QVector<double> &signal){
    QVector<double> envelope(signal.size());
    QVector<double> signal1(signal.size());
    signal1 = lowPassFilter(signal,1);
    double mean = 0;
    double sum = 0;
    double standardDeviatio = 0;
    int count = 0;
    double currentMax = 0;
    double currentMaxValue = 0;
    int currentMaxStartIndex = 0;
    int currentMaxEndIndex = 0;
    bool inMax = false;

    // Вычисление среднего значения и СКО
    for (double value : signal1) {
        sum += value;
    }
    mean = sum / signal1.size();
    sum = 0;
    for (double value : signal1) {
        double difference = value - mean;
        sum += difference * difference;
    }
    standardDeviatio = standardDeviation(calculateLocalMaxima(signal1,50));

    // Поиск максимумов, которые больше СКО
    for (int i = 0; i < signal1.size(); i++) {
        double value = signal1[i];
        if (value > mean + standardDeviatio && value > signal1[i - 1] && value > signal1[i + 1]) {
            if (!inMax) {
                currentMaxStartIndex = i;
                inMax = true;
            }
            currentMax = value;
            currentMaxEndIndex = i;
        } else if (inMax) {
            if (currentMax >= mean + standardDeviatio) {
                count++;
                qDebug() << mean << "СКО=" << standardDeviatio << "Максимум " << count << ": значение = " << currentMax << ", начало = " << currentMaxStartIndex << ", конец = " << currentMaxEndIndex;
                envelope[i]=currentMax;
            }
            inMax = false;
        }
    }

    QVector<QVector<double>> clusters; // Кластеры
    QVector<double> maxS;
    QVector<QPair<int, int>> boundaries; // Границы кластеров

    // Объединение ближайших значений в кластеры
    for (int i = 0; i < envelope.size(); i++) {
        double value = qAbs(envelope[i]);
        bool addedToCluster = false;
        for (int j = 0; j < clusters.size(); j++) {
            double distance = std::abs(value - clusters[j][0]);
            if (distance <= standardDeviatio) {
                clusters[j].push_back(value);
                addedToCluster = true;
                break;
            }
        }
        if (!addedToCluster) {
            clusters.push_back({value});
        }
    }
    // Замена значений каждого кластера на максимальное значение кластера
    for (int i = 0; i < clusters.size(); i++) {
        double max = clusters[i][0];
        int startIndex = -1;
        int endIndex = -1;
        for (int j = 0; j < clusters[i].size(); j++) {
            if (clusters[i][j] > max) {
                max = clusters[i][j];
            }
            if (startIndex == -1) {
                startIndex = signal1.indexOf(clusters[i][j]);
            }
            endIndex = signal1.indexOf(clusters[i][j]);
        }
        maxS.push_back(max);
        boundaries.push_back(qMakePair(startIndex, endIndex));
    }
    if(!maxS.empty() && maxS[0] == 0){
        maxS.erase(maxS.begin());
        boundaries.removeFirst();
    }
    int startV;
    int lastV;
    // Вывод кластеров и их границ
    for (int i = 0; i < maxS.size(); i++) {
        qDebug() << "Максимум кластера " << i + 1 << ": " << maxS[i] << ", начало = " << boundaries[i].first << ", конец = " << boundaries[i].second;
        startV = boundaries[0].first;
        lastV = startV;
        if(boundaries[i].second>startV) lastV = boundaries[i].second;
    }
    qDebug() << "Количество максимумов, которые больше СКО: " << count;
    double pulse_daration_in_samples = lastV-startV;
    double maxVal = *std::max_element(maxS.constBegin(), maxS.constEnd());
    QVector<double>output(signal.size());
    for(int i = 0; i < signal.size(); i++){
        for(int j = 0; j < boundaries.size(); j++){
            if (startV-1000 <= i && i < startV + 0.1 * pulse_daration_in_samples){
                double x1 = startV - 1000;
                double y1 = standardDeviatio;
                double x2 = startV+0.1*pulse_daration_in_samples;
                double y2 = maxVal;
                double slope = (y2 - y1) / (x2 - x1);
                double intercept = y1 - slope * x1;
                output[i] = slope*i+intercept;
            }
            else if (startV+0.1*pulse_daration_in_samples <= i && i < startV + 0.9 * pulse_daration_in_samples){
                output[i] = maxVal;
            }
            else if (startV + 0.9 * pulse_daration_in_samples <= i && i < lastV + 1000){
                double x1 = startV + 0.9 * pulse_daration_in_samples;
                double y1 = maxVal;
                double x2 = lastV + 1000;
                double y2 = standardDeviatio;
                double slope = (y2 - y1) / (x2 - x1);
                double intercept = y1 - slope * x1;
                output[i] = slope*i+intercept;
            }
            else if (output[i]!=0) continue;
            else output[i] = standardDeviatio;
        }
    }

    return output;
}

double standardDeviation(const QVector<double> &signal){
    double sum = 0;
    for (double value : signal) {
        sum += value;
    }
    double mean = sum / signal.size();

    // Вычисление суммы квадратов разностей
    double sumOfSquares = 0;
    for (double value : signal) {
        double difference = value - mean;
        sumOfSquares += difference * difference;
    }

    // Вычисление среднеквадратического отклонения
    double standardDeviation = std::sqrt(sumOfSquares / signal.size());

    return standardDeviation;
}

float convertF(float data){
    int c = 8192;
    if (data != c ) {
        data = (data-c)*0.000123;
    }
    else data = 0;
    return data;
}
