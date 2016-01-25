#include <iostream>
#include <tt/event>

using namespace tt;
using namespace std;

class LineEdit: public Recipient
{
    string text_;
    unique_ptr< Event<string> > textChanged_;

    void repaint(string text) {
        cerr << "repaint(): text = " << text << endl;
    }

public:
    LineEdit(string text):
        text_(text),
        textChanged_(new Event<string>)
    {
        textChanged_->connect(this, &LineEdit::repaint);
    }

    string text() const { return text_; }

    void setText(string newText) {
        if (text_ == newText) return;
        text_ = newText;
        textChanged_->notify(text_);
    }

    Event<string> *textChanged() { return textChanged_.get(); }
};

struct CheckInput: public Recipient
{
    void check(string text) { cerr << "check(): text = " << text << endl; }
};

int main()
{
    auto lineEdit = make_shared<LineEdit>("Hello!");
    auto checkInput = make_shared<CheckInput>();
    lineEdit->textChanged()->connect(checkInput, &CheckInput::check);
    lineEdit->setText("Tic, tac!");
    checkInput.reset();
    lineEdit->setText("A, B, C...");
    return 0;
}
