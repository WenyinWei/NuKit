#pragma once
#include <QProcess>
#include <QString>
#include <QThread>

class thread_python : public QThread
{
public:
    QProcess *m_process = new QProcess(this);
    QStringList inputList;

private:
    void run();

public:
    thread_python(QStringList inputList);
};
