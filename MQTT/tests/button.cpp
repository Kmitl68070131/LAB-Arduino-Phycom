// ทดสอบตัวกรองจริง: bounce, hold, release และ millis wrap
#include <cassert>
#include <climits>
#include "../ButtonEvents/ButtonDebounce.h"
int main(){
 ButtonDebounce button;
 button.begin(false,0);
 assert(!button.update(true,1));
 assert(!button.update(false,5));
 assert(!button.update(true,10));
 assert(!button.update(true,39));
 assert(button.update(true,40));assert(button.pressed());
 assert(!button.update(true,300)); // hold ไม่เกิดขอบซ้ำ
 assert(!button.update(false,301));assert(button.update(false,331));assert(!button.pressed());
 button.begin(true,1000);assert(!button.update(true,2000)); // เริ่มกดค้างไม่เพิ่มขอบ
 button.begin(false,ULONG_MAX-20);
 assert(!button.update(true,ULONG_MAX-10));
 assert(button.update(true,20)); // unsigned rollover
}
