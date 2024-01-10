#ifndef Person_H
#define Person_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsItemAnimation>
#include <QTimeLine>
#include <QDebug>
#include <cmath>
#include <QRandomGenerator>
#include "qcustomplot.h"
#include <iostream>
#include <random>

class Person : public QWidget {
    Q_OBJECT

    public:
        /// Different states of the heart
        enum state{
            healthy,
            asystole,
            ventricularFib,
            ventricularTach
        };
        explicit Person(QWidget *parent = nullptr);
        ~Person();

        void showChest();
        void showElectrodes();
        void show() {
            scene->addSimpleText("boboobob");
        }

        QGraphicsScene* getPersonScene();
        void generateVoltageValues();

        void generateNormalSinusRhythm();
        void generateAsystole();
        void generateVentricularFibrillation();
        void generateVentricularTachycardia();
        void checkState();

        QVector<double> getVoltageValues(){return voltageValues;};

    public slots:
        void showScissors();
        void checkElectroPads(bool sensor);
        void deliverShock();
        void deliverCPR();

    private:
        QGraphicsScene* scene;
        int age;
        int heartRate = 90;
        QVector<double> voltageValues;
        state patientState;
        
    protected:
        // definition is only allowed within the header file
        void paintEvent(QPaintEvent *event) override
        {
            Q_UNUSED(event);
            QPainter painter(this);
            painter.setPen(QPen(QColor("#9c6313")));
            painter.setBrush(QBrush(QColor("#c47a12")));
            painter.drawEllipse(120,20,70,70);

            painter.setBrush(QBrush(QColor("#6b512e")));
            painter.drawEllipse(145,45,5,5); //eyes
            painter.drawEllipse(160,45,5,5);

            painter.setBrush(QBrush(QColor("#3071ab")));
            painter.drawRect(105,90,100,110);

            painter.setBrush(QBrush(QColor("#c47a12")));
            painter.drawRect(45,120,60,30); //left arm
            painter.drawRect(205,120,60,30); //right arm

            painter.setBrush(QBrush(QColor("#233535")));
            painter.drawRect(105,200,45,80); //left leg
            painter.drawRect(160,200,45,80); //right leg
        }

    signals:
        void scissorsDone();
        void wrongElectrodes();
        void electrodesAreAttached(Person* patient);
        void patientNeedsShock();
        void patientNoShock();
        void patientShocked();

};

#endif // Person_H
