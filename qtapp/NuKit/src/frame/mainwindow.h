#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qtbarwindow.h"
#include <QMainWindow>
#include <QProcess>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public Q_SLOTS:
    void on_simulateButton_clicked();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QVector<QPointer<QtBarWindow>> bar_window_list;
    QProcess* m_process;
};

#endif // MAINWINDOW_H
