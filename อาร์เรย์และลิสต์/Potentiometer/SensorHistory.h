#pragma once
#include <math.h>
// ข้อมูลชุดเดียวเก็บสองโครงสร้างเพื่อเทียบผลและวิธีเข้าถึง
// RAM คงที่ ไม่ใช้ new/delete ป้องกัน heap fragmentation บนบอร์ด
struct SensorHistory {
  static const int CAPACITY = 20; // จำนวนค่าล่าสุดที่เก็บต่อช่อง
  float values[CAPACITY] = {}; // array แบบ ring buffer
  int nextWrite = 0; // ตำแหน่ง array ที่จะเขียนรอบหน้า
  int count = 0; // จำนวนค่าจริง 0..20 ของทั้ง array และ list
  struct Node {
    float value = 0; // ค่าที่อ่านจากเซนเซอร์
    Node* next = nullptr; // ลิงก์ไปโหนดถัดไป; nullptr = จบลิสต์
  };
  Node pool[CAPACITY]; // พื้นที่โหนดคงที่ ไม่ขอหน่วยความจำระหว่างทำงาน
  Node* head = nullptr; // โหนดเก่าสุด
  Node* tail = nullptr; // โหนดใหม่สุด
  SensorHistory() = default;
  SensorHistory(const SensorHistory&) = delete; // ไม่ copy เพราะมี pointer ภายใน pool
  SensorHistory& operator=(const SensorHistory&) = delete;
  void clear() { nextWrite = 0; count = 0; head = tail = nullptr; }
  bool append(float value) { // value=ค่าที่จะเพิ่ม; NaN/Inf ไม่เก็บ
    if (!isfinite(value)) return false;
    values[nextWrite] = value;
    nextWrite = (nextWrite + 1) % CAPACITY;
    Node* node; // โหนดว่างหรือโหนดเก่าสุดที่นำกลับใช้
    if (count < CAPACITY) { node = &pool[count]; ++count; }
    else { node = head; head = head->next; } // เต็มแล้วทิ้งตัวเก่าสุด
    node->value = value; node->next = nullptr;
    if (tail) tail->next = node;
    else head = node;
    tail = node;
    return true;
  }
  float arrayAt(int index) const { // index=ลำดับจากเก่าสุด เริ่ม 0; เข้าถึง O(1)
    if (index < 0 || index >= count) return NAN;
    int oldest = count == CAPACITY ? nextWrite : 0; // จุดเริ่มข้อมูลในวงแหวน
    return values[(oldest + index) % CAPACITY];
  }
  float listAt(int index) const { // index=ลำดับจากเก่าสุด; เดินลิงก์ O(N)
    if (index < 0 || index >= count) return NAN;
    const Node* node = head; // โหนดที่กำลังเยี่ยมชม
    for (int step = 0; step < index; ++step) node = node->next; // step=จำนวนลิงก์ที่เดิน
    return node->value;
  }
};
