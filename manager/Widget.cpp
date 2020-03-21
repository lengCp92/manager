#include "Widget.h"
#include "QSplitter"
#include <QtSql>
#include <QMessageBox>
#include <QListWidgetItem>

Widget::Widget(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::WidgetClass) 
{
	ui->setupUi(this);
	setFixedSize(750, 500);
	createMenuBar(); //生成菜单栏

	QSqlQueryModel *factoryModel = new QSqlQueryModel(this);
	factoryModel->setQuery("select name from factory");
	ui->sellFactoryComboBox->setModel(factoryModel);

	QSplitter *splitter = new QSplitter(ui->managerPage);
	splitter->resize(1020, 360);
	splitter->move(0, 50);
	splitter->addWidget(ui->toolBox);
	splitter->addWidget(ui->dailyList);
	splitter->setStretchFactor(0, 1);
	splitter->setStretchFactor(1, 1);

	on_sellCancelBtn_clicked();

	showDailyList();
}

Widget::~Widget() 
{

}

void Widget::createMenuBar() 
{
	manageAction = new QAction(QStringLiteral("品牌车管理"), this);
	chartAction = new QAction(QStringLiteral("销售统计"), this);
	quitAction = new QAction(QStringLiteral("退出"), this);

	manageAction->setShortcut(tr("Ctrl+M"));
	chartAction->setShortcut(tr("Ctrl+C"));
	quitAction->setShortcut(tr("Ctrl+Q"));

	manageMenu = menuBar()->addMenu(QStringLiteral("销售管理"));
	manageMenu->addAction(manageAction);
	manageMenu->addAction(chartAction);
	manageMenu->addSeparator();
	manageMenu->addAction(quitAction);

	passwordMenu = menuBar()->addMenu(QStringLiteral("修改密码"));

	connect(manageAction, SIGNAL(triggered()), this, SLOT(on_manageMenu_clicked()));
	connect(chartAction, SIGNAL(triggered()), this, SLOT(on_chartMenu_clicked()));
	connect(quitAction, SIGNAL(triggered()), this, SLOT(on_quitMenu_clicked()));
	connect(ui->sellFactoryComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_sellFactoryComboBox_currentIndexChanged(QString)));
	connect(ui->sellBrandComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_sellBrandComboBox_currentIndexChanged(QString)));
	connect(ui->sellNumSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_sellNumSpinBox_valueChanged(int)));

}

void Widget::on_manageMenu_clicked() 
{
	ui->stackedWidget->setCurrentIndex(0);
}

QString Widget::getDateTime(DateTimeType type)
{
	QDateTime datetime = QDateTime::currentDateTime();
	QString date = datetime.toString("yyyy-MM-dd");
	QString time = datetime.toString("hh:mm");
	QString dateAndTime = datetime.toString("yyyy-MM-dd dddd hh:mm");
	if (type == Date) {
		return date;
	}
	else if (type == Time) {
		return time;
	}
	else {
		return dateAndTime;
	}
}

void Widget::on_chartMenu_clicked()
{
	ui->stackedWidget->setCurrentIndex(1);
}

void Widget::on_quitMenu_clicked() 
{
	this->close();
}

void Widget::on_sellFactoryComboBox_currentIndexChanged(QString argl)
{
	if (argl == "请选择厂家") {
		on_sellCancelBtn_clicked();
	}
	else {
		ui->sellBrandComboBox->setEnabled(true);
		QSqlQueryModel *model = new QSqlQueryModel(this);
		model->setQuery(QString("select name from brand where factory = '%1'").arg(argl));
		ui->sellBrandComboBox->setModel(model);
		ui->sellCancelBtn->setEnabled(true);
	}
}

void Widget::on_sellBrandComboBox_currentIndexChanged(QString argl)
{
	ui->sellNumSpinBox->setValue(0);
	ui->sellNumSpinBox->setEnabled(false);
	ui->sellSumLineEdit->clear();
	ui->sellSumLineEdit->setEnabled(false);
	ui->sellOkBtn->setEnabled(false);

	QSqlQuery query;
	query.exec(QString("select price from brand where name='%1' and factory='%2'").arg(argl).arg(ui->sellFactoryComboBox->currentText()));
	query.next();
	ui->sellPriceLineEdit->setEnabled(true);
	ui->sellPriceLineEdit->setReadOnly(true);
	ui->sellPriceLineEdit->setText(query.value(0).toString());
	
	query.exec(QString("select last from brand where name='%1' and factory = '%2'").arg(argl).arg(ui->sellFactoryComboBox->currentText()));
	query.next();

	int num = query.value(0).toInt();
	if (num == 0) {
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("该品牌汽车已经售完!"), QMessageBox::Ok);
	}
	else {
		ui->sellNumSpinBox->setEnabled(true);
		ui->sellNumSpinBox->setMaximum(num);
		ui->sellLastNumLabel->setText(QStringLiteral("剩余数量:%1").arg(num));
		ui->sellLastNumLabel->setVisible(true);
	}
}

void Widget::on_sellNumSpinBox_valueChanged(int arg1)
{
	if (arg1 == 0) {
		ui->sellSumLineEdit->clear();
		ui->sellSumLineEdit->setEnabled(false);
		ui->sellOkBtn->setEnabled(false);
	}
	else {
		ui->sellSumLineEdit->setEnabled(true);
		ui->sellSumLineEdit->setReadOnly(true);
		qreal sum = arg1 * ui->sellPriceLineEdit->text().toInt();
		ui->sellSumLineEdit->setText(QString::number(sum));
		ui->sellOkBtn->setEnabled(true);
	}
}


void Widget::on_sellCancelBtn_clicked()
{
	ui->sellFactoryComboBox->setCurrentIndex(0);
	ui->sellBrandComboBox->clear();
}

void Widget::on_sellOkBtn_clicked()
{
	QString factory = ui->sellFactoryComboBox->currentText();
	QString name = ui->sellBrandComboBox->currentText();
	int value = ui->sellNumSpinBox->value();
	//sellNumSpinBox的最大值就是以前的剩余量
	int last = ui->sellNumSpinBox->maximum() - value;

	QSqlQuery query;
	//获取以前的销售量
	query.exec(QString("select sell from brand where name='%1' and factory= '%2'").arg(name).arg(factory));
	query.next();
	int sell = query.value(0).toInt() + value;

	//事务操作
	QSqlDatabase::database().transaction();
	bool rtn = query.exec(QString("update brand set sell='%1', last='%2' where name='%3' and factory='%4'").arg(sell).arg(last).arg(name).arg(factory));
	if (rtn) {
		QSqlDatabase::database().commit();
		QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("购车成功！"), QMessageBox::Ok);
		//writeXml();
		//showDailyList();
		on_sellCancelBtn_clicked();
	}
	else {
		QSqlDatabase::database().rollback();
	}

}

bool Widget::docRead()
{
	QFile file("data.xml");
	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	if (!doc.setContent(&file)) {
		file.close();
		return false;
	}
	file.close();
	return true;
}

bool Widget::docWrite()
{
	QFile file("data.xml");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return false;
	}
	QTextStream out(&file);
	doc.save(out, 4);
	file.close();
	return true;
}

void Widget::writeXml()
{
	//首先从文件读取
	if (docRead()) {
		QString currentDate = getDateTime(Date);
		QDomElement root = doc.documentElement();
		//根据是否有日期借点进行处理
		if (!root.hasChildNodes()) {
			QDomElement date = doc.createElement(QStringLiteral("日期"));
			QDomAttr curdate = doc.createAttribute("date");
			curdate.setValue(currentDate);
			date.setAttributeNode(curdate);
			root.appendChild(date);
			createNodes(date);
		}
		else {
			QDomElement date = root.lastChild().toElement();
			//根据是否已经有今天的日期借点进行处理
			if (date.attribute("date") == currentDate) {
				createNodes(date);
			}
			else {
				QDomElement date = doc.createElement(QStringLiteral("日期"));
				QDomAttr curdate = doc.createAttribute("date");
				curdate.setValue(currentDate);
				date.setAttributeNode(curdate);
				root.appendChild(date);
				createNodes(date);
			}
		}
		//写入文件
		docWrite();
	}
}

void Widget::createNodes(QDomElement & date)
{
	QDomElement time = doc.createElement(QStringLiteral("时间"));
	QDomAttr curTime = doc.createAttribute("time");
	curTime.setValue(getDateTime(Time));
	time.setAttributeNode(curTime);
	date.appendChild(time);
	QDomElement factory = doc.createElement(QStringLiteral("厂家"));
	QDomElement brand = doc.createElement(QStringLiteral("品牌"));
	QDomElement price = doc.createElement(QStringLiteral("报价"));
	QDomElement num = doc.createElement(QStringLiteral("数量"));
	QDomElement sum = doc.createElement(QStringLiteral("金额"));

	QDomText text;
	text = doc.createTextNode(
		QString("%1").arg(ui->sellFactoryComboBox->currentText()));
	factory.appendChild(text);

	text = doc.createTextNode(
		QString("%1").arg(ui->sellBrandComboBox->currentText()));
	brand.appendChild(text);

	text = doc.createTextNode(
		QString("%1").arg(ui->sellPriceLineEdit->text()));
	price.appendChild(text);

	text = doc.createTextNode(
		QString("%1").arg(ui->sellNumSpinBox->value()));
	num.appendChild(text);

	text = doc.createTextNode(
		QString("%1").arg(ui->sellSumLineEdit->text()));
	sum.appendChild(text);

	time.appendChild(factory);
	time.appendChild(brand);
	time.appendChild(price);
	time.appendChild(num);
	time.appendChild(sum);
}

void Widget::showDailyList()
{
	ui->dailyList->clear();
	if (docRead()) {
		QDomElement root = doc.documentElement();
		QString title = root.tagName();
		QListWidgetItem *titleItem = new QListWidgetItem;
		titleItem->setText(QString("----%1----").arg(title));
		titleItem->setTextAlignment(Qt::AlignCenter);
		ui->dailyList->addItem(titleItem);
		if (root.hasChildNodes()) {
			QString currentDate = getDateTime(Date);
			QDomElement dateElement = root.lastChild().toElement();
			QString date = dateElement.attribute("date");
			if (date == currentDate) {
				ui->dailyList->addItem("");
				ui->dailyList->addItem(QString("日期：%1").arg(date));
				ui->dailyList->addItem("");
				QDomNodeList children = dateElement.childNodes();
				//遍历当日销售的所有汽车
				for (int i = 0; i < children.count(); i++) {
					QDomNode node = children.at(i);
					QString time = node.toElement().attribute("time");
					QDomNodeList list = node.childNodes();
					QString factory = list.at(0).toElement().text();
					QString brand = list.at(1).toElement().text();
					QString price = list.at(2).toElement().text();
					QString num = list.at(3).toElement().text();
					QString sum = list.at(4).toElement().text();
					QString str = time + "出售" + brand + factory + "" + num + "辆，" + "成交价：" + price
						+ "万，共" + sum + "万元";
					QListWidgetItem *tempItem = new QListWidgetItem;
					tempItem->setText("***********************************");
					tempItem->setTextAlignment(Qt::AlignCenter);
					ui->dailyList->addItem(tempItem);
					ui->dailyList->addItem(str);
				}
			}
		}

	}
}


