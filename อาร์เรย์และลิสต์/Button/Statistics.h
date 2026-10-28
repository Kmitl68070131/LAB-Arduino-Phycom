#pragma once
#include <initializer_list>
// เก็บ 20 ค่าล่าสุดของแต่ละช่อง; ไม่รวม NaN/timeout
struct WindowStats {
  static const int CAPACITY = 20; // ขนาดหน้าต่าง
  float samples[CAPACITY] = {}; // ค่าที่บันทึกใน ring buffer
  int count = 0; // จำนวนค่าที่มีจริง
  int next = 0; // ช่องที่จะเขียนครั้งถัดไป
  void clear() { count = 0; next = 0; }
  void add(float value) { // value = ค่าที่ผ่านการตรวจว่า finite แล้ว
    samples[next] = value; next = (next + 1) % CAPACITY;
    if (count < CAPACITY) ++count;
  }
  void json(JsonObject out) { // out = JSON object สำหรับผลสถิติ
    out["n"] = count;
    if (!count) { for (const char* key : {"min", "max", "avg", "median", "std"}) out[key] = nullptr; return; } // key=ชื่อสถิติ
    float sorted[CAPACITY]; // สำเนาเรียงลำดับ ไม่แก้ ring buffer
    float sum = 0; // ผลรวมค่า
    for (int i = 0; i < count; ++i) { sorted[i] = samples[i]; sum += samples[i]; } // i=ตำแหน่งข้อมูล
    for (int i = 1; i < count; ++i) {
      float held = sorted[i]; int j = i - 1; // held=ค่าที่แทรก; j=ตำแหน่งเทียบ
      while (j >= 0 && sorted[j] > held) { sorted[j+1] = sorted[j]; --j; }
      sorted[j+1] = held;
    }
    float mean = sum / count; // ค่าเฉลี่ย
    float squared = 0; // ผลรวมกำลังสองของส่วนต่างจากค่าเฉลี่ย
    for (int i = 0; i < count; ++i) { float delta = samples[i] - mean; squared += delta*delta; } // delta=ส่วนต่าง
    out["min"] = sorted[0]; out["max"] = sorted[count-1]; out["avg"] = mean;
    out["median"] = count % 2 ? sorted[count/2] : (sorted[count/2-1]+sorted[count/2])/2;
    out["std"] = sqrt(squared/count); // population std หาร N; N=1 ได้ 0
  }
};
