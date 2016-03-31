#include <QScopedPointer>
#include <QCoreApplication>
#include <QTimer>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

class TestEmitter: public QObject
{
    Q_OBJECT

public:
    TestEmitter(QObject *parent = 0): QObject(parent) {}

    void paste(int s0, int s1, const QByteArray &chunk = QByteArray()) {
        // doSmth...
        emit textChanged(s0, s1, chunk);
    }

signals:
    void textChanged(int s0, int s1, const QByteArray &chunk);
};

class TestReceiver: public QObject
{
    Q_OBJECT

public:
    TestReceiver(QObject *parent = 0): QObject(parent), dummy_(0) {}

    void onTextChanged(int s0, int s1, const QByteArray &chunk) {
        dummy_ += (s0 * s1);
    }

private:
    int dummy_;
};

class BulkEmissionTest: public QObject
{
public:
    BulkEmissionTest(QObject *parent = 0):
        QObject(parent),
        e_(new TestEmitter(this)),
        r_(new TestReceiver(this))

    {
        connect(e_, &TestEmitter::textChanged, r_, &TestReceiver::onTextChanged);
        QTimer *t = new QTimer(this);
        connect(t, &QTimer::timeout, this, &BulkEmissionTest::run);
        t->setSingleShot(true);
        t->start(0);
    }

    void run() {
        cout << "run()..." << endl;
        QByteArray chunk = "Abc";
        high_resolution_clock::time_point t0 = high_resolution_clock::now();
        const int n = 10000000;
        for (int i = 0; i < n; ++i)
            e_->paste(i, i + 1, chunk);
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        double dt = duration_cast<duration<double>>(t1 - t0).count();
        cout << "took " << dt << " s" << endl;
        cout << "=> " << n / dt << " Hz" << endl;
        QCoreApplication::instance()->quit();
    }

private:
    TestEmitter *e_;
    TestReceiver *r_;
};

int main(int argc, char **argv)
{
    QCoreApplication  app(argc, argv);

    new BulkEmissionTest(&app);

    return app.exec();
}

#include "moc_testQtNotifyPerformance.cc"
