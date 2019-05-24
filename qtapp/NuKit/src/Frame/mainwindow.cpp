#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QProcess> // Invoke python spider
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
    QStringList decayElements, fissionElements, absorbtionElements;
    for (int i = 0; i < text_list.size(); i++)
    {
        QStringList tmp = text_list[i].split(" ");
        qDebug() << tmp;
        text_table.append(tmp);
        decayElements << tmp[0];
        fissionElements << tmp[0];
        absorbtionElements<< tmp[0];
    };
    //TODO: Acquire the user-input neutron flux
    QString n_flux_text = this->ui->inputNeutron->text();
    QScriptEngine expression;
    double my_val = expression.evaluate(n_flux_text.repeated(1).replace("T", "10")).toNumber();
    qDebug() << "This is the value" << my_val;
    //TODO: Calculate the group correspondent cross section

    //TODO: Calculate the transition matrix
        //Routine: Recycle to find any new elements appear in the reactor
        //Repeat:
        //TODO: Transition: 1. Decay Part
        QStringList decayInput;
        decayInput.append("../src/DataClass/MultiIAEASpider.py");
        for (size_t i=0; i < decayElements.length(); i++) {
            decayElements[i] = "'" + decayElements[i] + "'";
        };
        decayInput.append("set([" + decayElements.join(",") + "])");
        QProcess::execute("dir");
        QProcess::execute("python", decayInput);
        //TODO: Transition: 2. Fission Part
        //TODO: Transition: 3. Neutron Absorbtion Part
        //TODO: Read the reaction file to judge hom many elements are added requiring more analysis.

        std::ifstream inFile("../data/DecayInfo.csv", std::ios::in);
        std::string lineStr;
        std::vector<std::vector<std::string>> strArray;
        while (getline(inFile, lineStr))
        {
            // store the data as 2D table 存成二维表结构
            std::stringstream ss(lineStr);
            std::string str;
            std::vector<std::string> lineArray;
            // extracted cell data seperated by the ',' 按照逗号分隔
            while (getline(ss, str, ','))
                lineArray.push_back(str);
            strArray.push_back(lineArray);
        }

        std::vector<Element> vElement;
        for (size_t i = 1; i < strArray.size(); i++)
        {
            Element ele1(strArray[i][3], stoi(strArray[i][1]), stoi(strArray[i][2]));
            Element ele2(strArray[i][13], stoi(strArray[i][11]), stoi(strArray[i][12]));
            vElement.push_back(ele1);
            vElement.push_back(ele2);
        }
        // Now we clear the duplicated elements and sort the vector
        sort(vElement.begin(), vElement.end(), sort_element);
        std::vector<Element>::iterator unque_it = std::unique(vElement.begin(), vElement.end());
        vElement.erase(unque_it, vElement.end());

        for (size_t i = 0; i < vElement.size(); i++)
            vElement[i].Eindex = i;

        for (size_t i = 1; i < strArray.size(); i++)
        {
            Element ele1(strArray[i][3], stoi(strArray[i][1]), stoi(strArray[i][2]));
            Element ele2(strArray[i][13], stoi(strArray[i][11]), stoi(strArray[i][12]));
            int index1, index2;
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
            NuReaction tmpNuReaction;
            double d = atof(strArray[i][16].c_str());
            tmpNuReaction.ReConst = d;
            tmpNuReaction.Retype = strArray[i][8];
            tmpNuReaction.ReDaughter = &vElement[index2];
            tmpNuReaction.ReMother = &vElement[index1];
            vElement[index1].ReactionAsM.push_back(tmpNuReaction);
            vElement[index2].ReactionAsD.push_back(tmpNuReaction);
        }

        vector_type x(vElement.size());
        for (size_t i = 0; i < vElement.size(); i++)
        {
            x(i) = 0;
        }
        for (size_t i = 0; i < text_table.size(); i++)
        {
            for (size_t j = 0; j < vElement.size(); j++)
            {
                qDebug() << "Input Element: " << (text_table[i][0]);
                qDebug() << "OneElement: " << QString::fromStdString(vElement[j].outname());
                qDebug() << "Input Element: ";
                qDebug() << "OneElement: " ;
                if (text_table[i][0] == QString::fromStdString(vElement[j].outname()))
                {
                    x(j) = text_table[i][1].toDouble();
                }
            }
        }
        write_element_list(vElement);

        for (size_t i = 0; i < vElement.size(); i++)
            vElement[i].Eindex = i;

        matrix_type J(vElement.size(), vElement.size());
        for (size_t i = 0; i < vElement.size(); i++)
            for (size_t j = 0; j < vElement.size(); j++)
                J(i, j) = 0;
        for (size_t i = 0; i < vElement.size(); i++)
        {
            for (size_t j = 0; j < vElement[i].ReactionAsD.size(); j++)
                J(i, vElement[i].ReactionAsD[j].ReMother->Eindex) += vElement[i].ReactionAsD[j].ReConst;
            for (size_t j = 0; j < vElement[i].ReactionAsM.size(); j++)
                J(i, i) -= vElement[i].ReactionAsM[j].ReConst;
        }

        stiff_system stiff;
        stiff.iJ = J;
        stiff_system_jacobi stiffj;
        stiffj.iJ = J;

        boost::numeric::odeint::observer_collection<vector_type, double> obs;
        obs.observers().push_back(write_on_terminal(1));
        obs.observers().push_back(write_csv(vElement, 10,"../data/oderun.csv"));

        size_t num_of_steps = boost::numeric::odeint::integrate_const(boost::numeric::odeint::make_dense_output<boost::numeric::odeint::rosenbrock4<double>>(1.0e-6, 1.0e-6),
                                              std::make_pair(stiff, stiffj),
                                              x, 0.0, 100000.0, 20.0, boost::ref(obs));
        //]

        qDebug() << num_of_steps << endl;

        //If there is new elements added, the loop continue, else break.
    //Repeat until no new elements is added.
    //TODO: set the matrix from the reaction csv files and set the initial conditions for ode
    //TODO: calculate the ode
    bar_window_list.push_back(new QtBarWindow(this));
    bar_window_list.back()->show();
}
