#include "../property.h"
#include <cassert>
using namespace prop;

class TestClass
{
public:
	TestClass()
		:
	GetOnly(this, &TestClass::getVar),
	SetOnly(this, &TestClass::setVar),
	FullProperty([this]() {return this->var; }, [this](int var) {this->var = var; })
	{}

	FunctionGetter<int> GetOnly;
	FunctionSetter<int> SetOnly;
	FunctionGetterSetter<int> FullProperty;

	void setVar(int val)
	{
		var = val;
	}
	int getVar() const
	{
		return var;
	}
private:
	int var = 0;
};

int main()
{
	DefaultGetter<float> DefGetter(10.0f);
	float fval = DefGetter;

	DefaultSetter<float> DefSetter(20.0f);
	DefSetter = 15.0f;

	DefaultGetterSetter<int> DefProperty(20);
	int ival = DefProperty;
	DefProperty = 200;

	TestClass tc;
	ival = tc.GetOnly;
	tc.SetOnly = 30;
	tc.FullProperty = 40;
	ival = tc.FullProperty;

	return 0;
}
