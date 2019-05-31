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

#include "element.hpp"
#include "odeoutput.hpp"
#include "stiffsys.hpp"

//using namespace std;
//using namespace boost::numeric::odeint;
//namespace phoenix = boost::phoenix;

//[ stiff_system_definition
typedef boost::numeric::ublas::vector<double> vector_type;
typedef boost::numeric::ublas::matrix<double> matrix_type;

class thread_getDecayInfo : public QThread
{
public:
    QProcess *m_process = new QProcess(this);
    QStringList decayElements;

private:
    void run()
    {
        m_process->setProcessChannelMode(QProcess::MergedChannels); //it prints everything , even errors
                                                                    //    connect(getDecayChain, &QProcess::readyReadStandardOutput, this->ui->messageBrowser, [this] {this->ui->messageBrowser->append("I am running"); });
        //    connect(getDecayChain, &QProcess::readyReadStandardOutput, [=] { this->ui->messageBrowser->append("I am running"); });

        QStringList decayInput;
        decayInput.append("../src/DataClass/MultiIAEASpider.py");
        for (int i = 0; i < decayElements.length(); i++)
        {
            decayElements[i] = "'" + decayElements[i] + "'";
        };
        decayInput.append("set([" + decayElements.join(",") + "])");
        m_process->start("python", decayInput);
        m_process->waitForFinished();
    }

public:
    thread_getDecayInfo(QStringList decayElements) : decayElements(decayElements) {}
};

class thread_calReact : public QThread
{
public:
    QProcess *m_process = new QProcess(this);
    QStringList decayInput;

private:
    void run()
    {
        m_process->setProcessChannelMode(QProcess::MergedChannels); //it prints everything , even errors
                                                                    //    connect(getDecayChain, &QProcess::readyReadStandardOutput, this->ui->messageBrowser, [this] {this->ui->messageBrowser->append("I am running"); });
        //    connect(getDecayChain, &QProcess::readyReadStandardOutput, [=] { this->ui->messageBrowser->append("I am running"); });
        //TODO: Transition: 2. Fission Part
        //TODO: Transition: 3. Neutron Absorbtion Part
        //TODO: Read the reaction file to judge hom many elements are added requiring more analysis.

        std::ifstream inDecayFile("../data/DecayInfo.csv", std::ios::in);
        std::ifstream inNeutronFile("../data/NeutronInfo.csv", std::ios::in);
        std::string lineStr;
        std::vector<std::vector<std::string>> DecaystrArray;
        std::vector<std::vector<std::string>> NeutronstrArray;
        qDebug() << "before reading the table, it works";
        while (getline(inDecayFile, lineStr))
        { // store the csv data file as 2D table 存成二维表结构
            std::stringstream ss(lineStr);
            std::string str;
            std::vector<std::string> lineArray;
            // extracted cell data seperated by the ',' 按照逗号分隔
            while (getline(ss, str, ','))
                lineArray.push_back(str);
            if (lineArray.size() <= 1)
                continue;
            DecaystrArray.push_back(lineArray);
        }

        while (getline(inNeutronFile, lineStr))
        { // store the csv data file as 2D table 存成二维表结构
            std::stringstream ss(lineStr);
            std::string str;
            std::vector<std::string> lineArray;
            // extracted cell data seperated by the ',' 按照逗号分隔
            while (getline(ss, str, ','))
                lineArray.push_back(str);
            if (lineArray.size() <= 1)
                continue;
            NeutronstrArray.push_back(lineArray);
        }

        std::vector<Element> vElement;
        for (size_t i = 1; i < DecaystrArray.size(); i++)
        {
            Element ele1(DecaystrArray[i][3], stoi(DecaystrArray[i][1]), stoi(DecaystrArray[i][2]));
            Element ele2(DecaystrArray[i][13], stoi(DecaystrArray[i][11]), stoi(DecaystrArray[i][12]));
            vElement.push_back(ele1);
            vElement.push_back(ele2);
        }

        for (size_t i = 1; i < NeutronstrArray.size(); i++)
        {
            Element ele1(NeutronstrArray[i][3], stoi(NeutronstrArray[i][1]), stoi(NeutronstrArray[i][2]));
            Element ele2(NeutronstrArray[i][13], stoi(NeutronstrArray[i][11]), stoi(NeutronstrArray[i][12]));
            vElement.push_back(ele1);
            vElement.push_back(ele2);
        }
        // Now we clear the duplicated elements and sort the vector
        sort(vElement.begin(), vElement.end(), sort_element);
        std::vector<Element>::iterator unque_it = std::unique(vElement.begin(), vElement.end());
        vElement.erase(unque_it, vElement.end());

        vector_type x(vElement.size());
        matrix_type DecayJ(vElement.size(), vElement.size());
        matrix_type NeutronJ(vElement.size(), vElement.size());
        for (size_t i = 0; i < vElement.size(); i++)
        {
            x(i) = 0;
            for (size_t j = 0; j < vElement.size(); j++)
            {
                DecayJ(i, j) = 0;
                NeutronJ(i, j) = 0;
            }
        }

        write_element_list(vElement);

        for (size_t i = 1; i < DecaystrArray.size(); i++)
        { // Fill the transition matrix with reaction constant or crossection
            // Attention, this one is for decay reaction
            Element ele1(DecaystrArray[i][3], stoi(DecaystrArray[i][1]), stoi(DecaystrArray[i][2]));
            Element ele2(DecaystrArray[i][13], stoi(DecaystrArray[i][11]), stoi(DecaystrArray[i][12]));
            size_t index1, index2;
            for (size_t j = 0; j < vElement.size(); j++)
                if (ele1 == vElement[j])
                {
                    index1 = j;
                    break;
                };
            for (size_t j = 0; j < vElement.size(); j++)
                if (ele2 == vElement[j])
                {
                    index2 = j;
                    break;
                };
            double d = atof(DecaystrArray[i][16].c_str());
            DecayJ(index1, index1) -= d;
            DecayJ(index2, index1) += d;
        }

        for (size_t i = 1; i < NeutronstrArray.size(); i++)
        { // Fill the transition matrix with reaction constant or crossection
            // Attention, this one is for decay reaction
            Element ele1(NeutronstrArray[i][3], stoi(NeutronstrArray[i][1]), stoi(NeutronstrArray[i][2]));
            Element ele2(NeutronstrArray[i][13], stoi(NeutronstrArray[i][11]), stoi(NeutronstrArray[i][12]));
            size_t index1, index2;
            for (size_t j = 0; j < vElement.size(); j++)
                if (ele1 == vElement[j])
                {
                    index1 = j;
                    break;
                };
            for (size_t j = 0; j < vElement.size(); j++)
                if (ele2 == vElement[j])
                {
                    index2 = j;
                    break;
                };
            double d = atof(NeutronstrArray[i][16].c_str());
            NeutronJ(index1, index1) -= d;
            NeutronJ(index2, index1) += d;
        }

        for (size_t i=0; i< th_elements.size(); i++)
        {
//            qDebug() << "This is the element"<< th_elements[i];
            for (size_t j = 0; j < vElement.size(); j++)
                if (th_elements[i] == QString::fromStdString(vElement[j].outname()))
                {
//                    qDebug() << QString::fromStdString(vElement[j].outname());
                    x(j) = th_elementsDensity[i];
                    break;
                };
        }

        stiff_system stiff;
        stiff.iDecayJ = DecayJ;
        stiff.iNeutronJ = NeutronJ;
        stiff.iNeutronText = n_flux_text;
        stiff_system_jacobi stiffj;
        stiffj.iDecayJ = DecayJ;
        stiffj.iNeutronJ = NeutronJ;
        stiffj.iNeutronText = n_flux_text;
        QScriptEngine expression;
        boost::numeric::odeint::observer_collection<vector_type, double> obs;
//        obs.observers().push_back(write_on_terminal(1));
        obs.observers().push_back(write_csv(vElement, 10, "../data/oderun.csv"));
        double sumofElements = 0;
        for (size_t i=0;i<x.size();i++)
        {
            sumofElements += x[i];
        }


        size_t num_of_steps = boost::numeric::odeint::integrate_const(boost::numeric::odeint::make_dense_output<boost::numeric::odeint::rosenbrock4<double>>(1.0e-6, 1.0e-6),
                                                                      std::make_pair(stiff, stiffj),
                                                                      x, 0.0, th_time, th_timestep, boost::ref(obs));


        //]

        qDebug() << num_of_steps << endl;
    }
private:
    QString n_flux_text;
    QStringList th_elements;
    QVector<double> th_elementsDensity;
    double th_time, th_timestep;

public:
    thread_calReact(QString n_flux_text, double inputTime, double inputTimeStep, QStringList Elements, QVector<double> ElementsDensity) :
        n_flux_text(n_flux_text),
        th_time(inputTime),
        th_timestep(inputTimeStep),
        th_elements(Elements),
        th_elementsDensity(ElementsDensity)
    {

    }
};


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
    //TODO: Acquire the user-input neutron flux
    QString n_flux_text = this->ui->inputNeutron->text();
    qDebug() << "Everything works well now 2 ";


    //TODO: Calculate the transition matrix
    //Routine: Recycle to find any new elements appear in the reactor
// Uncomment the following commands after debug
    thread_calReact *calReact_thread = new thread_calReact(n_flux_text,
                                                           this->ui->inputTime->text().toDouble(),
                                                           this->ui->inputTimeStep->text().toDouble(),
                                                           Elements, ElementsDensity);
    calReact_thread->start();
    //TODO: Transition: 1. Decay Part

    //    thread_getDecayInfo *getDecayInfo_thread = new thread_getDecayInfo(decayElements);

    //    connect(getDecayInfo_thread->m_process, &QProcess::readyRead, [=] { this->ui->messageBrowser->append(getDecayInfo_thread->m_process->readAll()); });
    //    connect(getDecayInfo_thread, SIGNAL(finished()), getDecayInfo_thread, SLOT(deletelater()));
    //    connect(getDecayInfo_thread, SIGNAL(finished()), getNeutronInfo_thread, SLOT(start()));
    //    connect(getNeutronInfo_thread, SIGNAL(finished()), organizeReact_thread, SLOT(start()));
    //    connect(organizeReact_thread, SIGNAL(finished()), calReact_thread, SLOT(start()));

    connect(calReact_thread, SIGNAL(finished()), this, SLOT(visualize_data()));

    //If there is new elements added, the loop continue, else break.
    //Repeat until no new elements is added.
    //TODO: set the matrix from the reaction csv files and set the initial conditions for ode
    //TODO: calculate the ode

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
