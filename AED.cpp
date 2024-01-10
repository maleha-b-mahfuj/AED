#include "AED.h"

AED::AED(QObject *parent)
    : QObject{parent}
{
    display = new QGraphicsScene;
    mainScene = new QGraphicsScene;
    view = new QGraphicsView(mainScene);
    scissors = new QPushButton("scissors");
    attachPads = new QPushButton("Connect");
    adultPads = new QGroupBox("Adult Pads");
    childPads = new QGroupBox("Child Pads");
    compressionSlider = new QSlider(Qt::Horizontal);
    compressionSlider->setMinimum(0);
    compressionSlider->setMaximum(3);
    //timer.setInterval(3000);
    srand(time(0));
    float battery_luck = float(std::rand())/float(RAND_MAX);
    // 85% chance battery is  OK, 15% battery too low
    if( battery_luck < 0.85f){
       isAedOperational = true;
    }
    else{
        isAedOperational = false;
    }

    ambulance_timer.setInterval(120*1000);
    ambulance_timer.setSingleShot(true);

    QObject::connect(&ambulance_timer, &QTimer::timeout, [this]() {
        QGraphicsTextItem *ambulance_text = new QGraphicsTextItem("Ambulance is here!\nStop, and let them handle the rest");
        ambulance_text->setScale(2);
        ambulance_text->setPos(0,-90);
        mainScene->addItem(ambulance_text);
        view->setInteractive(false);
    });

    timer_text = new QGraphicsTextItem("");
    timer_text->setPos(550,-50);
    mainScene->addItem(timer_text);
}

AED::~AED()
{
    delete display;
    delete scissors;
    delete attachPads;
    delete adultPads;
    delete childPads;
    delete mainScene;
}

QGraphicsView* AED::drawAED(){
    QGraphicsRectItem* aedOutline = new QGraphicsRectItem(0,0,625,400); // green rectangle
    view->setScene(mainScene);
    mainScene->addItem(aedOutline);

    // The centre window the display
    QWidget *proxyWidget = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout(proxyWidget);
    layout->setMargin(0);
    QGraphicsView *displayView = new QGraphicsView(proxyWidget);

    displayView->setScene(display);
    proxyWidget->setGeometry(0.5 * aedOutline->rect().width() - 200,100,400,150);
    layout->addWidget(displayView);

    mainScene->addWidget(proxyWidget);

    aedOutline->setBrush(QBrush(QColor("#0c854e")));
    aedOutline->setPen(Qt::NoPen);

    // Adding images for each phase of the AED

    // Check responsiveness
    QGraphicsPixmapItem *firstStepImg = new QGraphicsPixmapItem(QPixmap(":/assets/aed1.png"));
    firstStepImg->setScale(0.6);
    firstStepImg->setPos(0,140); // left, center
    QGraphicsRectItem* firstStep = new QGraphicsRectItem(firstStepImg->boundingRect(), firstStepImg);
    firstStep->setPen(Qt::NoPen);
    imgBorders.append(firstStep);
    mainScene->addItem(firstStepImg);
    // Call ambulance
    QGraphicsPixmapItem *secondStepImg = new QGraphicsPixmapItem(QPixmap(":/assets/aed2.png"));
    secondStepImg->setScale(0.6);
    secondStepImg->setPos(0.5 * aedOutline->rect().width() - 41,0); // center, top
    QGraphicsRectItem* secondStep = new QGraphicsRectItem(secondStepImg->boundingRect(), secondStepImg);
    secondStep->setPen(Qt::NoPen);
    imgBorders.append(secondStep);
    mainScene->addItem(secondStepImg);
    // Attach electrodes
    QGraphicsPixmapItem *thirdStepImg = new QGraphicsPixmapItem(QPixmap(":/assets/aed3.png"));
    thirdStepImg->setScale(0.6);
    thirdStepImg->setPos(aedOutline->rect().width() - 86,140); // right, center
    QGraphicsRectItem* thirdStep = new QGraphicsRectItem(thirdStepImg->boundingRect(), thirdStepImg);
    thirdStep->setPen(Qt::NoPen);
    imgBorders.append(thirdStep);
    mainScene->addItem(thirdStepImg);
    // Analysis
    QGraphicsPixmapItem *fourStepImg = new QGraphicsPixmapItem(QPixmap(":/assets/aed4.png"));
    fourStepImg->setScale(0.6);
    fourStepImg->setPos(0.66 * aedOutline->rect().width() - 42,aedOutline->rect().height() - 68); // right(ish) bottom
    QGraphicsRectItem* fourStep = new QGraphicsRectItem(fourStepImg->boundingRect(), fourStepImg);
    fourStep->setPen(Qt::NoPen);
    imgBorders.append(fourStep);
    mainScene->addItem(fourStepImg);
    // CPR (cardiopulmonary resuscitation) i.e. chest compressions
    QGraphicsPixmapItem *fiveStepImg = new QGraphicsPixmapItem(QPixmap(":/assets/aed5.png"));
    fiveStepImg->setScale(0.6);
    fiveStepImg->setPos(0.33 * aedOutline->rect().width() - 62,aedOutline->rect().height() - 99); // left(ish) bottom
    QGraphicsRectItem* fiveStep = new QGraphicsRectItem(fiveStepImg->boundingRect(), fiveStepImg);
    fiveStep->setPen(Qt::NoPen);
    imgBorders.append(fiveStep);
    mainScene->addItem(fiveStepImg);

    QPushButton* powerButton = new QPushButton;
    powerButton->setIcon(QIcon(":/assets/offButton.png"));
    powerButton->setIconSize(QSize(35,35));
    powerButton->setGeometry(10,10,60,40);
    connect(powerButton, &QPushButton::clicked, this, [this, powerButton](){
        powerButton->setIcon(QIcon(":/assets/onButton.png"));
        powerButton->setDisabled(true); // cannot be turned off at any point
        startAED();
    });
    // disabled until a shock is required
    shockButton = new QPushButton("shock");
    shockButton->setFixedSize(50,30);
    shockButton->setGeometry(0.5 * aedOutline->rect().width() - 24,250,0,0);
    shockButton->setDisabled(true);

    //label for compression slider
    QGraphicsTextItem* compressionLabel = new QGraphicsTextItem;
    compressionLabel->setPos(400,425);
    compressionLabel->setTextInteractionFlags(Qt::TextInteractionFlag::NoTextInteraction);
    compressionLabel->setPlainText("CHEST COMPRESSION RATE");
    mainScene->addItem(compressionLabel);

    // initially disabled.
    compressionSlider->setFixedSize(200,30);
    compressionSlider->setGeometry(400,450,0,0);
    compressionSlider->setDisabled(true);

    mainScene->addWidget(compressionSlider);
    mainScene->addWidget(powerButton);
    mainScene->addWidget(shockButton);
    return view;
}

void AED::startAED(){
    QGraphicsTextItem* displayText = new QGraphicsTextItem;
    display->addItem(displayText);
    if (isAedOperational){
        //std::cout<<"bar"<<std::endl;
        displayText->setPlainText("AED OPERATIONAL");
        QTimer::singleShot(3000, this, [this](){
            checkResponsiveness();
        });
    } else {
        displayText->setPlainText("AED BATTERY CRITICALLY LOW\nFIND NEW AED\nCALL 911");
    }

}

//hightlights the appropriate image
void AED::highlightImageBorder(int stepNum){
    for (QGraphicsRectItem* border: imgBorders) {
        border->setPen(Qt::NoPen);
    };

    imgBorders[stepNum - 1]->setPen(QPen(QColor("#f0970a"), 3));
}

//updates the display
void AED::updateDisplayText(QString text){
    display->clear();
    QGraphicsTextItem* displayText = new QGraphicsTextItem;
    displayText->setTextInteractionFlags(Qt::NoTextInteraction);
    displayText->setPos(0,0);
    display->addItem(displayText);
    displayText->setPlainText(text);
}

//AED STEP 1
void AED::checkResponsiveness(){
    highlightImageBorder(1);
    updateDisplayText("CHECK RESPONSIVENESS");

    QTimer::singleShot(3000, this, [this](){
        callForHelp();
    });
}

//AED STEP 2: CALLS THE AMBULANCE
void AED::callForHelp(){
    highlightImageBorder(2);
    updateDisplayText("CALL 911 NOW");

    QTimer::singleShot(3000, this, [this](){
        ambulance_timer.start();
        timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
        electrodeStep();
    });
}

//AED PART 3: shows the scissors to cut the shirt
void AED::electrodeStep(){
    timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
    highlightImageBorder(3);
    updateDisplayText("USE THE SCISSORS");
    scissors->setGeometry(0,420,100,50);
    QIcon icon(":/assets/scissors.png");
    scissors->setIcon(icon);
    scissors->setIconSize(QSize(40, 40));
    mainScene->addWidget(scissors);
    connect(scissors, &QPushButton::clicked, this, [this](){
        timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
        emit scissorsPressed();
        scissors->setDisabled(true);
    });
}

//AED PART 3: connects the electrode pads to the AED
void AED::attachElectrodes(Person* patient1){
    timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
    adultPads->hide();
    childPads->hide();
    updateDisplayText("CONNECT PADS TO AED");
    attachPads->setGeometry(0,420,100,50);
    attachPads->setStyleSheet("background-color: #08435e; color: white");
    mainScene->addWidget(attachPads);

    //timer.start();
    connect(attachPads, &QPushButton::clicked, this, [this, patient1](){
        timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
        //timer.stop();
        //count = 0;
        attachPads->hide();
        QGraphicsLineItem* line1 = new QGraphicsLineItem(125,400,75,485);
        QGraphicsLineItem* line2 = new QGraphicsLineItem(275,400,325,485);
        line1->setPen(QPen(Qt::gray, 2));
        line2->setPen(QPen(Qt::gray, 2));
        mainScene->addItem(line1);
        mainScene->addItem(line2);

        //electrodes
        QGraphicsRectItem* elect1 = new QGraphicsRectItem(55,485,40,60);
        QGraphicsRectItem* elect2 = new QGraphicsRectItem(305,485,40,60);
        elect1->setBrush(QBrush(QColor("#08435e")));
        elect1->setPen(Qt::NoPen);
        elect2->setBrush(QBrush(QColor("#08435e")));
        elect2->setPen(Qt::NoPen);
        mainScene->addItem(elect1);
        mainScene->addItem(elect2);
        patient = patient1; //assigns person atttribute
        updateDisplayText("ANALYZING. DO NOT TOUCH PERSON");
        highlightImageBorder(4);
        QTimer::singleShot(3000, this, [this](){
            analyzingStep();
        });
    });
}

void AED::analyzingStep(){
    timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
    highlightImageBorder(4);
    patient->generateVoltageValues();
    QCustomPlot* customPlot = new QCustomPlot;
    customPlot->addGraph();

    QVector<double> ecgData = patient->getVoltageValues();
    QVector<double> timeData;
    int size = ecgData.size();

    for (int i=0; i < size; i++){
        timeData.append(i);
    }

    customPlot->graph(0)->setData(timeData, ecgData);
    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(customPlot, "ANALYZING", QFont("sans", 12, QFont::Bold)));

    customPlot->yAxis->setRange(-1 ,3);
    customPlot->xAxis->setRange(0,30);

    QGraphicsProxyWidget *proxyWidget = display->addWidget(customPlot);
         // Set the size of the QCustomPlot widget to match the scene size
    proxyWidget->setMinimumSize(display->sceneRect().size().toSize());
    proxyWidget->setMaximumSize(display->sceneRect().size().toSize());
    customPlot->replot();

    QTimer::singleShot(5000, this, [this](){
        timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
        patient->checkState();
    });
}

//AED PART 3: show the "attach" button for the electrodes
void AED::showElectrodes(){
    timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
    scissors->hide();
    updateDisplayText("ATTACH THE CORRECT ELECTRODES");

    adultPads->setStyleSheet("background-color: #08435e; color: white");
    adultPads->setGeometry(0,420,120,70);

    childPads->setStyleSheet("background-color: #08435e; color: white");
    childPads->setGeometry(140,420,120,70);

    QHBoxLayout* adultlayout = new QHBoxLayout;
    QHBoxLayout* childlayout = new QHBoxLayout;
    QPushButton *adultButton = new QPushButton("attach");

    connect(adultButton, &QPushButton::clicked, this, [this, adultButton](){
        timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
        isAdult = true;
        //count = 0;
        //timer.stop();
        emit adultPadsPressed(isAdult);
        adultButton->setDisabled(true);
    });

    QPushButton *childButton = new QPushButton("attach");

    connect(childButton, &QPushButton::clicked, this, [this, childButton](){
        timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
        isAdult = false;
        emit childPadsPressed(isAdult);
        childButton->setDisabled(true);
    });

    adultlayout->addWidget(adultButton);
    childlayout->addWidget(childButton);

    adultPads->setLayout(adultlayout);
    childPads->setLayout(childlayout);


    mainScene->addWidget(adultPads);
    mainScene->addWidget(childPads);
}

// Part 5.5 - Post-analysis shock
void AED::deliveringShock(){
    timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
    updateDisplayText("SHOCK ADVISED");
    QTimer::singleShot(3000, this, [this](){
        timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
        updateDisplayText("STAND CLEAR\nPRESS SHOCK BUTTON");
        shockButton->setEnabled(true);
        connect(shockButton, &QPushButton::clicked, this, [this](){
            emit shockButtonPressed();
        });
    });
}
void AED::noShock(){
    timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
    updateDisplayText("NO SHOCK REQUIRED");
    QTimer::singleShot(3000, this, [this](){
        timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
        doCPR();
    });
}
void AED::doCPR(){
    timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
    shockButton->setDisabled(true);
    compressionSlider->setDisabled(false);
    updateDisplayText("YOU MAY APPLY CHEST COMPRESSIONS\n(USE SLIDER)");
    highlightImageBorder(5);

    connect(compressionSlider, &QSlider::valueChanged, this, [this](int value){
        timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
        QString t;
        if(value > 2){
            t = QString("COMPRESSING TOO FAST!\n%1/second").arg(value);
        }
        else if(value == 2){
            t = QString("APPLYING COMPRESSIONS\n%1/second").arg(value);
        }
        else if(value == 1){
            t = QString("COMPRESSING SLOWLY\n%1/second").arg(value);
        }
        else if(value == 0){
            t = QString("NO COMPRESSIONS DETECTED");
        }
        updateDisplayText(t);
    });
      // after  10 seconds, go back to analysis
    QTimer::singleShot(10000, this, [this](){
        timer_text->setPlainText(QString("%1/120s").arg(120 - (ambulance_timer.remainingTime()/1000)));
        last_compression_value = compressionSlider->value();
        compressionSlider->setDisabled(true);
        compressionSlider->setValue(0);

        // use last compression value to determine effectiveness of compression
        if(last_compression_value == 2){
            emit chestCompressed();
        }
        last_compression_value = 0;

        analyzingStep();
    });
}
