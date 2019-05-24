#pragma once
#include <QStringList>
#include <QFile>
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
