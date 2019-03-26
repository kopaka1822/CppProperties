# CppProperties
Single header file C# like properties for c++

Example for default properties (The value is embedded in the property).

```c++
using namespace prop;

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
```

Example for properties with custom getter/setter (with class member functions or lambda functions).

```c++
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

   void setVar(int val) {
      var = val;
   }
   int getVar() const {
      return var;
   }
private:
   int var = 0;
};

// usage:
TestClass tc;
ival = tc.GetOnly;
tc.SetOnly = 30;
tc.FullProperty = 40;
ival = tc.FullProperty;
```
