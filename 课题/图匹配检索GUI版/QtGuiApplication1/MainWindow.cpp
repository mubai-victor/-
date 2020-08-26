#include "MainWindow.h"
#include <QSplitter>
#include <QList>
#include <string.h>
#include <iostream>
#include <QMessageBox>
#include <QTableWidget>
#include <queue>
#include <calculate.h>
#include <fstream>

#define MODELNUM 103
#define SIZE 100

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}
void MainWindow::init()
{
	std::string str;
	std::string temp;
	int pos = 0;

	std::ifstream file;
	file.open(_pictureName);
	if (!file) {
		QMessageBox::information(this,
			tr("failed to open file."),
			tr("failed to open file."));
	}

	char buff[SIZE];
	file.getline(buff,SIZE);
	while (file) {
		ui.listLeft->addItem(QString::fromLocal8Bit(buff));
		file.getline(buff, SIZE);
	}

	connect(ui.listLeft, &QListWidget::itemClicked, this, &MainWindow::OnItemDoubleClicked);
	connect(ui.pushButtonOk, &QPushButton::clicked, this, &MainWindow::OnRetrival);

	_calculate.readGraphFile(_graphFile);
	_calculate.readShapeDistribution(_shapeDistributionFile);
	setWindowState(Qt::WindowMaximized);

}

void MainWindow::OnRetrival()
{
	_calculate.clear();//首先把上次检索的结果清除，防止影响当前的检索
	std::vector<RetrivalRes> res = _calculate.retrival(_id);

	int width = ui.tableRight->width();
	int column =width / 200;
	int row = MODELNUM / column + (MODELNUM%column != 0);//计算tableWidget的行数，列数。

	ui.tableRight->clear();
	ui.tableRight->setColumnCount(column);
	ui.tableRight->setRowCount( 2 * row);

	QLabel *label = NULL;
	QPixmap pixmap = NULL;
	QListWidgetItem *item = NULL;
	
	QString text;
	int count = res.size();
	for (int i = 0; i < count; i++) {

		ui.tableRight->setColumnWidth(i % column , 200);
		ui.tableRight->setRowHeight(2 * (i / column), 200);

		RetrivalRes pres = res[i];
		item = ui.listLeft->item(pres._graphNum);
		QString pictureName = QString("")+_picturePath + "\\" + item->text() + ".png";
		label = new QLabel;

		pixmap.load(pictureName);
		label->setPixmap(pixmap);

		text = "depth:"+QString::number(pres._depth)+" sim"+QString::number(pres._similarity);
		ui.tableRight->setCellWidget(2 * (i / column), i % column, label);
		ui.tableRight->setItem(2 * (i / column) + 1, i % column, new QTableWidgetItem(text));
	}

}

void MainWindow::OnItemDoubleClicked(QListWidgetItem * item)
{
	_id = ui.listLeft->currentRow();
	QString pictureName = QString("")+_picturePath + "\\" + item->text() + ".png";

	if (!_picture.load(pictureName)) {
		QMessageBox::information(this,
			tr("failed to open file."),
			tr("failed to open file."));
	}
	ui.label->setPixmap(_picture);
}