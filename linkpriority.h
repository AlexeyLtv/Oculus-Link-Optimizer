#ifndef LINKPRIORITY_H
#define LINKPRIORITY_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class LinkPriority;
}
QT_END_NAMESPACE

class LinkPriority : public QMainWindow
{
    Q_OBJECT

public:
    LinkPriority(QWidget *parent = nullptr);
    ~LinkPriority();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_Save_clicked();

private:
    Ui::LinkPriority *ui;
};
#endif // LINKPRIORITY_H
