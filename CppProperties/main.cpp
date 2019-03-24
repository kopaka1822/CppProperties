#include "property.h"
#include <cassert>
using namespace prop;

class TestClass
{
public:
	TestClass()
		:
	cg(this, &TestClass::getVar),
	cs(this, &TestClass::setVar),
	cgs(this, &TestClass::getVar, &TestClass::setVar)
	{}

	void setVar(int val)
	{
		var = val;
	}
	int getVar() const
	{
		return var;
	}

	ClassGetter<int, TestClass> cg;
	ClassSetter<int, TestClass> cs;
	ClassGetterSetter<int, TestClass> cgs;
private:
	int var = 0;
};

int main()
{
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
	ival = tc.cg;
	assert(ival == 0);

	tc.cs = 10;
	assert(tc.cg == 10);

	ival = tc.cgs;
	assert(tc.cg == 10);
	tc.cgs = 20;

	return 0;
}
