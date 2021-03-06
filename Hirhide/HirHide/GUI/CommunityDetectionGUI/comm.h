#ifndef COMM_H
#define COMM_H

#include <QWidget>

#include "../../Graph.h"

#include <QLabel>

namespace Ui {
class Comm;
}

class Comm : public QWidget
{
    Q_OBJECT

public:
    explicit Comm(QWidget *parent = 0);
    ~Comm();

    Communities comm1;

    Graph * pg;
    Communities * comm2;
    QLabel *info;
    QLabel * cinfo;

    Comm * w2;

    QString * cliq;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void on_t_cellClicked(int row, int column);

private:
    Ui::Comm *ui;
};

#endif // COMM_H
