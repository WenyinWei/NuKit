#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("Nukit V0.2"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_simulateButton_clicked()
/* Once we click on the simulate button, the series of work to compute the burning procedure has been started.
 *
 * 1. Read the input element density information according to the user specification.
 * 2. There are two approaches to acquire the relevant parameters of the concerned reactions.
 *   2.1 Invoke the python module to calculate the whole decay module.
 *   2.2 Utilize the local nuclear database directly. To ensure time is enough, we take this approach first.
 * 3. Use the parameters and neutron set by the user to construct the ode matrix.
 * 4. Invoke odeint to figure out the ode problem.
 * 5. Visualize the change procedure by QtBarWindow.
 */
{
    QString original_text = this->ui->inputText->toPlainText();
    QStringList text_list = original_text.split("\n");
    QVector<QStringList> text_table;
    for (int i = 0; i < text_list.size(); i++)
    {
        text_table.append(text_list[i].split(" "));
        //        qDebug()<<text_table[i][0];
    };
    bar_window_list.push_back(new QtBarWindow(this));
    bar_window_list.back()->show();
}
