// ทดสอบโค้ดโครงสร้างที่ใช้ในบอร์ดจริง โดยไม่ต้องต่อเซนเซอร์
#include <cassert>
#include <cmath>
#include <deque>
#include "../TMP36/SensorHistory.h"
int main() {
  SensorHistory history, humidity;
  std::deque<float> expected;
  assert(history.count==0 && std::isnan(history.arrayAt(0)) && std::isnan(history.listAt(0)));
  assert(!history.append(NAN) && !history.append(INFINITY));
  for(int value=1;value<=1000;++value){
    if(value%97==0){history.clear();expected.clear();}
    assert(history.append(value));expected.push_back(value);
    if(expected.size()>20)expected.pop_front();
    assert(history.count==static_cast<int>(expected.size()));
    const SensorHistory::Node* node=history.head;
    for(int index=0;index<history.count;++index){
      assert(history.arrayAt(index)==expected[index]);assert(history.listAt(index)==expected[index]);
      assert(node && node->value==expected[index]);node=node->next;
    }
    assert(node==nullptr && history.tail && history.tail->next==nullptr);
    assert(std::isnan(history.arrayAt(-1)) && std::isnan(history.listAt(20)));
  }
  humidity.append(55);assert(humidity.count==1 && humidity.arrayAt(0)==55);
  history.clear();assert(humidity.count==1 && history.head==nullptr && history.tail==nullptr);
  history.append(7);assert(history.count==1 && history.arrayAt(0)==7 && history.listAt(0)==7);
}
