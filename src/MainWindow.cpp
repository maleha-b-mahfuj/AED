#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ui = new Ui::MainWindow;
    ui->setupUi(this);

    setMinimumSize(1280,720);

    QWidget* mainCentral = new QWidget; // Holds all other widgets
    QHBoxLayout* mainLayout = new QHBoxLayout(mainCentral); // sets widgets horizontally

    AED* aedBox = new AED;
    mainLayout->addWidget(aedBox->drawAED());
    // Person object
    // UI created, added to widget after connection setup
    Person* person = new Person(parent);
    QGraphicsView *view = new QGraphicsView(person->getPersonScene());
    // Connections for interactions between AED and Person
    //*
    connect(aedBox, &AED::scissorsPressed, person, &Person::showScissors);
    connect(aedBox, &AED::adultPadsPressed, person, &Person::checkElectroPads);
    connect(aedBox, &AED::childPadsPressed, person, &Person::checkElectroPads);
    connect(aedBox, &AED::shockButtonPressed, person, &Person::deliverShock);
    connect(aedBox, &AED::chestCompressed, person, &Person::deliverCPR);
    connect(person, &Person::scissorsDone, aedBox, &AED::showElectrodes);
    connect(person, &Person::electrodesAreAttached, aedBox, &AED::attachElectrodes);
    connect(person, &Person::patientNeedsShock, aedBox, &AED::deliveringShock);
    connect(person, &Person::patientNoShock, aedBox, &AED::noShock);
    connect(person, &Person::patientShocked, aedBox, &AED::doCPR);

    connect(person, &Person::wrongElectrodes, aedBox, [aedBox](){
        if (aedBox->getIsAdult()){
            aedBox->updateDisplayText("Wrong Electrodes - Too Big");
        } else {
            aedBox->updateDisplayText("Wrong Electrodes - Too Small");
        }

    });
//*
    mainLayout->addWidget(view);
    setCentralWidget(mainCentral);
}

MainWindow::~MainWindow()
{
    delete ui;
}

