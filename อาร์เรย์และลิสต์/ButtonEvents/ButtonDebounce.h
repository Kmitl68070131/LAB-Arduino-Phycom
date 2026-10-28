#pragma once
// ใช้ bool true=กด (ผู้เรียกแปลง LOW จาก INPUT_PULLUP ก่อน)
struct ButtonDebounce {
  bool raw = false; // ค่าที่อ่านล่าสุดก่อนกรอง
  bool stable = false; // ค่าสถานะที่นิ่งแล้ว
  unsigned long changedAt = 0; // เวลา raw เปลี่ยน หน่วย ms
  static const unsigned long DEBOUNCE_MS = 30; // ต้องนิ่งอย่างน้อย 30 ms
  void begin(bool initial, unsigned long now) { // initial=ค่าเริ่ม; now=เวลา uptime
    raw = stable = initial; changedAt = now;
  }
  bool update(bool reading, unsigned long now) { // reading=ค่าปุ่มรอบนี้; คืน true เมื่อเกิดขอบนิ่ง
    if (reading != raw) { raw = reading; changedAt = now; }
    if (raw != stable && now - changedAt >= DEBOUNCE_MS) { stable = raw; return true; }
    return false;
  }
  bool pressed() const { return stable; }
};
