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

	QMenu *manageMenu;//"���۹���"���˵�
	QMenu *passwordMenu;//"�޸�����"���˵�

	//����Action����
	QAction *manageAction;
	QAction *chartAction;
	QAction *quitAction;

	QString getDateTime(DateTimeType type);

private slots:
	void on_manageMenu_clicked(); //ʵ�֡�Ʒ�Ƴ������Ӳ˵����ܺ���

	void on_chartMenu_clicked(); //ʵ�֡�����ͳ�ơ��Ӳ˵����ܺ���

	void on_quitMenu_clicked(); //ʵ�֡��˳����Ӳ˵����ܺ���

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

	void createMenuBar(); //�������ɲ˵����ĺ���
};
