#include "thread_python.h"

void thread_python::run()
{
    //it prints everything , even errors
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    // connect(getDecayChain, &QProcess::readyReadStandardOutput, this->ui->messageBrowser, [this] {this->ui->messageBrowser->append("I am running"); });
    // connect(getDecayChain, &QProcess::readyReadStandardOutput, [=] { this->ui->messageBrowser->append("I am running"); });

    // If you want to use the IAEA spider python file, please use the following input format.
    // inputList.append("../src/DataClass/MultiIAEASpider.py");
    // for (int i = 0; i < decayElements.length(); i++)
    // {
    //     decayElements[i] = "'" + decayElements[i] + "'";
    // };
    // inputList.append("set([" + decayElements.join(",") + "])");
    m_process->start("python", inputList);
    m_process->waitForFinished();
}

thread_python::thread_python(QStringList inputList) : inputList(inputList) {}
