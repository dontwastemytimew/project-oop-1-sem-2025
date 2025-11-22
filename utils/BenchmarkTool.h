#ifndef BENCHMARKTOOL_H
#define BENCHMARKTOOL_H

#include <QElapsedTimer>
#include <QDebug>

class BenchmarkTool {
public:
    /**
     * @brief Запускає вимірювання часу виконання даної функції N разів.
     * @param operationName Назва операції (для логування).
     * @param iterations Кількість повторів.
     * @param func Лямбда-функція, яку потрібно виміряти.
     */
    template <typename Func>
    static void run(const QString& operationName, int iterations, Func func) {
        QElapsedTimer timer;
        timer.start();

        for (int i = 0; i < iterations; ++i) {
            func();
        }

        qint64 elapsedMs = timer.elapsed();
        
        qDebug() << "--- BENCHMARK RESULT ---";
        qDebug() << "Operation:" << operationName;
        qDebug() << "Iterations:" << iterations;
        qDebug() << "Total time:" << elapsedMs << "ms";
        qDebug() << "Avg time per run:" << (double)elapsedMs / iterations << "ms";
        qDebug() << "------------------------";
    }
};

#endif // BENCHMARKTOOL_H