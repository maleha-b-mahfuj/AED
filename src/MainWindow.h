#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QPushButton>
#include <QHBoxLayout>

#include "ui_mainwindow.h"

#include "Person.h"
#include "AED.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
