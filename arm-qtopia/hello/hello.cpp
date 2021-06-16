#include "hello.h"
#include <qlabel.h>

HelloForm::HelloForm( QWidget* parent, const char* name, WFlags fl):
HelloBaseForm(parent, name, fl)
{
}
HelloForm::~HelloForm()
{
}

void HelloForm::SayHello()
{
	MessageLabel->setText("Hello, Qtopia world!");
}
