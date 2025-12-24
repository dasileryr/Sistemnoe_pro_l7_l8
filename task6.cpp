#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QSpinBox>
#include <QThread>
#include <QMessageBox>

class NumberThread : public QThread {
    Q_OBJECT
public:
    NumberThread(int start, int end, int threadId = 1, QObject* parent = nullptr)
        : QThread(parent), m_start(start), m_end(end), m_threadId(threadId) {}
    
signals:
    void numberGenerated(int value, int threadId);
    void threadFinished(int threadId);
    
protected:
    void run() override {
        for (int i = m_start; i <= m_end; i++) {
            emit numberGenerated(i, m_threadId);
            msleep(50);
        }
        emit threadFinished(m_threadId);
    }
    
private:
    int m_start;
    int m_end;
    int m_threadId;
};

class MainWindow : public QWidget {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Задание 6: Потоки с числами");
        setFixedSize(600, 500);
        
        // Создаем элементы интерфейса
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        
        // Параметры
        QHBoxLayout *paramsLayout = new QHBoxLayout();
        paramsLayout->addWidget(new QLabel("Начало диапазона:"));
        startSpinBox = new QSpinBox();
        startSpinBox->setRange(-1000, 1000);
        startSpinBox->setValue(0);
        paramsLayout->addWidget(startSpinBox);
        
        paramsLayout->addWidget(new QLabel("Конец диапазона:"));
        endSpinBox = new QSpinBox();
        endSpinBox->setRange(-1000, 1000);
        endSpinBox->setValue(50);
        paramsLayout->addWidget(endSpinBox);
        
        paramsLayout->addWidget(new QLabel("Количество потоков:"));
        threadsSpinBox = new QSpinBox();
        threadsSpinBox->setRange(1, 10);
        threadsSpinBox->setValue(1);
        paramsLayout->addWidget(threadsSpinBox);
        
        mainLayout->addLayout(paramsLayout);
        
        // Кнопки
        QHBoxLayout *buttonsLayout = new QHBoxLayout();
        startButton = new QPushButton("Запустить");
        stopButton = new QPushButton("Остановить");
        stopButton->setEnabled(false);
        clearButton = new QPushButton("Очистить");
        
        buttonsLayout->addWidget(startButton);
        buttonsLayout->addWidget(stopButton);
        buttonsLayout->addWidget(clearButton);
        mainLayout->addLayout(buttonsLayout);
        
        // Область вывода
        outputText = new QTextEdit();
        outputText->setReadOnly(true);
        mainLayout->addWidget(outputText);
        
        // Подключаем сигналы
        connect(startButton, &QPushButton::clicked, this, &MainWindow::startThreads);
        connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopThreads);
        connect(clearButton, &QPushButton::clicked, outputText, &QTextEdit::clear);
    }
    
private slots:
    void startThreads() {
        int start = startSpinBox->value();
        int end = endSpinBox->value();
        int numThreads = threadsSpinBox->value();
        
        if (start > end) {
            QMessageBox::warning(this, "Ошибка", "Начало диапазона не может быть больше конца!");
            return;
        }
        
        // Очищаем предыдущие потоки
        stopThreads();
        
        outputText->append(QString("Запуск %1 потоков для диапазона [%2, %3]...\n")
                          .arg(numThreads).arg(start).arg(end));
        
        int range = end - start + 1;
        int numbersPerThread = range / numThreads;
        int remainder = range % numThreads;
        
        int currentStart = start;
        for (int i = 0; i < numThreads; i++) {
            int currentEnd = currentStart + numbersPerThread - 1;
            if (i < remainder) {
                currentEnd++;
            }
            
            NumberThread *thread = new NumberThread(currentStart, currentEnd, i + 1, this);
            threads.append(thread);
            
            connect(thread, &NumberThread::numberGenerated, this, &MainWindow::onNumberGenerated);
            connect(thread, &NumberThread::threadFinished, this, &MainWindow::onThreadFinished);
            connect(thread, &NumberThread::finished, thread, &NumberThread::deleteLater);
            
            thread->start();
            
            currentStart = currentEnd + 1;
        }
        
        startButton->setEnabled(false);
        stopButton->setEnabled(true);
    }
    
    void stopThreads() {
        for (NumberThread *thread : threads) {
            if (thread && thread->isRunning()) {
                thread->terminate();
                thread->wait();
            }
        }
        threads.clear();
        startButton->setEnabled(true);
        stopButton->setEnabled(false);
        outputText->append("Все потоки остановлены.\n");
    }
    
    void onNumberGenerated(int value, int threadId) {
        outputText->append(QString("[Поток %1] %2").arg(threadId).arg(value));
    }
    
    void onThreadFinished(int threadId) {
        outputText->append(QString("Поток %1 завершил работу.\n").arg(threadId));
        
        // Проверяем, все ли потоки завершены
        bool allFinished = true;
        for (NumberThread *thread : threads) {
            if (thread && thread->isRunning()) {
                allFinished = false;
                break;
            }
        }
        
        if (allFinished) {
            startButton->setEnabled(true);
            stopButton->setEnabled(false);
            outputText->append("Все потоки завершены.\n");
        }
    }
    
private:
    QSpinBox *startSpinBox;
    QSpinBox *endSpinBox;
    QSpinBox *threadsSpinBox;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *clearButton;
    QTextEdit *outputText;
    QList<NumberThread*> threads;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

#include "task6.moc"

