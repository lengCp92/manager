#pragma once

#include <QtWidgets/QWidget>
#include "ui_Widget.h"
#include <QMainWindow>
#include <QMenuBar>
#include <QDomDocument>


enum DateTimeType { Time, Date, DateTime };

class Widget : public QMainWindow
{
	Q_OBJECT

public:
	explicit Widget(QWidget *parent = Q_NULLPTR);
	~Widget();

	QMenu *manageMenu;//"销售管理"主菜单
	QMenu *passwordMenu;//"修改密码"主菜单

	//动作Action定义
	QAction *manageAction;
	QAction *chartAction;
	QAction *quitAction;

	QString getDateTime(DateTimeType type);

private slots:
	void on_manageMenu_clicked(); //实现“品牌车管理”子菜单功能函数

	void on_chartMenu_clicked(); //实现“销售统计”子菜单功能函数

	void on_quitMenu_clicked(); //实现“退出”子菜单功能函数

	void on_sellFactoryComboBox_currentIndexChanged(QString argl);

	void on_sellBrandComboBox_currentIndexChanged(QString argl);

	void on_sellNumSpinBox_valueChanged(int arg1);

	void on_sellCancelBtn_clicked();

	void on_sellOkBtn_clicked();


private:
	Ui::WidgetClass *ui;
	QDomDocument doc;
	bool docRead();
	bool docWrite();
	void writeXml();
	void createNodes(QDomElement &date);
	void showDailyList();

	void createMenuBar(); //用于生成菜单栏的函数
};
