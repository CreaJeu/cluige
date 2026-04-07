//TODO

Members :
-  snake_case
-  public : no _ prefix
-  private : one _ prefix

In .c
-  prefixed static functions to feed function pointers :
  -  nde_public_func()
  -  nde__private_func()


function pointers :
-  virtual/inheritance : in `_Struct`
-  non virtual/inheritance : in `_iiStruct`


methods :
-  name equivalent of "this": `this_MyClass`
  -  void my_method(MyClass* this_MyClass)
  -  void my_method_2(const MyClass* this_MyClass, int arg_1, int arg_2)


comments :
-  prefer `//` rather than `/* */`
-  put comments above concerned lines rather than below (trick to remember : think usual .h). Short comments at the end of short lines is acceptable.

General :
-  indentation : tab (no space indent, any remaining space-indents will be fixed in near future) (in Code::Blocks, check the checkbox to force tab-indent)
-  use but don't abuse of new lines

C++ compatibility :
-  C++ keywords like `new`, `this`, `class`, `delete`, `private`, `public`, `try`, `throw`, `template` are forbidden, to facilitate usage by other projects in C++.
