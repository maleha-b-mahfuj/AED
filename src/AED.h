#ifndef AED_H
#define AED_H

#include <QDir>
#include <QDebug>
#include <QObject>
#include <QPushButton>
#include <QGraphicsView>
#include <QTextEdit>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QVector>
#include <QGroupBox>
#include <QTimer>
#include <QHBoxLayout>
#include <QSlider>
#include <iostream>
#include <random>
#include "Person.h"
#include "states.h"

/*! Automated external defibrillator.
 * 1) Specifies a user interface to be used in a main window.
 * 2) Simulates a real-life AED system.
*/
class AED : public QObject {
    Q_OBJECT

    public:
        explicit AED(QObject *parent = nullptr);
        ~AED();
        /// Creates the GUI for the AED
        QGraphicsView* drawAED();

        void checkResponsiveness(); //step 2
        void callForHelp();   //step 3
        void electrodeStep();  //step 4
        void highlightImageBorder(int step); //hightlights the appropriate image
        void updateDisplayText(QString text);
        bool getIsAdult() {
            return isAdult;
        }
        void analyzingStep();   //step 5


        PowerState getPower();
        PowerState togglePower();
        void attachElectrode(Person *Person);

    public slots:
        void deliveringShock();
        void noShock();
        void doCPR();
        /// Performs checks on the AED and enters the first step.
        void startAED();
        void showElectrodes();  //show the "attach" button for the electrodes
        void attachElectrodes(Person* patient);  //connect the electrode pads to the aed

    private:
        QGraphicsView *view;
        QGraphicsScene* display; // the central white box
        QGraphicsScene* mainScene;
        bool isAedOperational; // assumed to always be true
        QVector<QGraphicsRectItem*> imgBorders;
        bool isAdult; //indicates which electrodaes where chosen
        QTimer ambulance_timer;
        int count =0;
        QPushButton* scissors;
        QPushButton* attachPads;
        QGroupBox* adultPads;
        QGroupBox* childPads;
        Person* patient; //not initialized until electrodes have been placed
        QPushButton* shockButton;
        QSlider* compressionSlider; // used to set the rate of chest compressions during CPR phase
        PowerState power;
        Stage currentStage;
        int last_compression_value = 0;
        QGraphicsTextItem *timer_text;
        
    signals:
        void stateFinished();
        void scissorsPressed();
        void adultPadsPressed(bool adult);
        void childPadsPressed(bool adult);
        void analyzePatient();
        void shockButtonPressed();
        void chestCompressed();
};

#endif // AED_H
