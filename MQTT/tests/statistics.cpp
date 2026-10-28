// Host test ใช้ header จริงกับ JsonObject adapter จึงไม่ต้องใช้บอร์ด
#include <map>
#include <string>
#include <cmath>
#include <cassert>
#include <initializer_list>
struct Value { double number=0; bool null=false; void operator=(double n){number=n;null=false;} void operator=(std::nullptr_t){null=true;} };
struct JsonObject { std::map<std::string,Value>* values; Value& operator[](const char* key){return (*values)[key];} };
#include "../TMP36/Statistics.h"
int main(){
  WindowStats stats;
  std::map<std::string,Value> values;
  JsonObject out{&values};
  stats.json(out); assert(values["min"].null);
  for(float value:{1,2,3,4}) stats.add(value);
  stats.json(out);
  assert(values["min"].number==1 && values["max"].number==4);
  assert(values["median"].number==2.5 && values["avg"].number==2.5);
  assert(std::abs(values["std"].number-std::sqrt(1.25))<1e-6);
  stats.clear();stats.add(9);stats.json(out);assert(values["std"].number==0);
  stats.clear();for(int i=1;i<=25;++i)stats.add(i);stats.json(out);
  assert(values["n"].number==20 && values["min"].number==6 && values["max"].number==25 && values["median"].number==15.5);
}
