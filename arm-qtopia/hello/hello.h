#ifndef HELLOFORM_H
#define HELLOFORM_H
#include "hello_base.h"

class HelloForm : public HelloBaseForm
{ 
public:
    HelloForm( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    virtual ~HelloForm();
protected:
    virtual void SayHello();
};

#endif // HELLOFORM_H
