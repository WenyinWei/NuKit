#pragma once
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <boost/smart_ptr/shared_ptr.hpp>

class CSVParser
{
public:
    CSVParser(QString filePath);
    ~CSVParser(void);

public:
    bool setCSVData(const QStringList &CSVList);
    QStringList getCSVData();

private:
    boost::shared_ptr<QFile> CSVFile_;
};


CSVParser::CSVParser(QString filePath) // Initialization Function
{
    CSVFile_ = boost::shared_ptr<QFile>(new QFile(filePath));
    CSVFile_->open(QIODevice::ReadWrite);
}
CSVParser::~CSVParser(void) // Destruction Function
{
    CSVFile_->close();
}


// 注意， QStringList 的参数必须符合csv格式： 逗号分隔，\n结尾
bool CSVParser::setCSVData(const QStringList &CSVList) // You can set this class with your data
{
    if (CSVList.empty() || !CSVFile_->isOpen() || !CSVFile_->resize(0))
        return false;
    Q_FOREACH (QString str, CSVList)
    {
        CSVFile_->write(str.toUtf8());
    }
    CSVFile_->flush();
    return true;
}
QStringList CSVParser::getCSVData() // You can also get the data from this class
{
    QStringList CSVList;
    CSVList.clear();

    QTextStream stream(CSVFile_.get());
    while (!stream.atEnd())
    {
        CSVList.push_back(stream.readLine());
    }

    return CSVList;
}
