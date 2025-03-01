/********************************************************************************
** Form generated from reading UI file 'main_window.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PaperTrackerMainWindow
{
public:
    QStackedWidget *stackedWidget;
    QWidget *page;
    QLabel *ImageLabel;
    QPlainTextEdit *SSIDText;
    QPlainTextEdit *PasswordText;
    QScrollBar *BrightnessBar;
    QLabel *label;
    QPushButton *pushButton;
    QPlainTextEdit *LogText;
    QLabel *label_2;
    QTextEdit *textEdit;
    QLabel *label_16;
    QWidget *page_2;
    QLabel *label_3;
    QLabel *label_4;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QPlainTextEdit *plainTextEdit_3;
    QPlainTextEdit *plainTextEdit_17;
    QPlainTextEdit *plainTextEdit_15;
    QLabel *label_5;
    QPlainTextEdit *plainTextEdit_2;
    QLabel *label_10;
    QLabel *label_13;
    QLabel *label_6;
    QLabel *label_15;
    QPlainTextEdit *plainTextEdit_7;
    QPlainTextEdit *plainTextEdit_18;
    QPlainTextEdit *plainTextEdit_21;
    QPlainTextEdit *plainTextEdit_19;
    QLabel *label_7;
    QPlainTextEdit *plainTextEdit_13;
    QPlainTextEdit *plainTextEdit_16;
    QPlainTextEdit *plainTextEdit_20;
    QPlainTextEdit *plainTextEdit_9;
    QLabel *label_11;
    QPlainTextEdit *plainTextEdit_8;
    QLabel *label_9;
    QPlainTextEdit *plainTextEdit_4;
    QPlainTextEdit *plainTextEdit_6;
    QLabel *label_14;
    QPlainTextEdit *plainTextEdit_10;
    QPlainTextEdit *plainTextEdit_11;
    QLabel *label_8;
    QPlainTextEdit *plainTextEdit;
    QPlainTextEdit *plainTextEdit_12;
    QPlainTextEdit *plainTextEdit_14;
    QPlainTextEdit *plainTextEdit_5;
    QLabel *label_12;
    QPlainTextEdit *plainTextEdit_22;
    QPlainTextEdit *plainTextEdit_45;
    QLabel *label_30;
    QPlainTextEdit *plainTextEdit_46;
    QLabel *label_31;
    QPlainTextEdit *plainTextEdit_47;
    QPlainTextEdit *plainTextEdit_48;
    QPlainTextEdit *plainTextEdit_49;
    QLabel *label_32;
    QPlainTextEdit *plainTextEdit_50;
    QPlainTextEdit *plainTextEdit_51;
    QLabel *label_33;
    QPlainTextEdit *plainTextEdit_52;
    QPlainTextEdit *plainTextEdit_53;
    QLabel *label_34;
    QPlainTextEdit *plainTextEdit_54;
    QLabel *label_35;
    QPlainTextEdit *plainTextEdit_55;
    QPlainTextEdit *plainTextEdit_56;
    QPlainTextEdit *plainTextEdit_57;
    QLabel *label_36;
    QPlainTextEdit *plainTextEdit_58;
    QPlainTextEdit *plainTextEdit_59;
    QLabel *label_37;
    QPlainTextEdit *plainTextEdit_60;
    QPlainTextEdit *plainTextEdit_61;
    QLabel *label_38;
    QPlainTextEdit *plainTextEdit_62;
    QLabel *label_39;
    QPlainTextEdit *plainTextEdit_63;
    QPlainTextEdit *plainTextEdit_64;
    QPlainTextEdit *plainTextEdit_65;
    QLabel *label_40;
    QPlainTextEdit *plainTextEdit_66;
    QPlainTextEdit *plainTextEdit_67;
    QLabel *label_41;
    QPlainTextEdit *plainTextEdit_68;
    QPushButton *StartCalibrationButton;
    QComboBox *CalibrationModeComboBox;
    QPushButton *ResetMaxButton;
    QPushButton *ResetMinButton;
    QLabel *label_29;
    QPushButton *MainPageButton;
    QPushButton *CalibrationPageButton;

    void setupUi(QWidget *PaperTrackerMainWindow)
    {
        if (PaperTrackerMainWindow->objectName().isEmpty())
            PaperTrackerMainWindow->setObjectName("PaperTrackerMainWindow");
        PaperTrackerMainWindow->resize(835, 553);
        stackedWidget = new QStackedWidget(PaperTrackerMainWindow);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(0, 50, 841, 491));
        page = new QWidget();
        page->setObjectName("page");
        ImageLabel = new QLabel(page);
        ImageLabel->setObjectName("ImageLabel");
        ImageLabel->setGeometry(QRect(0, 0, 371, 261));
        SSIDText = new QPlainTextEdit(page);
        SSIDText->setObjectName("SSIDText");
        SSIDText->setGeometry(QRect(390, 30, 221, 41));
        PasswordText = new QPlainTextEdit(page);
        PasswordText->setObjectName("PasswordText");
        PasswordText->setGeometry(QRect(390, 80, 221, 41));
        BrightnessBar = new QScrollBar(page);
        BrightnessBar->setObjectName("BrightnessBar");
        BrightnessBar->setGeometry(QRect(460, 140, 261, 31));
        BrightnessBar->setOrientation(Qt::Orientation::Horizontal);
        label = new QLabel(page);
        label->setObjectName("label");
        label->setGeometry(QRect(390, 140, 51, 31));
        pushButton = new QPushButton(page);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(630, 30, 91, 91));
        LogText = new QPlainTextEdit(page);
        LogText->setObjectName("LogText");
        LogText->setGeometry(QRect(10, 310, 821, 171));
        LogText->setReadOnly(true);
        label_2 = new QLabel(page);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 270, 111, 31));
        textEdit = new QTextEdit(page);
        textEdit->setObjectName("textEdit");
        textEdit->setGeometry(QRect(390, 220, 221, 41));
        textEdit->setReadOnly(true);
        label_16 = new QLabel(page);
        label_16->setObjectName("label_16");
        label_16->setGeometry(QRect(390, 180, 51, 31));
        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        label_3 = new QLabel(page_2);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(130, 10, 53, 15));
        label_4 = new QLabel(page_2);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(240, 10, 53, 15));
        scrollArea = new QScrollArea(page_2);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setGeometry(QRect(0, 30, 641, 441));
        scrollArea->setMinimumSize(QSize(641, 441));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
        scrollArea->setWidgetResizable(false);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 639, 1221));
        scrollAreaWidgetContents->setMinimumSize(QSize(0, 1000));
        plainTextEdit_3 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_3->setObjectName("plainTextEdit_3");
        plainTextEdit_3->setGeometry(QRect(220, 40, 81, 31));
        plainTextEdit_17 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_17->setObjectName("plainTextEdit_17");
        plainTextEdit_17->setGeometry(QRect(220, 320, 81, 31));
        plainTextEdit_15 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_15->setObjectName("plainTextEdit_15");
        plainTextEdit_15->setGeometry(QRect(220, 280, 81, 31));
        label_5 = new QLabel(scrollAreaWidgetContents);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 10, 91, 16));
        plainTextEdit_2 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_2->setObjectName("plainTextEdit_2");
        plainTextEdit_2->setGeometry(QRect(220, 0, 81, 31));
        label_10 = new QLabel(scrollAreaWidgetContents);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(10, 210, 91, 16));
        label_13 = new QLabel(scrollAreaWidgetContents);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(10, 330, 91, 16));
        label_6 = new QLabel(scrollAreaWidgetContents);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(10, 50, 91, 16));
        label_15 = new QLabel(scrollAreaWidgetContents);
        label_15->setObjectName("label_15");
        label_15->setGeometry(QRect(10, 410, 91, 16));
        plainTextEdit_7 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_7->setObjectName("plainTextEdit_7");
        plainTextEdit_7->setGeometry(QRect(220, 120, 81, 31));
        plainTextEdit_18 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_18->setObjectName("plainTextEdit_18");
        plainTextEdit_18->setGeometry(QRect(110, 320, 81, 31));
        plainTextEdit_21 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_21->setObjectName("plainTextEdit_21");
        plainTextEdit_21->setGeometry(QRect(220, 400, 81, 31));
        plainTextEdit_19 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_19->setObjectName("plainTextEdit_19");
        plainTextEdit_19->setGeometry(QRect(220, 360, 81, 31));
        label_7 = new QLabel(scrollAreaWidgetContents);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(10, 90, 91, 16));
        plainTextEdit_13 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_13->setObjectName("plainTextEdit_13");
        plainTextEdit_13->setGeometry(QRect(220, 240, 81, 31));
        plainTextEdit_16 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_16->setObjectName("plainTextEdit_16");
        plainTextEdit_16->setGeometry(QRect(110, 280, 81, 31));
        plainTextEdit_20 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_20->setObjectName("plainTextEdit_20");
        plainTextEdit_20->setGeometry(QRect(110, 360, 81, 31));
        plainTextEdit_9 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_9->setObjectName("plainTextEdit_9");
        plainTextEdit_9->setGeometry(QRect(220, 160, 81, 31));
        label_11 = new QLabel(scrollAreaWidgetContents);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(10, 250, 91, 16));
        plainTextEdit_8 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_8->setObjectName("plainTextEdit_8");
        plainTextEdit_8->setGeometry(QRect(110, 120, 81, 31));
        label_9 = new QLabel(scrollAreaWidgetContents);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(10, 170, 91, 16));
        plainTextEdit_4 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_4->setObjectName("plainTextEdit_4");
        plainTextEdit_4->setGeometry(QRect(110, 40, 81, 31));
        plainTextEdit_6 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_6->setObjectName("plainTextEdit_6");
        plainTextEdit_6->setGeometry(QRect(110, 80, 81, 31));
        label_14 = new QLabel(scrollAreaWidgetContents);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(10, 370, 91, 16));
        plainTextEdit_10 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_10->setObjectName("plainTextEdit_10");
        plainTextEdit_10->setGeometry(QRect(110, 160, 81, 31));
        plainTextEdit_11 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_11->setObjectName("plainTextEdit_11");
        plainTextEdit_11->setGeometry(QRect(220, 200, 81, 31));
        label_8 = new QLabel(scrollAreaWidgetContents);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(10, 130, 91, 16));
        plainTextEdit = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit->setObjectName("plainTextEdit");
        plainTextEdit->setGeometry(QRect(110, 0, 81, 31));
        plainTextEdit_12 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_12->setObjectName("plainTextEdit_12");
        plainTextEdit_12->setGeometry(QRect(110, 200, 81, 31));
        plainTextEdit_14 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_14->setObjectName("plainTextEdit_14");
        plainTextEdit_14->setGeometry(QRect(110, 240, 81, 31));
        plainTextEdit_5 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_5->setObjectName("plainTextEdit_5");
        plainTextEdit_5->setGeometry(QRect(220, 80, 81, 31));
        label_12 = new QLabel(scrollAreaWidgetContents);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(10, 290, 91, 16));
        plainTextEdit_22 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_22->setObjectName("plainTextEdit_22");
        plainTextEdit_22->setGeometry(QRect(110, 400, 81, 31));
        plainTextEdit_45 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_45->setObjectName("plainTextEdit_45");
        plainTextEdit_45->setGeometry(QRect(110, 560, 81, 31));
        label_30 = new QLabel(scrollAreaWidgetContents);
        label_30->setObjectName("label_30");
        label_30->setGeometry(QRect(10, 570, 91, 16));
        plainTextEdit_46 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_46->setObjectName("plainTextEdit_46");
        plainTextEdit_46->setGeometry(QRect(110, 480, 81, 31));
        label_31 = new QLabel(scrollAreaWidgetContents);
        label_31->setObjectName("label_31");
        label_31->setGeometry(QRect(10, 450, 91, 16));
        plainTextEdit_47 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_47->setObjectName("plainTextEdit_47");
        plainTextEdit_47->setGeometry(QRect(220, 560, 81, 31));
        plainTextEdit_48 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_48->setObjectName("plainTextEdit_48");
        plainTextEdit_48->setGeometry(QRect(220, 440, 81, 31));
        plainTextEdit_49 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_49->setObjectName("plainTextEdit_49");
        plainTextEdit_49->setGeometry(QRect(220, 480, 81, 31));
        label_32 = new QLabel(scrollAreaWidgetContents);
        label_32->setObjectName("label_32");
        label_32->setGeometry(QRect(10, 490, 91, 16));
        plainTextEdit_50 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_50->setObjectName("plainTextEdit_50");
        plainTextEdit_50->setGeometry(QRect(110, 440, 81, 31));
        plainTextEdit_51 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_51->setObjectName("plainTextEdit_51");
        plainTextEdit_51->setGeometry(QRect(220, 520, 81, 31));
        label_33 = new QLabel(scrollAreaWidgetContents);
        label_33->setObjectName("label_33");
        label_33->setGeometry(QRect(10, 530, 91, 16));
        plainTextEdit_52 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_52->setObjectName("plainTextEdit_52");
        plainTextEdit_52->setGeometry(QRect(110, 520, 81, 31));
        plainTextEdit_53 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_53->setObjectName("plainTextEdit_53");
        plainTextEdit_53->setGeometry(QRect(110, 720, 81, 31));
        label_34 = new QLabel(scrollAreaWidgetContents);
        label_34->setObjectName("label_34");
        label_34->setGeometry(QRect(10, 730, 91, 16));
        plainTextEdit_54 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_54->setObjectName("plainTextEdit_54");
        plainTextEdit_54->setGeometry(QRect(110, 640, 81, 31));
        label_35 = new QLabel(scrollAreaWidgetContents);
        label_35->setObjectName("label_35");
        label_35->setGeometry(QRect(10, 610, 91, 16));
        plainTextEdit_55 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_55->setObjectName("plainTextEdit_55");
        plainTextEdit_55->setGeometry(QRect(220, 720, 81, 31));
        plainTextEdit_56 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_56->setObjectName("plainTextEdit_56");
        plainTextEdit_56->setGeometry(QRect(220, 600, 81, 31));
        plainTextEdit_57 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_57->setObjectName("plainTextEdit_57");
        plainTextEdit_57->setGeometry(QRect(220, 640, 81, 31));
        label_36 = new QLabel(scrollAreaWidgetContents);
        label_36->setObjectName("label_36");
        label_36->setGeometry(QRect(10, 650, 91, 16));
        plainTextEdit_58 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_58->setObjectName("plainTextEdit_58");
        plainTextEdit_58->setGeometry(QRect(110, 600, 81, 31));
        plainTextEdit_59 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_59->setObjectName("plainTextEdit_59");
        plainTextEdit_59->setGeometry(QRect(220, 680, 81, 31));
        label_37 = new QLabel(scrollAreaWidgetContents);
        label_37->setObjectName("label_37");
        label_37->setGeometry(QRect(10, 690, 91, 16));
        plainTextEdit_60 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_60->setObjectName("plainTextEdit_60");
        plainTextEdit_60->setGeometry(QRect(110, 680, 81, 31));
        plainTextEdit_61 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_61->setObjectName("plainTextEdit_61");
        plainTextEdit_61->setGeometry(QRect(110, 880, 81, 31));
        label_38 = new QLabel(scrollAreaWidgetContents);
        label_38->setObjectName("label_38");
        label_38->setGeometry(QRect(10, 890, 91, 16));
        plainTextEdit_62 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_62->setObjectName("plainTextEdit_62");
        plainTextEdit_62->setGeometry(QRect(110, 800, 81, 31));
        label_39 = new QLabel(scrollAreaWidgetContents);
        label_39->setObjectName("label_39");
        label_39->setGeometry(QRect(10, 770, 91, 16));
        plainTextEdit_63 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_63->setObjectName("plainTextEdit_63");
        plainTextEdit_63->setGeometry(QRect(220, 880, 81, 31));
        plainTextEdit_64 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_64->setObjectName("plainTextEdit_64");
        plainTextEdit_64->setGeometry(QRect(220, 760, 81, 31));
        plainTextEdit_65 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_65->setObjectName("plainTextEdit_65");
        plainTextEdit_65->setGeometry(QRect(220, 800, 81, 31));
        label_40 = new QLabel(scrollAreaWidgetContents);
        label_40->setObjectName("label_40");
        label_40->setGeometry(QRect(10, 810, 91, 16));
        plainTextEdit_66 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_66->setObjectName("plainTextEdit_66");
        plainTextEdit_66->setGeometry(QRect(110, 760, 81, 31));
        plainTextEdit_67 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_67->setObjectName("plainTextEdit_67");
        plainTextEdit_67->setGeometry(QRect(220, 840, 81, 31));
        label_41 = new QLabel(scrollAreaWidgetContents);
        label_41->setObjectName("label_41");
        label_41->setGeometry(QRect(10, 850, 91, 16));
        plainTextEdit_68 = new QPlainTextEdit(scrollAreaWidgetContents);
        plainTextEdit_68->setObjectName("plainTextEdit_68");
        plainTextEdit_68->setGeometry(QRect(110, 840, 81, 31));
        scrollArea->setWidget(scrollAreaWidgetContents);
        StartCalibrationButton = new QPushButton(page_2);
        StartCalibrationButton->setObjectName("StartCalibrationButton");
        StartCalibrationButton->setGeometry(QRect(660, 30, 131, 41));
        CalibrationModeComboBox = new QComboBox(page_2);
        CalibrationModeComboBox->addItem(QString());
        CalibrationModeComboBox->addItem(QString());
        CalibrationModeComboBox->setObjectName("CalibrationModeComboBox");
        CalibrationModeComboBox->setGeometry(QRect(660, 110, 131, 31));
        ResetMaxButton = new QPushButton(page_2);
        ResetMaxButton->setObjectName("ResetMaxButton");
        ResetMaxButton->setGeometry(QRect(660, 430, 131, 41));
        ResetMinButton = new QPushButton(page_2);
        ResetMinButton->setObjectName("ResetMinButton");
        ResetMinButton->setGeometry(QRect(660, 380, 131, 41));
        label_29 = new QLabel(page_2);
        label_29->setObjectName("label_29");
        label_29->setGeometry(QRect(660, 90, 91, 16));
        stackedWidget->addWidget(page_2);
        MainPageButton = new QPushButton(PaperTrackerMainWindow);
        MainPageButton->setObjectName("MainPageButton");
        MainPageButton->setGeometry(QRect(10, 10, 75, 23));
        CalibrationPageButton = new QPushButton(PaperTrackerMainWindow);
        CalibrationPageButton->setObjectName("CalibrationPageButton");
        CalibrationPageButton->setGeometry(QRect(110, 10, 75, 23));

        retranslateUi(PaperTrackerMainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(PaperTrackerMainWindow);
    } // setupUi

    void retranslateUi(QWidget *PaperTrackerMainWindow)
    {
        PaperTrackerMainWindow->setWindowTitle(QCoreApplication::translate("PaperTrackerMainWindow", "PaperTrack", nullptr));
        ImageLabel->setText(QString());
        SSIDText->setPlainText(QCoreApplication::translate("PaperTrackerMainWindow", "\350\257\267\350\276\223\345\205\245SSID", nullptr));
        PasswordText->setPlainText(QCoreApplication::translate("PaperTrackerMainWindow", "\350\257\267\350\276\223\345\205\245\345\257\206\347\240\201", nullptr));
        label->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\344\272\256\345\272\246\350\260\203\346\225\264", nullptr));
        pushButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\345\217\221\351\200\201", nullptr));
        label_2->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\227\245\345\277\227\347\252\227\345\217\243\357\274\232", nullptr));
        label_16->setText(QCoreApplication::translate("PaperTrackerMainWindow", "IP\345\234\260\345\235\200\357\274\232", nullptr));
        label_3->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\234\200\345\260\217\345\200\274", nullptr));
        label_4->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\234\200\345\244\247\345\200\274", nullptr));
        label_5->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_10->setText(QCoreApplication::translate("PaperTrackerMainWindow", "jawRight", nullptr));
        label_13->setText(QCoreApplication::translate("PaperTrackerMainWindow", "mouthFunnel", nullptr));
        label_6->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffRight", nullptr));
        label_15->setText(QCoreApplication::translate("PaperTrackerMainWindow", "mouthLeft", nullptr));
        label_7->setText(QCoreApplication::translate("PaperTrackerMainWindow", "jawOpen", nullptr));
        label_11->setText(QCoreApplication::translate("PaperTrackerMainWindow", "noseSneefLeft", nullptr));
        label_9->setText(QCoreApplication::translate("PaperTrackerMainWindow", "jawLeft", nullptr));
        label_14->setText(QCoreApplication::translate("PaperTrackerMainWindow", "mouthPucker", nullptr));
        label_8->setText(QCoreApplication::translate("PaperTrackerMainWindow", "jawForward", nullptr));
        label_12->setText(QCoreApplication::translate("PaperTrackerMainWindow", "noseSneefRight", nullptr));
        label_30->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_31->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_32->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_33->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_34->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_35->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_36->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_37->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_38->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_39->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_40->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_41->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        StartCalibrationButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\345\274\200\345\247\213\346\240\207\345\256\232", nullptr));
        CalibrationModeComboBox->setItemText(0, QCoreApplication::translate("PaperTrackerMainWindow", "Neutral\346\250\241\345\274\217", nullptr));
        CalibrationModeComboBox->setItemText(1, QCoreApplication::translate("PaperTrackerMainWindow", "Full\346\250\241\345\274\217", nullptr));

        ResetMaxButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\351\207\215\347\275\256\346\234\200\345\244\247\345\200\274", nullptr));
        ResetMinButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\351\207\215\347\275\256\346\234\200\345\260\217\345\200\274", nullptr));
        label_29->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\240\207\345\256\232\346\250\241\345\274\217\351\200\211\346\213\251", nullptr));
        MainPageButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\344\270\273\351\241\265\351\235\242", nullptr));
        CalibrationPageButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\240\207\345\256\232\351\241\265\351\235\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PaperTrackerMainWindow: public Ui_PaperTrackerMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H
