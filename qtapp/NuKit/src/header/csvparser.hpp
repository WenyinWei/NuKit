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
    QStringList CSVParser::getHeader();
    QVector<QVector<double>> CSVParser::getdDataWHead();
    QVector<QVector<int>> CSVParser::getiDataWHead();

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
};

QStringList CSVParser::getHeader() // You can also get the data from this class
{
    QTextStream stream(CSVFile_.get());
    return stream.readLine().split(",");
};

QVector<QVector<double>> CSVParser::getdDataWHead() // You can also get the data from this class
{
    QVector<QVector<double>> data2D;
    QTextStream stream(CSVFile_.get());
    stream.readLine();
    while (!stream.atEnd())
    {
        QStringList RowdataStr = stream.readLine().split(",");
        QVector<double> Rowdata;
        for (size_t i = 0; i < RowdataStr.size(); i++)
            Rowdata << RowdataStr[i].toDouble();
        data2D<<Rowdata;
    }
    return data2D;
};

QVector<QVector<int>> CSVParser::getiDataWHead() // You can also get the data from this class
{
    QVector<QVector<int>> data2D;
    QTextStream stream(CSVFile_.get());
    stream.readLine();
    while (!stream.atEnd())
    {
        QStringList RowdataStr = stream.readLine().split(",");
        QVector<int> Rowdata;
        for (size_t i = 0; i < RowdataStr.size(); i++)
            Rowdata << RowdataStr[i].toInt();
        data2D<<Rowdata;
    }
    return data2D;
};
