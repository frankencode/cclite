#include <cc/Event>
#include <iostream>
#include <chrono>

using namespace cc;
using namespace std;
using namespace std::chrono;

struct TextDelta {
    TextDelta(int s0, int s1, const string &chunk):
        s0_(s0),
        s1_(s1),
        chunk_(chunk)
    {}
    int s0_, s1_;
    string chunk_;
};

class TestEmitter: public Recipient
{
    unique_ptr< Event<TextDelta> > textChanged_;

public:
    TestEmitter():
        textChanged_(new Event<TextDelta>)
    {}

    Event<TextDelta> *textChanged() const { return textChanged_.get(); }

    void paste(int s0, int s1, const string &chunk) {
        // do smth.
        textChanged()->notify(TextDelta(s0, s1, chunk));
    }
};

class TestReceiver: public Recipient
{
    int dummy_;

public:
    void onTextChanged(TextDelta delta)
    {
        dummy_ += (delta.s0_ * delta.s1_);
    }
};

class BulkEmissionTest
{
    unique_ptr<TestEmitter> e_;
    unique_ptr<TestReceiver> r_;

public:
    BulkEmissionTest():
        e_(new TestEmitter),
        r_(new TestReceiver)
    {
        e_->textChanged()->connect(r_.get(), &TestReceiver::onTextChanged);
    }

    void run() {
        cout << "run()..." << endl;
        string chunk = "Abc";
        high_resolution_clock::time_point t0 = high_resolution_clock::now();
        const int n = 10000000;
        for (int i = 0; i < n; ++i)
            e_->paste(i, i + 1, chunk);
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        double dt = duration_cast<duration<double>>(t1 - t0).count();
        cout << "took " << dt << " s" << endl;
        cout << "=> " << n / dt << " Hz" << endl;
    }
};


int main()
{
    make_shared<BulkEmissionTest>()->run();
    return 0;
}
