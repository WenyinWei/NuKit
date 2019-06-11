#pragma once

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

class write_on_messagebrowser
{
    size_t m_every, m_count;
    thread_calReact *m_thread;

    write_on_messagebrowser(thread_calReact *m_thread, size_t every = 10)
        : m_every(every), m_count(0), m_thread(m_thread) {}

    void operator()(const vector_type &x, double t)
    {
        if ((m_count % m_every) == 0)
        {
            std::clog << t << std::endl;
            std::cout << "sp '-'" << std::endl;
            for (size_t i = 0; i < x.size(); ++i)
                std::cout << "i: " << i << "\tx(i):" << x(i) << std::endl;
            std::cout << "e" << std::endl
                      << std::endl;
        }
        ++m_count;
    }
};
//class thread_plotElementsNumber : public QThread
//{
//public:
//    QProcess *m_process = new QProcess(this);
//    QStringList decayElements;

//private:
//    void run()
//    {
//        m_process->setProcessChannelMode(QProcess::MergedChannels); //it prints everything , even errors
//                                                                    //    connect(getDecayChain, &QProcess::readyReadStandardOutput, this->ui->messageBrowser, [this] {this->ui->messageBrowser->append("I am running"); });
//        //    connect(getDecayChain, &QProcess::readyReadStandardOutput, [=] { this->ui->messageBrowser->append("I am running"); });

//        QStringList decayInput;
//        decayInput.append("../src/DataClass/MultiIAEASpider.py");
//        for (int i = 0; i < decayElements.length(); i++)
//        {
//            decayElements[i] = "'" + decayElements[i] + "'";
//        };
//        decayInput.append("set([" + decayElements.join(",") + "])");
//        m_process->start("python", decayInput);
//        m_process->waitForFinished();
//    }

//public:
//    thread_getDecayInfo(QStringList decayElements) : decayElements(decayElements) {}
//};
