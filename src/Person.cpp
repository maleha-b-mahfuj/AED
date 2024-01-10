#include "Person.h"

Person::Person(QWidget *parent) : QWidget(parent){
    setFixedSize(300,300);
    scene =  new QGraphicsScene;
    scene->addWidget(this);
    patientState = state::ventricularFib;
    srand(time(0));
    age = (std::rand() % 16) + 15;
    scene->addSimpleText("Age: " + QString::number(age));

}

Person::~Person(){
    delete scene;
}

void Person::showElectrodes(){
    QGraphicsRectItem* elect1 = new QGraphicsRectItem(115,100,20,30);
    QGraphicsRectItem* elect2 = new QGraphicsRectItem(175,160,20,30);
    elect1->setBrush(QBrush(QColor("#08435e")));
    elect1->setPen(Qt::NoPen);
    elect2->setBrush(QBrush(QColor("#08435e")));
    elect2->setPen(Qt::NoPen);
    scene->addItem(elect1);
    scene->addItem(elect2);
}

QGraphicsScene* Person::getPersonScene(){
    return scene;
}

//shows the animation of the scissors
void Person::showScissors(){
    QGraphicsLineItem* line = new QGraphicsLineItem(155,90,155,200);
    line->setPen(Qt::DotLine);
    scene->addItem(line);
    QPixmap image(":/assets/scissors.png");
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(image);
    pixmapItem->setScale(0.8);
    pixmapItem->setPos(132,175);
    scene->addItem(pixmapItem);

    QGraphicsItemAnimation *animation = new QGraphicsItemAnimation;
    animation->setItem(pixmapItem);

    QTimeLine *timeLine = new QTimeLine(4000);
    timeLine->setFrameRange(0, 200);

    QObject::connect(timeLine, &QTimeLine::valueChanged, [=](qreal value) {
         pixmapItem->setPos(pixmapItem->x(), pixmapItem->y() - 2*value);
    });

    QObject::connect(timeLine, &QTimeLine::finished, this, [this, pixmapItem](){
        QGraphicsRectItem* body = new QGraphicsRectItem(105,90,100,110);
        scene->addItem(body); //body
        body->setBrush(QBrush(QColor("#c47a12")));
        scene->removeItem(pixmapItem);
        emit scissorsDone();
    });

     animation->setTimeLine(timeLine);
     timeLine->start();

}

void Person::deliverShock() {
    srand(time(0));
    float luck = float(std::rand()) / float(RAND_MAX); // random number 0-1 (inclusive)

    // only 50% chance of success
    if(luck >= 0.5){
        patientState = state::healthy;
    }
    emit patientShocked();
}

void Person::deliverCPR() {
    srand(time(0));
    float luck = float(std::rand()) / float(RAND_MAX); // random number 0-1 (inclusive)

    // only 25% chance of success
    if(luck >= 0.75){
        patientState = state::healthy;
    }
}

void Person::generateNormalSinusRhythm() {
    //patientState = healthy;
    QVector<double> ecgData;
    int numPointsPerCycle = 15;
    int numOfCycles = 9; //three full cycles

    for (int cycle=0; cycle < numOfCycles; cycle++){
        //straight line segment
        for (int j=0; j < 3; ++j) {
            ecgData.append(1);
        }

        //P Wave
        double pWaveAmplitude = 0.5;
        double pWaveFrequency = 2.5;
        for (int j=1; j < 4; ++j) {
            double pWaveValue = (pWaveAmplitude * sin(2.0 * M_PI * pWaveFrequency * j/numPointsPerCycle))+1;
            ecgData.append(pWaveValue);

        }
        ecgData.append(1);

        //slight down curve
        double qWaveAmplitude = 1.0;
        double qWaveFrequency = 1.5;
        double qWaveValue = (qWaveAmplitude * sin(2.0 * M_PI * qWaveFrequency * 1/numPointsPerCycle + M_PI)+1);
        ecgData.append(qWaveValue);

        //qrs curve
        double qrsWaveAmplitude = 2.25;
        double qrsWaveFrequency = 4;
        for (int j=1; j <=2; ++j) {
            double qrsWaveValue = qrsWaveAmplitude * sin(2.0 * M_PI * qrsWaveFrequency * j/numPointsPerCycle);
            ecgData.append(qrsWaveValue);
        }
        ecgData.append(1);
        ecgData.append(1);

        double tWaveAmplitude = 0.75;
        double tWaveFrequency = 2.5;
        for (int j=1; j <= 3; ++j) {
            double tWaveValue = (tWaveAmplitude * sin(2.0 * M_PI * tWaveFrequency * j/numPointsPerCycle))+1;
            ecgData.append(tWaveValue);
        }
        ecgData.append(1);
    }

    qDebug() << "size of ecg:" << ecgData.size();
    voltageValues = ecgData;
    qDebug() << "size of ecg:";
}

void Person::checkState(){
    switch(patientState){
    case(ventricularFib):
        emit patientNeedsShock();
        break;
    case(ventricularTach):
        emit patientNeedsShock();
        break;
    case(asystole):
        emit patientNoShock();
        break;
    case(healthy):
        emit patientNoShock();
        break;
    default:
        break;
    }
}

void Person::generateVoltageValues(){
    switch(patientState) {
    case healthy:
        generateNormalSinusRhythm();
        break;
    case asystole:
        generateAsystole();
        break;
    case ventricularFib:
        generateVentricularFibrillation();
        break;
    case ventricularTach:
        generateVentricularTachycardia();
        break;
    }

}


void Person::generateAsystole() {
    //patientState = asystole;
    QVector<double> ecgData;

    int numDataPoints = 16 * 3; //chose these numbers based on how i generated the normal wave

    int asystoleValue = 1;

    for (int i = 0; i < numDataPoints; ++i) {
        ecgData.append(asystoleValue * QRandomGenerator::global()->bounded(0.5));
    }
    voltageValues = ecgData;
}


void Person::generateVentricularFibrillation() {
    QVector<double> ecgData;

    int numDataPoints = 16 * 3;
    for (int i = 0; i < numDataPoints; ++i) {
        double randomValue = static_cast<double>(rand()) / RAND_MAX * 3.0 - 0.5;
        ecgData.append(randomValue);

    }
    voltageValues = ecgData;
}


void Person::generateVentricularTachycardia() {
    //patientState = ventricularTach;

    QVector<double> ecgData;
    int numDataPoints = 16*3;

    const double QRSComplexAmplitude = 1.5;

    for (int i = 0; i < numDataPoints; ++i) {
        double time = static_cast<double>(i) / heartRate; // Time in seconds
        double qrsComplex = QRSComplexAmplitude * std::sin(2.0 * M_PI * time * 10.0)+1;

        ecgData.append(qrsComplex);
    }
    voltageValues = ecgData;
}

//check if the right electrode pads where chosen
void Person::checkElectroPads(bool padSensor){
    if ((age > 18 && padSensor) || (age <= 18 && !padSensor)) {
        showElectrodes();
        emit electrodesAreAttached(this);
    } else {
        emit wrongElectrodes();
    }
}

