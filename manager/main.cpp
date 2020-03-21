#include "Widget.h"
#include <QtWidgets/QApplication>
#include "connection.h"

int main(int argc, char *argv[])
{
	if (!createConnection()||!createXml()) {
		return -1;
	}

	QApplication a(argc, argv);
	Widget w;
	w.show();
	return a.exec();
}
