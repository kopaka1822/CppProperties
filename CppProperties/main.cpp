#include "property.h"
#include <cassert>
using namespace prop;

class TestClass
{
public:
	TestClass()
		:
	fg(this, &TestClass::getVar),
	fs(std::bind(&TestClass::setVar, this, std::placeholders::_1)),
	fgs([this]() {return this->var; }, [this](int var) {this->var = var; })
	{}
private:
	void setVar(int val)
	{
		var = val;
	}
	int getVar() const
	{
		return var;
	}
public:
	FunctionGetter<int> fg;
	FunctionSetter<int> fs;
	FunctionGetterSetter<int> fgs;
private:
	int var = 0;
};

int main()
{
	//p1 = 1.0f;
	//float val = p1;


	DefaultGetter<float> dg(10.0f);
	float fval = dg;
	assert(fval == 10.0f);

	DefaultSetter<float> ds(20.0f);
	ds = 15.0f;

	DefaultGetterSetter<int> dgs(20);
	int ival = dgs;
	dgs.DefaultSetter<int>::operator=(200);
	assert(ival == 20);
	dgs = 200;
	assert(dgs == 200);

	TestClass tc;

	ival = tc.fg;
	assert(ival == 0);

	tc.fs = 30;
	assert(tc.fg == 30);

	tc.fgs = 40;

	return 0;
}
