#pragma once
#include "graphmodifier.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>

#include <QtGui/QScreen>
#include <QtGui/QFontDatabase>

class QtBarWindow : public QWidget
{
    Q_OBJECT

public:
    explicit QtBarWindow(QWidget *parent);

private:
    void createWindow();
};
