#pragma once
#include <QtSql>
#include <QDebug>
#include <QtXml/qdom.h>

static bool createConnection() 
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setHostName("easybool-3313b0");
	db.setDatabaseName("data.db");
	db.setUserName("leng");
	db.setPassword("123456");
	if (!db.open()) {
		//��ʾ����
		return false;
	}

	QSqlQuery query;

	//�������ұ�
	query.exec("create table factory(id varchar primary key, name varchar)");
	query.exec(QStringLiteral("insert into factory values('0', '��ѡ�񳧼�')"));
	query.exec(QStringLiteral("insert into factory values('01', 'һ������')"));
	query.exec(QStringLiteral("insert into factory values('02', '��������')"));
	query.exec(QStringLiteral("insert into factory values('03', '�Ϻ�����')"));

	//����Ʒ�Ʊ�
	query.exec("create table brand(id varchar primary key, name varchar, factory varchar, price int, sum int, sell int, last int)");
	query.exec(QStringLiteral("insert into brand values('01', '�µ�A6', 'һ������', 36,50,10,40)"));
	query.exec(QStringLiteral("insert into brand values('02', '�ݴ�', 'һ������', 34,20,5,15)"));
	query.exec(QStringLiteral("insert into brand values('03', '����', 'һ������', 41,80,20,60)"));
	query.exec(QStringLiteral("insert into brand values('04', '����', 'һ������', 83,40,15,25)"));
	query.exec(QStringLiteral("insert into brand values('05', '�ϼ���', '��������', 39,50,15,35)"));
	query.exec(QStringLiteral("insert into brand values('06', '����', '��������', 28,60,10,50)"));
	query.exec(QStringLiteral("insert into brand values('07', '��־307', '��������', 27,70,20,50)"));
	query.exec(QStringLiteral("insert into brand values('08', 'ɣ����', '�Ϻ�����', 25,75,25,50)"));
	query.exec(QStringLiteral("insert into brand values('09', '������', '�Ϻ�����', 27,65,20,45)"));

	return true;
}

static bool createXml()
{
	QFile file("data.xml");
	if (file.exists()) {
		return true;
	}

	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return false;
	}

	QDomDocument doc;
	QDomProcessingInstruction instruction;
	instruction = doc.createProcessingInstruction("xml", "version=\"1.0\"encoding=\"UTF-8\"");
	doc.appendChild(instruction);
	QDomElement root = doc.createElement(QStringLiteral("�ճ������嵥��"));
	doc.appendChild(root);
	QTextStream out(&file);
	doc.save(out, 4);
	file.close();
	return true;
}


