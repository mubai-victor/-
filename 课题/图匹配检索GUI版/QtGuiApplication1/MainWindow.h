#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <calculate.h>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	void init();

public slots:
	void OnItemDoubleClicked(QListWidgetItem * item);
	void OnRetrival();

private:
	Ui::MainWindowClass ui;
	int _id;
	Calculate _calculate;
	QPixmap _picture;
	char _graphFile[100] = "E:\\Desktop\\TempFile\\information.txt";
	char _pictureName[100] = "E:\\DataBase\\Drawing\\name.txt";
	char _picturePath[100] = "E:\\DataBase\\Drawing";
	char _shapeDistributionFile[100] = "E:\\Desktop\\TempFile\\ShapeDistribution.txt";	
};
