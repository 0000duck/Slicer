﻿                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             #include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTextEdit>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QSplitter>
#include <QTableView>
#include <QDebug>

#include "mainwindow.h"

static float faceArray[4][9] = { {0.0,  1.0,  0.0, -1.0, -1.0,  1.0, 1.0, -1.0, 1.0},
                                       {0.0,  1.0,  0.0,1.0, -1.0,  1.0,1.0, -1.0, -1.0 },
                                       {0.0,  1.0,  0.0,1.0, -1.0, -1.0,-1.0, -1.0, -1.0},
                                       {0.0,  1.0,  0.0, -1.0, -1.0, -1.0,-1.0, -1.0,  1.0}
                                      };


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Slicer"));
    openAction = new QAction(QIcon(":/images/file-open"), tr("&Open..."), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing file"));

    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    QMenu *file = menuBar()->addMenu(tr("&File"));
    file->addAction(openAction);

    statusBar() ;
    QWidget *cenWidget = new QWidget(this); //this is point to QMainWindow
    this->setCentralWidget(cenWidget);

    opengl= new MyGLWidget(cenWidget);
    opengl->nFaceCount = nFaceCount;
    memcpy(opengl->faceArray,faceArray,sizeof(float)*nFaceCount*9);
    qDebug()<<"Number of faces:"<<nFaceCount<<endl;
    tableWidget =new MyTableWidget((cenWidget));
    tableWidget->setRowCount(opengl->nFaceCount);
    for(int i=0; i< opengl->nFaceCount; i++)
    {
        for (int j=0;j<9;j++)
        {
            tableWidget->setItem(i,j, new QTableWidgetItem(QString::number(faceArray[i][j],'f',2)));
        }

    }
    tableWidget->show();
    connect(tableWidget,SIGNAL(rowClicked(int)),opengl,SLOT(changeColorFlag(int)));

    QPushButton *button = new QPushButton(tr("useless"),(cenWidget));

    QHBoxLayout *mainlayout = new QHBoxLayout(cenWidget);

    QVBoxLayout *rightlayout = new QVBoxLayout(cenWidget);
    QSplitter *splitter=new QSplitter(cenWidget);

    rightlayout->addWidget(tableWidget);
    rightlayout->addWidget(button);

    splitter->addWidget(opengl);
    splitter->setMinimumWidth(400);
    splitter->setMinimumHeight(400);
    cenWidget->setLayout(mainlayout);

    mainlayout->addWidget(splitter);
    mainlayout->addLayout(rightlayout);
}

MainWindow::~MainWindow()
{

}

void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Open File"),
                                                ".",
                                                tr("Text Files(*.txt)"));
    if(!path.isEmpty()) {
        QFile file(path);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Read File"),
                                 tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QTextStream in(&file);
        nFaceCount=0;
        while (!in.atEnd())
        {
            QString line = in.readLine();
            qDebug()<<line <<endl;
            QStringList str_list= line.split(" ");

            for (int i=0; i<str_list.length(); ++i) {
                qDebug()<<str_list[i].toFloat()<<endl;
                Array[nFaceCount][i] = str_list[i].toFloat();
            }
            ++nFaceCount;
        }
        opengl->nFaceCount = nFaceCount;
        memcpy(opengl->faceArray,Array,sizeof(float)*nFaceCount*9);
        qDebug()<<"Number of faces:"<<nFaceCount<<endl;
        QMessageBox::information(0,tr("Message"),tr("Success!"));
        file.close();

    } else {
        QMessageBox::warning(this, tr("Path"),
                             tr("You did not select any file."));
    }
}
