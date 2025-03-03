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
#include <QtWidgets/QProgressBar>
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
    QPushButton *FlashFirmwareButton;
    QPlainTextEdit *LogText;
    QLabel *label_2;
    QTextEdit *textEdit;
    QLabel *label_16;
    QWidget *page_2;
    QLabel *label_3;
    QLabel *label_4;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QPlainTextEdit *CheekPullRightMax;
    QPlainTextEdit *TongueUpMax;
    QPlainTextEdit *TongueOutMax;
    QLabel *label_5;
    QPlainTextEdit *CheekPullLeftMax;
    QLabel *label_10;
    QLabel *label_13;
    QLabel *label_6;
    QLabel *label_15;
    QPlainTextEdit *JawLeftMax;
    QPlainTextEdit *TongueUpMin;
    QPlainTextEdit *TongueLeftMax;
    QPlainTextEdit *TongueDownMax;
    QLabel *label_7;
    QPlainTextEdit *MouthRightMax;
    QPlainTextEdit *TongueOutMin;
    QPlainTextEdit *TongueDownMin;
    QPlainTextEdit *JawRightMax;
    QLabel *label_11;
    QPlainTextEdit *JawLeftMin;
    QLabel *label_9;
    QPlainTextEdit *CheekPullRightMin;
    QPlainTextEdit *JawOpenMin;
    QLabel *label_14;
    QPlainTextEdit *JawRightMin;
    QPlainTextEdit *MouthLeftMax;
    QLabel *label_8;
    QPlainTextEdit *CheekPullLeftMin;
    QPlainTextEdit *MouthLeftMin;
    QPlainTextEdit *MouthRightMin;
    QPlainTextEdit *JawOpenMax;
    QLabel *label_12;
    QPlainTextEdit *TongueLeftMin;
    QLabel *label_31;
    QPlainTextEdit *TongueRightMax;
    QPlainTextEdit *TongueRightMin;
    QProgressBar *CheekPullLeftValue;
    QProgressBar *CheekPullRightValue;
    QProgressBar *JawLeftValue;
    QProgressBar *JawOpenValue;
    QProgressBar *MouthLeftValue;
    QProgressBar *JawRightValue;
    QProgressBar *TongueOutValue;
    QProgressBar *MouthRightValue;
    QProgressBar *TongueDownValue;
    QProgressBar *TongueUpValue;
    QProgressBar *TongueRightValue;
    QProgressBar *TongueLeftValue;
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
        FlashFirmwareButton = new QPushButton(page);
        FlashFirmwareButton->setObjectName("FlashFirmwareButton");
        FlashFirmwareButton->setGeometry(QRect(730, 30, 91, 31));
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
        scrollAreaWidgetContents->setGeometry(QRect(0, -73, 639, 500));
        scrollAreaWidgetContents->setMinimumSize(QSize(0, 500));
        CheekPullRightMax = new QPlainTextEdit(scrollAreaWidgetContents);
        CheekPullRightMax->setObjectName("CheekPullRightMax");
        CheekPullRightMax->setGeometry(QRect(220, 40, 81, 31));
        TongueUpMax = new QPlainTextEdit(scrollAreaWidgetContents);
        TongueUpMax->setObjectName("TongueUpMax");
        TongueUpMax->setGeometry(QRect(220, 320, 81, 31));
        TongueOutMax = new QPlainTextEdit(scrollAreaWidgetContents);
        TongueOutMax->setObjectName("TongueOutMax");
        TongueOutMax->setGeometry(QRect(220, 280, 81, 31));
        label_5 = new QLabel(scrollAreaWidgetContents);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 10, 91, 16));
        CheekPullLeftMax = new QPlainTextEdit(scrollAreaWidgetContents);
        CheekPullLeftMax->setObjectName("CheekPullLeftMax");
        CheekPullLeftMax->setGeometry(QRect(220, 0, 81, 31));
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
        JawLeftMax = new QPlainTextEdit(scrollAreaWidgetContents);
        JawLeftMax->setObjectName("JawLeftMax");
        JawLeftMax->setGeometry(QRect(220, 120, 81, 31));
        TongueUpMin = new QPlainTextEdit(scrollAreaWidgetContents);
        TongueUpMin->setObjectName("TongueUpMin");
        TongueUpMin->setGeometry(QRect(110, 320, 81, 31));
        TongueLeftMax = new QPlainTextEdit(scrollAreaWidgetContents);
        TongueLeftMax->setObjectName("TongueLeftMax");
        TongueLeftMax->setGeometry(QRect(220, 400, 81, 31));
        TongueDownMax = new QPlainTextEdit(scrollAreaWidgetContents);
        TongueDownMax->setObjectName("TongueDownMax");
        TongueDownMax->setGeometry(QRect(220, 360, 81, 31));
        label_7 = new QLabel(scrollAreaWidgetContents);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(10, 90, 91, 16));
        MouthRightMax = new QPlainTextEdit(scrollAreaWidgetContents);
        MouthRightMax->setObjectName("MouthRightMax");
        MouthRightMax->setGeometry(QRect(220, 240, 81, 31));
        TongueOutMin = new QPlainTextEdit(scrollAreaWidgetContents);
        TongueOutMin->setObjectName("TongueOutMin");
        TongueOutMin->setGeometry(QRect(110, 280, 81, 31));
        TongueDownMin = new QPlainTextEdit(scrollAreaWidgetContents);
        TongueDownMin->setObjectName("TongueDownMin");
        TongueDownMin->setGeometry(QRect(110, 360, 81, 31));
        JawRightMax = new QPlainTextEdit(scrollAreaWidgetContents);
        JawRightMax->setObjectName("JawRightMax");
        JawRightMax->setGeometry(QRect(220, 160, 81, 31));
        label_11 = new QLabel(scrollAreaWidgetContents);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(10, 250, 91, 16));
        JawLeftMin = new QPlainTextEdit(scrollAreaWidgetContents);
        JawLeftMin->setObjectName("JawLeftMin");
        JawLeftMin->setGeometry(QRect(110, 120, 81, 31));
        label_9 = new QLabel(scrollAreaWidgetContents);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(10, 170, 91, 16));
        CheekPullRightMin = new QPlainTextEdit(scrollAreaWidgetContents);
        CheekPullRightMin->setObjectName("CheekPullRightMin");
        CheekPullRightMin->setGeometry(QRect(110, 40, 81, 31));
        JawOpenMin = new QPlainTextEdit(scrollAreaWidgetContents);
        JawOpenMin->setObjectName("JawOpenMin");
        JawOpenMin->setGeometry(QRect(110, 80, 81, 31));
        label_14 = new QLabel(scrollAreaWidgetContents);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(10, 370, 91, 16));
        JawRightMin = new QPlainTextEdit(scrollAreaWidgetContents);
        JawRightMin->setObjectName("JawRightMin");
        JawRightMin->setGeometry(QRect(110, 160, 81, 31));
        MouthLeftMax = new QPlainTextEdit(scrollAreaWidgetContents);
        MouthLeftMax->setObjectName("MouthLeftMax");
        MouthLeftMax->setGeometry(QRect(220, 200, 81, 31));
        label_8 = new QLabel(scrollAreaWidgetContents);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(10, 130, 91, 16));
        CheekPullLeftMin = new QPlainTextEdit(scrollAreaWidgetContents);
        CheekPullLeftMin->setObjectName("CheekPullLeftMin");
        CheekPullLeftMin->setGeometry(QRect(110, 0, 81, 31));
        MouthLeftMin = new QPlainTextEdit(scrollAreaWidgetContents);
        MouthLeftMin->setObjectName("MouthLeftMin");
        MouthLeftMin->setGeometry(QRect(110, 200, 81, 31));
        MouthRightMin = new QPlainTextEdit(scrollAreaWidgetContents);
        MouthRightMin->setObjectName("MouthRightMin");
        MouthRightMin->setGeometry(QRect(110, 240, 81, 31));
        JawOpenMax = new QPlainTextEdit(scrollAreaWidgetContents);
        JawOpenMax->setObjectName("JawOpenMax");
        JawOpenMax->setGeometry(QRect(220, 80, 81, 31));
        label_12 = new QLabel(scrollAreaWidgetContents);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(10, 290, 91, 16));
        TongueLeftMin = new QPlainTextEdit(scrollAreaWidgetContents);
        TongueLeftMin->setObjectName("TongueLeftMin");
        TongueLeftMin->setGeometry(QRect(110, 400, 81, 31));
        label_31 = new QLabel(scrollAreaWidgetContents);
        label_31->setObjectName("label_31");
        label_31->setGeometry(QRect(10, 450, 91, 16));
        TongueRightMax = new QPlainTextEdit(scrollAreaWidgetContents);
        TongueRightMax->setObjectName("TongueRightMax");
        TongueRightMax->setGeometry(QRect(220, 440, 81, 31));
        TongueRightMin = new QPlainTextEdit(scrollAreaWidgetContents);
        TongueRightMin->setObjectName("TongueRightMin");
        TongueRightMin->setGeometry(QRect(110, 440, 81, 31));
        CheekPullLeftValue = new QProgressBar(scrollAreaWidgetContents);
        CheekPullLeftValue->setObjectName("CheekPullLeftValue");
        CheekPullLeftValue->setGeometry(QRect(350, 0, 151, 31));
        CheekPullLeftValue->setValue(24);
        CheekPullRightValue = new QProgressBar(scrollAreaWidgetContents);
        CheekPullRightValue->setObjectName("CheekPullRightValue");
        CheekPullRightValue->setGeometry(QRect(350, 40, 151, 31));
        CheekPullRightValue->setValue(24);
        JawLeftValue = new QProgressBar(scrollAreaWidgetContents);
        JawLeftValue->setObjectName("JawLeftValue");
        JawLeftValue->setGeometry(QRect(350, 120, 151, 31));
        JawLeftValue->setValue(24);
        JawOpenValue = new QProgressBar(scrollAreaWidgetContents);
        JawOpenValue->setObjectName("JawOpenValue");
        JawOpenValue->setGeometry(QRect(350, 80, 151, 31));
        JawOpenValue->setValue(24);
        MouthLeftValue = new QProgressBar(scrollAreaWidgetContents);
        MouthLeftValue->setObjectName("MouthLeftValue");
        MouthLeftValue->setGeometry(QRect(350, 200, 151, 31));
        MouthLeftValue->setValue(24);
        JawRightValue = new QProgressBar(scrollAreaWidgetContents);
        JawRightValue->setObjectName("JawRightValue");
        JawRightValue->setGeometry(QRect(350, 160, 151, 31));
        JawRightValue->setValue(24);
        TongueOutValue = new QProgressBar(scrollAreaWidgetContents);
        TongueOutValue->setObjectName("TongueOutValue");
        TongueOutValue->setGeometry(QRect(350, 280, 151, 31));
        TongueOutValue->setValue(24);
        MouthRightValue = new QProgressBar(scrollAreaWidgetContents);
        MouthRightValue->setObjectName("MouthRightValue");
        MouthRightValue->setGeometry(QRect(350, 240, 151, 31));
        MouthRightValue->setValue(24);
        TongueDownValue = new QProgressBar(scrollAreaWidgetContents);
        TongueDownValue->setObjectName("TongueDownValue");
        TongueDownValue->setGeometry(QRect(350, 360, 151, 31));
        TongueDownValue->setValue(24);
        TongueUpValue = new QProgressBar(scrollAreaWidgetContents);
        TongueUpValue->setObjectName("TongueUpValue");
        TongueUpValue->setGeometry(QRect(350, 320, 151, 31));
        TongueUpValue->setValue(24);
        TongueRightValue = new QProgressBar(scrollAreaWidgetContents);
        TongueRightValue->setObjectName("TongueRightValue");
        TongueRightValue->setGeometry(QRect(350, 440, 151, 31));
        TongueRightValue->setValue(24);
        TongueLeftValue = new QProgressBar(scrollAreaWidgetContents);
        TongueLeftValue->setObjectName("TongueLeftValue");
        TongueLeftValue->setGeometry(QRect(350, 400, 151, 31));
        TongueLeftValue->setValue(24);
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
        FlashFirmwareButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\345\210\267\345\206\231\345\233\272\344\273\266", nullptr));
        label_2->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\227\245\345\277\227\347\252\227\345\217\243\357\274\232", nullptr));
        label_16->setText(QCoreApplication::translate("PaperTrackerMainWindow", "IP\345\234\260\345\235\200\357\274\232", nullptr));
        label_3->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\234\200\345\260\217\345\200\274", nullptr));
        label_4->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\234\200\345\244\247\345\200\274", nullptr));
        label_5->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffLeft", nullptr));
        label_10->setText(QCoreApplication::translate("PaperTrackerMainWindow", "mouthLeft", nullptr));
        label_13->setText(QCoreApplication::translate("PaperTrackerMainWindow", "tongueUp", nullptr));
        label_6->setText(QCoreApplication::translate("PaperTrackerMainWindow", "cheekPuffRight", nullptr));
        label_15->setText(QCoreApplication::translate("PaperTrackerMainWindow", "tongueLeft", nullptr));
        label_7->setText(QCoreApplication::translate("PaperTrackerMainWindow", "jawOpen", nullptr));
        label_11->setText(QCoreApplication::translate("PaperTrackerMainWindow", "mouthRight", nullptr));
        label_9->setText(QCoreApplication::translate("PaperTrackerMainWindow", "jawRight", nullptr));
        label_14->setText(QCoreApplication::translate("PaperTrackerMainWindow", "tongueDown", nullptr));
        label_8->setText(QCoreApplication::translate("PaperTrackerMainWindow", "jawLeft", nullptr));
        label_12->setText(QCoreApplication::translate("PaperTrackerMainWindow", "tongueOut", nullptr));
        label_31->setText(QCoreApplication::translate("PaperTrackerMainWindow", "tongueRight", nullptr));
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
