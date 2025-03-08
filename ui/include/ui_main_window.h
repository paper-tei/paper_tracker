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
#include <QtWidgets/QCheckBox>
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
    QPushButton *wifi_send_Button;
    QPushButton *FlashFirmwareButton;
    QPlainTextEdit *LogText;
    QLabel *label_2;
    QTextEdit *textEdit;
    QLabel *label_16;
    QPushButton *restart_Button;
    QLabel *SerialConnectLabel;
    QLabel *WifiConnectLabel;
    QLabel *label_17;
    QScrollBar *RotateImageBar;
    QCheckBox *UseFilterBox;
    QComboBox *EnergyModeBox;
    QLabel *label_18;
    QWidget *page_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QLabel *label_5;
    QLabel *label_10;
    QLabel *label_13;
    QLabel *label_6;
    QLabel *label_15;
    QLabel *label_7;
    QLabel *label_11;
    QLabel *label_9;
    QLabel *label_14;
    QLabel *label_8;
    QLabel *label_12;
    QLabel *label_31;
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
    QScrollBar *CheekPuffLeftBar;
    QScrollBar *CheekPuffRightBar;
    QScrollBar *JawOpenBar;
    QScrollBar *JawLeftBar;
    QScrollBar *MouthLeftBar;
    QScrollBar *JawRightBar;
    QScrollBar *TongueOutBar;
    QScrollBar *MouthRightBar;
    QScrollBar *TongueDownBar;
    QScrollBar *TongueUpBar;
    QScrollBar *TongueRightBar;
    QScrollBar *TongueLeftBar;
    QPushButton *SaveParamConfigButton;
    QLabel *ImageLabelCal;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_19;
    QPushButton *CheckFirmwareVersionButton;
    QPushButton *CheckClientVersionButton;
    QPushButton *MainPageButton;
    QPushButton *CalibrationPageButton;

    void setupUi(QWidget *PaperTrackerMainWindow)
    {
        if (PaperTrackerMainWindow->objectName().isEmpty())
            PaperTrackerMainWindow->setObjectName("PaperTrackerMainWindow");
        PaperTrackerMainWindow->resize(835, 553);
        PaperTrackerMainWindow->setStyleSheet(QString::fromUtf8(""));
        stackedWidget = new QStackedWidget(PaperTrackerMainWindow);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(0, 50, 841, 491));
        page = new QWidget();
        page->setObjectName("page");
        ImageLabel = new QLabel(page);
        ImageLabel->setObjectName("ImageLabel");
        ImageLabel->setGeometry(QRect(10, 10, 280, 280));
        ImageLabel->setTextInteractionFlags(Qt::TextInteractionFlag::NoTextInteraction);
        SSIDText = new QPlainTextEdit(page);
        SSIDText->setObjectName("SSIDText");
        SSIDText->setGeometry(QRect(310, 10, 221, 41));
        PasswordText = new QPlainTextEdit(page);
        PasswordText->setObjectName("PasswordText");
        PasswordText->setGeometry(QRect(310, 60, 221, 41));
        BrightnessBar = new QScrollBar(page);
        BrightnessBar->setObjectName("BrightnessBar");
        BrightnessBar->setGeometry(QRect(370, 110, 261, 20));
        BrightnessBar->setOrientation(Qt::Orientation::Horizontal);
        label = new QLabel(page);
        label->setObjectName("label");
        label->setGeometry(QRect(310, 110, 51, 21));
        wifi_send_Button = new QPushButton(page);
        wifi_send_Button->setObjectName("wifi_send_Button");
        wifi_send_Button->setGeometry(QRect(550, 10, 91, 91));
        FlashFirmwareButton = new QPushButton(page);
        FlashFirmwareButton->setObjectName("FlashFirmwareButton");
        FlashFirmwareButton->setGeometry(QRect(650, 10, 91, 41));
        LogText = new QPlainTextEdit(page);
        LogText->setObjectName("LogText");
        LogText->setGeometry(QRect(10, 310, 821, 171));
        LogText->setReadOnly(true);
        label_2 = new QLabel(page);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 291, 111, 20));
        textEdit = new QTextEdit(page);
        textEdit->setObjectName("textEdit");
        textEdit->setGeometry(QRect(310, 240, 221, 41));
        textEdit->setReadOnly(true);
        label_16 = new QLabel(page);
        label_16->setObjectName("label_16");
        label_16->setGeometry(QRect(310, 210, 51, 31));
        restart_Button = new QPushButton(page);
        restart_Button->setObjectName("restart_Button");
        restart_Button->setGeometry(QRect(650, 60, 91, 41));
        SerialConnectLabel = new QLabel(page);
        SerialConnectLabel->setObjectName("SerialConnectLabel");
        SerialConnectLabel->setGeometry(QRect(660, 210, 71, 31));
        SerialConnectLabel->setTextFormat(Qt::TextFormat::AutoText);
        WifiConnectLabel = new QLabel(page);
        WifiConnectLabel->setObjectName("WifiConnectLabel");
        WifiConnectLabel->setGeometry(QRect(660, 250, 71, 31));
        label_17 = new QLabel(page);
        label_17->setObjectName("label_17");
        label_17->setGeometry(QRect(310, 140, 81, 21));
        RotateImageBar = new QScrollBar(page);
        RotateImageBar->setObjectName("RotateImageBar");
        RotateImageBar->setGeometry(QRect(390, 140, 341, 20));
        RotateImageBar->setMaximum(1080);
        RotateImageBar->setPageStep(40);
        RotateImageBar->setOrientation(Qt::Orientation::Horizontal);
        UseFilterBox = new QCheckBox(page);
        UseFilterBox->setObjectName("UseFilterBox");
        UseFilterBox->setGeometry(QRect(500, 170, 81, 21));
        EnergyModeBox = new QComboBox(page);
        EnergyModeBox->addItem(QString());
        EnergyModeBox->addItem(QString());
        EnergyModeBox->addItem(QString());
        EnergyModeBox->setObjectName("EnergyModeBox");
        EnergyModeBox->setGeometry(QRect(400, 170, 80, 23));
        EnergyModeBox->setDuplicatesEnabled(false);
        label_18 = new QLabel(page);
        label_18->setObjectName("label_18");
        label_18->setGeometry(QRect(310, 170, 81, 21));
        stackedWidget->addWidget(page);
        SSIDText->raise();
        PasswordText->raise();
        BrightnessBar->raise();
        label->raise();
        wifi_send_Button->raise();
        FlashFirmwareButton->raise();
        LogText->raise();
        label_2->raise();
        textEdit->raise();
        label_16->raise();
        restart_Button->raise();
        SerialConnectLabel->raise();
        WifiConnectLabel->raise();
        label_17->raise();
        RotateImageBar->raise();
        UseFilterBox->raise();
        EnergyModeBox->raise();
        label_18->raise();
        ImageLabel->raise();
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        scrollArea = new QScrollArea(page_2);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setGeometry(QRect(0, 30, 500, 441));
        scrollArea->setMinimumSize(QSize(500, 441));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
        scrollArea->setWidgetResizable(false);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 639, 500));
        scrollAreaWidgetContents->setMinimumSize(QSize(0, 500));
        label_5 = new QLabel(scrollAreaWidgetContents);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 10, 91, 16));
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
        label_7 = new QLabel(scrollAreaWidgetContents);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(10, 90, 91, 16));
        label_11 = new QLabel(scrollAreaWidgetContents);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(10, 250, 91, 16));
        label_9 = new QLabel(scrollAreaWidgetContents);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(10, 170, 91, 16));
        label_14 = new QLabel(scrollAreaWidgetContents);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(10, 370, 91, 16));
        label_8 = new QLabel(scrollAreaWidgetContents);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(10, 130, 91, 16));
        label_12 = new QLabel(scrollAreaWidgetContents);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(10, 290, 91, 16));
        label_31 = new QLabel(scrollAreaWidgetContents);
        label_31->setObjectName("label_31");
        label_31->setGeometry(QRect(10, 450, 91, 16));
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
        CheekPuffLeftBar = new QScrollBar(scrollAreaWidgetContents);
        CheekPuffLeftBar->setObjectName("CheekPuffLeftBar");
        CheekPuffLeftBar->setGeometry(QRect(120, 10, 201, 16));
        CheekPuffLeftBar->setOrientation(Qt::Orientation::Horizontal);
        CheekPuffRightBar = new QScrollBar(scrollAreaWidgetContents);
        CheekPuffRightBar->setObjectName("CheekPuffRightBar");
        CheekPuffRightBar->setGeometry(QRect(120, 50, 201, 16));
        CheekPuffRightBar->setOrientation(Qt::Orientation::Horizontal);
        JawOpenBar = new QScrollBar(scrollAreaWidgetContents);
        JawOpenBar->setObjectName("JawOpenBar");
        JawOpenBar->setGeometry(QRect(120, 90, 201, 16));
        JawOpenBar->setOrientation(Qt::Orientation::Horizontal);
        JawLeftBar = new QScrollBar(scrollAreaWidgetContents);
        JawLeftBar->setObjectName("JawLeftBar");
        JawLeftBar->setGeometry(QRect(120, 130, 201, 16));
        JawLeftBar->setOrientation(Qt::Orientation::Horizontal);
        MouthLeftBar = new QScrollBar(scrollAreaWidgetContents);
        MouthLeftBar->setObjectName("MouthLeftBar");
        MouthLeftBar->setGeometry(QRect(120, 210, 201, 16));
        MouthLeftBar->setOrientation(Qt::Orientation::Horizontal);
        JawRightBar = new QScrollBar(scrollAreaWidgetContents);
        JawRightBar->setObjectName("JawRightBar");
        JawRightBar->setGeometry(QRect(120, 170, 201, 16));
        JawRightBar->setOrientation(Qt::Orientation::Horizontal);
        TongueOutBar = new QScrollBar(scrollAreaWidgetContents);
        TongueOutBar->setObjectName("TongueOutBar");
        TongueOutBar->setGeometry(QRect(120, 290, 201, 16));
        TongueOutBar->setOrientation(Qt::Orientation::Horizontal);
        MouthRightBar = new QScrollBar(scrollAreaWidgetContents);
        MouthRightBar->setObjectName("MouthRightBar");
        MouthRightBar->setGeometry(QRect(120, 250, 201, 16));
        MouthRightBar->setOrientation(Qt::Orientation::Horizontal);
        TongueDownBar = new QScrollBar(scrollAreaWidgetContents);
        TongueDownBar->setObjectName("TongueDownBar");
        TongueDownBar->setGeometry(QRect(120, 370, 201, 16));
        TongueDownBar->setOrientation(Qt::Orientation::Horizontal);
        TongueUpBar = new QScrollBar(scrollAreaWidgetContents);
        TongueUpBar->setObjectName("TongueUpBar");
        TongueUpBar->setGeometry(QRect(120, 330, 201, 16));
        TongueUpBar->setOrientation(Qt::Orientation::Horizontal);
        TongueRightBar = new QScrollBar(scrollAreaWidgetContents);
        TongueRightBar->setObjectName("TongueRightBar");
        TongueRightBar->setGeometry(QRect(120, 450, 201, 16));
        TongueRightBar->setOrientation(Qt::Orientation::Horizontal);
        TongueLeftBar = new QScrollBar(scrollAreaWidgetContents);
        TongueLeftBar->setObjectName("TongueLeftBar");
        TongueLeftBar->setGeometry(QRect(120, 410, 201, 16));
        TongueLeftBar->setOrientation(Qt::Orientation::Horizontal);
        scrollArea->setWidget(scrollAreaWidgetContents);
        SaveParamConfigButton = new QPushButton(page_2);
        SaveParamConfigButton->setObjectName("SaveParamConfigButton");
        SaveParamConfigButton->setGeometry(QRect(510, 30, 131, 41));
        ImageLabelCal = new QLabel(page_2);
        ImageLabelCal->setObjectName("ImageLabelCal");
        ImageLabelCal->setGeometry(QRect(510, 190, 280, 280));
        label_3 = new QLabel(page_2);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(200, 10, 61, 16));
        label_4 = new QLabel(page_2);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(310, 10, 61, 16));
        label_19 = new QLabel(page_2);
        label_19->setObjectName("label_19");
        label_19->setGeometry(QRect(130, 10, 61, 16));
        CheckFirmwareVersionButton = new QPushButton(page_2);
        CheckFirmwareVersionButton->setObjectName("CheckFirmwareVersionButton");
        CheckFirmwareVersionButton->setGeometry(QRect(510, 80, 131, 41));
        CheckClientVersionButton = new QPushButton(page_2);
        CheckClientVersionButton->setObjectName("CheckClientVersionButton");
        CheckClientVersionButton->setGeometry(QRect(510, 130, 131, 41));
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
        ImageLabel->setText(QCoreApplication::translate("PaperTrackerMainWindow", "                         \346\262\241\346\234\211\345\233\276\345\203\217\350\276\223\345\205\245", nullptr));
        SSIDText->setPlainText(QCoreApplication::translate("PaperTrackerMainWindow", "\350\257\267\350\276\223\345\205\245WIFI\345\220\215\345\255\227\357\274\210\344\273\205\346\224\257\346\214\2012.4ghz\357\274\211", nullptr));
        PasswordText->setPlainText(QCoreApplication::translate("PaperTrackerMainWindow", "\350\257\267\350\276\223\345\205\245WIFI\345\257\206\347\240\201", nullptr));
        label->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\344\272\256\345\272\246\350\260\203\346\225\264", nullptr));
        wifi_send_Button->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\345\217\221\351\200\201", nullptr));
        FlashFirmwareButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\345\210\267\345\206\231\345\233\272\344\273\266", nullptr));
        label_2->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\227\245\345\277\227\347\252\227\345\217\243\357\274\232", nullptr));
        label_16->setText(QCoreApplication::translate("PaperTrackerMainWindow", "IP\345\234\260\345\235\200\357\274\232", nullptr));
        restart_Button->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\351\207\215\345\220\257", nullptr));
        SerialConnectLabel->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\344\270\262\345\217\243\346\234\252\350\277\236\346\216\245", nullptr));
        WifiConnectLabel->setText(QCoreApplication::translate("PaperTrackerMainWindow", "wifi\346\234\252\350\277\236\346\216\245", nullptr));
        label_17->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\227\213\350\275\254\350\247\222\345\272\246\350\260\203\346\225\264", nullptr));
        UseFilterBox->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\345\220\257\347\224\250\346\273\244\346\263\242", nullptr));
        EnergyModeBox->setItemText(0, QCoreApplication::translate("PaperTrackerMainWindow", "\346\231\256\351\200\232\346\250\241\345\274\217", nullptr));
        EnergyModeBox->setItemText(1, QCoreApplication::translate("PaperTrackerMainWindow", "\350\212\202\350\203\275\346\250\241\345\274\217", nullptr));
        EnergyModeBox->setItemText(2, QCoreApplication::translate("PaperTrackerMainWindow", "\346\200\247\350\203\275\346\250\241\345\274\217", nullptr));

        label_18->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\200\247\350\203\275\346\250\241\345\274\217\351\200\211\346\213\251", nullptr));
        label_5->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\345\267\246\350\204\270\351\242\212", nullptr));
        label_10->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\345\230\264\345\267\246\347\247\273", nullptr));
        label_13->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\350\210\214\345\244\264\345\220\221\344\270\212", nullptr));
        label_6->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\345\217\263\350\204\270\351\242\212", nullptr));
        label_15->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\350\210\214\345\244\264\345\220\221\345\267\246", nullptr));
        label_7->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\345\274\240\345\230\264", nullptr));
        label_11->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\345\230\264\345\217\263\347\247\273", nullptr));
        label_9->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\344\270\213\345\267\264\345\217\263\347\247\273", nullptr));
        label_14->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\350\210\214\345\244\264\345\220\221\344\270\213", nullptr));
        label_8->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\344\270\213\345\267\264\345\267\246\347\247\273", nullptr));
        label_12->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\350\210\214\345\244\264\344\274\270\345\207\272", nullptr));
        label_31->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\350\210\214\345\244\264\345\220\221\345\217\263", nullptr));
        SaveParamConfigButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\344\277\235\345\255\230\345\217\202\346\225\260", nullptr));
        ImageLabelCal->setText(QString());
        label_3->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\224\276\345\244\247\345\200\215\347\216\207", nullptr));
        label_4->setText(QCoreApplication::translate("PaperTrackerMainWindow", "x3", nullptr));
        label_19->setText(QCoreApplication::translate("PaperTrackerMainWindow", "x1", nullptr));
        CheckFirmwareVersionButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\243\200\346\237\245\345\233\272\344\273\266\346\233\264\346\226\260", nullptr));
        CheckClientVersionButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\243\200\346\237\245\345\256\242\346\210\267\347\253\257\346\233\264\346\226\260", nullptr));
        MainPageButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\344\270\273\351\241\265\351\235\242", nullptr));
        CalibrationPageButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\240\207\345\256\232\351\241\265\351\235\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PaperTrackerMainWindow: public Ui_PaperTrackerMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H
