#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <random>
#include <vector>
#include <algorithm>
#include <climits>
#include <mutex>

const int ARRAY_SIZE = 10000;

class ComputationThread : public QThread {
    Q_OBJECT
public:
    enum ComputationType {
        FindMax,
        FindMin,
        FindSum,
        WriteToFile
    };
    
    ComputationThread(ComputationType type, const std::vector<int>& data, 
                     int start, int end, const QString& filename = "", QObject* parent = nullptr)
        : QThread(parent), m_type(type), m_data(data), m_start(start), m_end(end), m_filename(filename),
          m_maxValue(0), m_minValue(0), m_average(0.0) {}
    
    void setResults(int maxVal, int minVal, double avg) {
        m_maxValue = maxVal;
        m_minValue = minVal;
        m_average = avg;
    }
    
signals:
    void progressUpdated(int value);
    void resultReady(const QString& result);
    void computationFinished();
    
protected:
    void run() override {
        switch (m_type) {
            case FindMax: {
                int localMax = INT_MIN;
                for (int i = m_start; i < m_end; i++) {
                    if (m_data[i] > localMax) {
                        localMax = m_data[i];
                    }
                    if ((i - m_start) % 100 == 0) {
                        emit progressUpdated((i - m_start) * 100 / (m_end - m_start));
                    }
                }
                emit resultReady(QString("Максимум: %1").arg(localMax));
                break;
            }
            case FindMin: {
                int localMin = INT_MAX;
                for (int i = m_start; i < m_end; i++) {
                    if (m_data[i] < localMin) {
                        localMin = m_data[i];
                    }
                    if ((i - m_start) % 100 == 0) {
                        emit progressUpdated((i - m_start) * 100 / (m_end - m_start));
                    }
                }
                emit resultReady(QString("Минимум: %1").arg(localMin));
                break;
            }
            case FindSum: {
                long long localSum = 0;
                for (int i = m_start; i < m_end; i++) {
                    localSum += m_data[i];
                    if ((i - m_start) % 100 == 0) {
                        emit progressUpdated((i - m_start) * 100 / (m_end - m_start));
                    }
                }
                emit resultReady(QString("Сумма: %1").arg(localSum));
                break;
            }
            case WriteToFile: {
                QFile file(m_filename);
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream out(&file);
                    out << "=== РЕЗУЛЬТАТЫ ВЫЧИСЛЕНИЙ ===\n";
                    out << "Размер массива: " << ARRAY_SIZE << "\n\n";
                    out << "=== МАССИВ ЧИСЕЛ ===\n";
                    
                    for (int i = 0; i < m_data.size(); i++) {
                        out << QString("%1 ").arg(m_data[i], 8);
                        if ((i + 1) % 10 == 0) {
                            out << "\n";
                        }
                        if (i % 100 == 0) {
                            emit progressUpdated(i * 100 / m_data.size());
                        }
                    }
                    if (m_data.size() % 10 != 0) {
                        out << "\n";
                    }
                    
                    out << "\n=== РЕЗУЛЬТАТЫ ===\n";
                    out << "Максимум: " << m_maxValue << "\n";
                    out << "Минимум: " << m_minValue << "\n";
                    out << "Среднее: " << QString::number(m_average, 'f', 2) << "\n";
                    
                    file.close();
                    emit resultReady("Данные записаны в файл: " + m_filename);
                } else {
                    emit resultReady("Ошибка записи в файл!");
                }
                break;
            }
        }
        emit progressUpdated(100);
        emit computationFinished();
    }
    
private:
    ComputationType m_type;
    const std::vector<int>& m_data;
    int m_start;
    int m_end;
    QString m_filename;
    int m_maxValue;
    int m_minValue;
    double m_average;
};

class MainWindow : public QWidget {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Задание 7: Вычисления с потоками");
        setFixedSize(700, 600);
        
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        
        // Кнопки управления
        QHBoxLayout *buttonsLayout = new QHBoxLayout();
        generateButton = new QPushButton("Сгенерировать массив");
        computeButton = new QPushButton("Вычислить");
        computeButton->setEnabled(false);
        saveButton = new QPushButton("Сохранить в файл");
        saveButton->setEnabled(false);
        clearButton = new QPushButton("Очистить");
        
        buttonsLayout->addWidget(generateButton);
        buttonsLayout->addWidget(computeButton);
        buttonsLayout->addWidget(saveButton);
        buttonsLayout->addWidget(clearButton);
        mainLayout->addLayout(buttonsLayout);
        
        // Прогресс-бар
        progressBar = new QProgressBar();
        progressBar->setVisible(false);
        mainLayout->addWidget(progressBar);
        
        // Область вывода
        outputText = new QTextEdit();
        outputText->setReadOnly(true);
        mainLayout->addWidget(outputText);
        
        // Результаты
        resultsLabel = new QLabel("Результаты будут отображены здесь");
        resultsLabel->setWordWrap(true);
        mainLayout->addWidget(resultsLabel);
        
        // Подключаем сигналы
        connect(generateButton, &QPushButton::clicked, this, &MainWindow::generateArray);
        connect(computeButton, &QPushButton::clicked, this, &MainWindow::computeResults);
        connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveToFile);
        connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearOutput);
    }
    
private slots:
    void generateArray() {
        outputText->append("Генерация массива из 10000 элементов...");
        progressBar->setVisible(true);
        progressBar->setValue(0);
        
        numbers.clear();
        numbers.resize(ARRAY_SIZE);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 100000);
        
        for (int i = 0; i < ARRAY_SIZE; i++) {
            numbers[i] = dis(gen);
            if (i % 1000 == 0) {
                progressBar->setValue(i * 100 / ARRAY_SIZE);
                QApplication::processEvents();
            }
        }
        
        progressBar->setValue(100);
        outputText->append("Массив сгенерирован успешно!\n");
        computeButton->setEnabled(true);
        progressBar->setVisible(false);
    }
    
    void computeResults() {
        if (numbers.empty()) {
            QMessageBox::warning(this, "Ошибка", "Сначала сгенерируйте массив!");
            return;
        }
        
        outputText->append("Запуск вычислений...");
        progressBar->setVisible(true);
        progressBar->setValue(0);
        
        // Очищаем предыдущие результаты
        maxValue = INT_MIN;
        minValue = INT_MAX;
        sum = 0;
        finishedThreads = 0;
        
        int chunkSize = ARRAY_SIZE / 3;
        
        // Создаем потоки для вычислений
        ComputationThread *maxThread1 = new ComputationThread(
            ComputationThread::FindMax, std::ref(numbers), 0, chunkSize, "", this);
        ComputationThread *maxThread2 = new ComputationThread(
            ComputationThread::FindMax, std::ref(numbers), chunkSize, 2 * chunkSize, "", this);
        ComputationThread *maxThread3 = new ComputationThread(
            ComputationThread::FindMax, std::ref(numbers), 2 * chunkSize, ARRAY_SIZE, "", this);
        
        ComputationThread *minThread1 = new ComputationThread(
            ComputationThread::FindMin, std::ref(numbers), 0, chunkSize, "", this);
        ComputationThread *minThread2 = new ComputationThread(
            ComputationThread::FindMin, std::ref(numbers), chunkSize, 2 * chunkSize, "", this);
        ComputationThread *minThread3 = new ComputationThread(
            ComputationThread::FindMin, std::ref(numbers), 2 * chunkSize, ARRAY_SIZE, "", this);
        
        ComputationThread *sumThread1 = new ComputationThread(
            ComputationThread::FindSum, std::ref(numbers), 0, chunkSize, "", this);
        ComputationThread *sumThread2 = new ComputationThread(
            ComputationThread::FindSum, std::ref(numbers), chunkSize, 2 * chunkSize, "", this);
        ComputationThread *sumThread3 = new ComputationThread(
            ComputationThread::FindSum, std::ref(numbers), 2 * chunkSize, ARRAY_SIZE, "", this);
        
        // Подключаем сигналы для всех потоков
        QList<ComputationThread*> allThreads = {maxThread1, maxThread2, maxThread3,
                                                minThread1, minThread2, minThread3,
                                                sumThread1, sumThread2, sumThread3};
        
        for (ComputationThread *thread : allThreads) {
            connect(thread, &ComputationThread::resultReady, this, &MainWindow::onResultReady);
            connect(thread, &ComputationThread::computationFinished, this, &MainWindow::onComputationFinished);
            connect(thread, &ComputationThread::finished, thread, &ComputationThread::deleteLater);
        }
        
        // Запускаем потоки
        for (ComputationThread *thread : allThreads) {
            thread->start();
        }
        
        computeButton->setEnabled(false);
    }
    
    void onResultReady(const QString& result) {
        outputText->append(result);
        
        // Парсим результаты
        if (result.startsWith("Максимум:")) {
            int value = result.split(":").last().trimmed().toInt();
            if (value > maxValue) maxValue = value;
        } else if (result.startsWith("Минимум:")) {
            int value = result.split(":").last().trimmed().toInt();
            if (value < minValue) minValue = value;
        } else if (result.startsWith("Сумма:")) {
            sum += result.split(":").last().trimmed().toLongLong();
        }
    }
    
    void onComputationFinished() {
        finishedThreads++;
        progressBar->setValue(finishedThreads * 100 / 9);
        
        if (finishedThreads == 9) {
            double average = static_cast<double>(sum) / ARRAY_SIZE;
            
            QString results = QString("=== РЕЗУЛЬТАТЫ ===\n"
                                     "Максимум: %1\n"
                                     "Минимум: %2\n"
                                     "Среднее: %3\n"
                                     "Сумма: %4")
                             .arg(maxValue)
                             .arg(minValue)
                             .arg(average, 0, 'f', 2)
                             .arg(sum);
            
            resultsLabel->setText(results);
            outputText->append("\n" + results);
            progressBar->setVisible(false);
            computeButton->setEnabled(true);
            saveButton->setEnabled(true);
            finishedThreads = 0;
        }
    }
    
    void saveToFile() {
        if (numbers.empty()) {
            QMessageBox::warning(this, "Ошибка", "Нет данных для сохранения!");
            return;
        }
        
        if (maxValue == INT_MIN || minValue == INT_MAX) {
            QMessageBox::warning(this, "Ошибка", "Сначала выполните вычисления!");
            return;
        }
        
        QString filename = QFileDialog::getSaveFileName(this, "Сохранить результаты", 
                                                        "results.txt", "Text Files (*.txt)");
        if (filename.isEmpty()) {
            return;
        }
        
        outputText->append("Запись в файл...");
        progressBar->setVisible(true);
        progressBar->setValue(0);
        
        double average = static_cast<double>(sum) / ARRAY_SIZE;
        
        ComputationThread *fileThread = new ComputationThread(
            ComputationThread::WriteToFile, numbers, 0, numbers.size(), filename, this);
        fileThread->setResults(maxValue, minValue, average);
        
        connect(fileThread, &ComputationThread::resultReady, outputText, &QTextEdit::append);
        connect(fileThread, &ComputationThread::computationFinished, [this, filename]() {
            progressBar->setVisible(false);
            QMessageBox::information(this, "Успех", "Данные сохранены в файл:\n" + filename);
        });
        connect(fileThread, &ComputationThread::finished, fileThread, &ComputationThread::deleteLater);
        
        fileThread->start();
    }
    
    void clearOutput() {
        outputText->clear();
        resultsLabel->setText("Результаты будут отображены здесь");
        progressBar->setVisible(false);
    }
    
private:
    QPushButton *generateButton;
    QPushButton *computeButton;
    QPushButton *saveButton;
    QPushButton *clearButton;
    QTextEdit *outputText;
    QLabel *resultsLabel;
    QProgressBar *progressBar;
    
    std::vector<int> numbers;
    int maxValue = INT_MIN;
    int minValue = INT_MAX;
    long long sum = 0;
    int finishedThreads = 0;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

#include "task7.moc"

