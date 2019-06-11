#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QThread>
#include <QProcess>               // Invoke python spider
#include <QtScript/QScriptEngine> // For input the neutron flux
#include <QDebug>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <map> /* map */

#include <boost/numeric/odeint.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>

#include "../DataClass/element.hpp"
#include "../Odeint/odeoutput.hpp"
#include "../Odeint/stiffsys.hpp"
#include "../Thread/thread_calReact.h"
//using namespace std;
//using namespace boost::numeric::odeint;
//namespace phoenix = boost::phoenix;

//[ stiff_system_definition
typedef boost::numeric::ublas::vector<double> vector_type;
typedef boost::numeric::ublas::matrix<double> matrix_type;

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

// Full automatic operation is not supported now, however, part of it has been done.
//  Transition: 1. Decay Part

//    thread_getDecayInfo *getDecayInfo_thread = new thread_getDecayInfo(decayElements);

//    connect(getDecayInfo_thread->m_process, &QProcess::readyRead, [=] { this->ui->messageBrowser->append(getDecayInfo_thread->m_process->readAll()); });
//    connect(getDecayInfo_thread, SIGNAL(finished()), getDecayInfo_thread, SLOT(deletelater()));
//    connect(getDecayInfo_thread, SIGNAL(finished()), getNeutronInfo_thread, SLOT(start()));
//    connect(getNeutronInfo_thread, SIGNAL(finished()), organizeReact_thread, SLOT(start()));
//    connect(organizeReact_thread, SIGNAL(finished()), calReact_thread, SLOT(start()));
{
    QString original_text = this->ui->inputText->toPlainText();
    QStringList text_list = original_text.split("\n");
    QVector<QStringList> text_table;
    QStringList Elements, ElementDensities;
    QVector<double> ElementsDensity;
    qDebug() << "Everything works well now";
    for (int i = 0; i < text_list.size(); i++)
    {
        QStringList tmp = text_list[i].split(" ");
        this->ui->messageBrowser->append(text_list[i]);
        text_table.append(tmp);
        Elements << tmp[0];
        ElementsDensity << tmp[1].toDouble();
    };

    // Invoke the calculation thread, let it run in the background.
    thread_calReact *calReact_thread = new thread_calReact(this->ui->inputNeutron->text(),
                                                           this->ui->inputTime->text().toDouble(),
                                                           this->ui->inputTimeStep->text().toDouble(),
                                                           Elements, ElementsDensity);
    connect(calReact_thread, SIGNAL(sig_report_message(QString)), this->ui->messageBrowser, SLOT(append(QString)));
    connect(calReact_thread, SIGNAL(sig_set_progress(double)), this, SLOT(set_progress(double)));
    connect(calReact_thread, SIGNAL(finished()), this, SLOT(visualize_data()));
    connect(calReact_thread, SIGNAL(finished()), calReact_thread, SLOT(deletelater()));
    calReact_thread->start();
}

void MainWindow::on_visualizeButton_clicked()
{
    visualize_data();
}

void MainWindow::visualize_data()
{
    bar_window_list.push_back(new QtBarWindow(this));
    bar_window_list.back()->show();
}

void MainWindow::set_progress(double time)
{
    double total_time = this->ui->inputTime->text().toDouble();
    this->ui->progressBar->setValue((int)(time / total_time * 100));
}
