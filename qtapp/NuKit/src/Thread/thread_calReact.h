#pragma once
#include <QObject>
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

class thread_calReact : public QThread
{
    Q_OBJECT
public:
    QProcess *m_process = new QProcess(this);
    QStringList decayInput;
signals:
    void sig_report_message(const QString);
    void sig_set_progress(double);

private:
    void run();

private:
    QString n_flux_text;
    QStringList th_elements;
    QVector<double> th_elementsDensity;
    double th_time, th_timestep;

public:
    explicit thread_calReact(QString n_flux_text, double inputTime, double inputTimeStep, QStringList Elements, QVector<double> ElementsDensity);
    void report_message(const QString);
    void set_progress(double);
};
