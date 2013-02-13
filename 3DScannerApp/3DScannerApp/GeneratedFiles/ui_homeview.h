/********************************************************************************
** Form generated from reading UI file 'homeview.ui'
**
** Created: Tue Feb 12 17:32:30 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HOMEVIEW_H
#define UI_HOMEVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HomeViewClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *HomeViewClass)
    {
        if (HomeViewClass->objectName().isEmpty())
            HomeViewClass->setObjectName(QString::fromUtf8("HomeViewClass"));
        HomeViewClass->resize(600, 400);
        menuBar = new QMenuBar(HomeViewClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        HomeViewClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(HomeViewClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        HomeViewClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(HomeViewClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        HomeViewClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(HomeViewClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        HomeViewClass->setStatusBar(statusBar);

        retranslateUi(HomeViewClass);

        QMetaObject::connectSlotsByName(HomeViewClass);
    } // setupUi

    void retranslateUi(QMainWindow *HomeViewClass)
    {
        HomeViewClass->setWindowTitle(QApplication::translate("HomeViewClass", "HomeView", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class HomeViewClass: public Ui_HomeViewClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HOMEVIEW_H
