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
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PaperTrackerMainWindow
{
public:
    QPlainTextEdit *SSIDText;
    QPlainTextEdit *LogText;
    QPlainTextEdit *PasswordText;
    QPushButton *pushButton;
    QScrollBar *BrightnessBar;
    QLabel *ImageLabel;
    QLabel *label;
    QLabel *label_2;

    void setupUi(QWidget *PaperTrackerMainWindow)
    {
        if (PaperTrackerMainWindow->objectName().isEmpty())
            PaperTrackerMainWindow->setObjectName("PaperTrackerMainWindow");
        PaperTrackerMainWindow->resize(848, 538);
        SSIDText = new QPlainTextEdit(PaperTrackerMainWindow);
        SSIDText->setObjectName("SSIDText");
        SSIDText->setGeometry(QRect(470, 30, 221, 41));
        LogText = new QPlainTextEdit(PaperTrackerMainWindow);
        LogText->setObjectName("LogText");
        LogText->setGeometry(QRect(10, 320, 821, 211));
        LogText->setReadOnly(true);
        PasswordText = new QPlainTextEdit(PaperTrackerMainWindow);
        PasswordText->setObjectName("PasswordText");
        PasswordText->setGeometry(QRect(470, 80, 221, 41));
        pushButton = new QPushButton(PaperTrackerMainWindow);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(720, 30, 91, 91));
        BrightnessBar = new QScrollBar(PaperTrackerMainWindow);
        BrightnessBar->setObjectName("BrightnessBar");
        BrightnessBar->setGeometry(QRect(540, 140, 261, 31));
        BrightnessBar->setOrientation(Qt::Orientation::Horizontal);
        ImageLabel = new QLabel(PaperTrackerMainWindow);
        ImageLabel->setObjectName("ImageLabel");
        ImageLabel->setGeometry(QRect(10, 20, 371, 261));
        label = new QLabel(PaperTrackerMainWindow);
        label->setObjectName("label");
        label->setGeometry(QRect(480, 140, 51, 31));
        label_2 = new QLabel(PaperTrackerMainWindow);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 290, 111, 31));

        retranslateUi(PaperTrackerMainWindow);

        QMetaObject::connectSlotsByName(PaperTrackerMainWindow);
    } // setupUi

    void retranslateUi(QWidget *PaperTrackerMainWindow)
    {
        PaperTrackerMainWindow->setWindowTitle(QCoreApplication::translate("PaperTrackerMainWindow", "PaperTrack", nullptr));
        SSIDText->setPlainText(QCoreApplication::translate("PaperTrackerMainWindow", "\350\257\267\350\276\223\345\205\245SSID", nullptr));
        PasswordText->setPlainText(QCoreApplication::translate("PaperTrackerMainWindow", "\350\257\267\350\276\223\345\205\245\345\257\206\347\240\201", nullptr));
        pushButton->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\345\217\221\351\200\201", nullptr));
        ImageLabel->setText(QString());
        label->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\344\272\256\345\272\246\350\260\203\346\225\264", nullptr));
        label_2->setText(QCoreApplication::translate("PaperTrackerMainWindow", "\346\227\245\345\277\227\347\252\227\345\217\243\357\274\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PaperTrackerMainWindow: public Ui_PaperTrackerMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H
