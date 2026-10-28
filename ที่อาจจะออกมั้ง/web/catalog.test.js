const test=require('node:test'); // Node Test Runner
const assert=require('node:assert/strict'); // ตรวจค่าในรายการโจทย์
const fs=require('node:fs'); // โหลด catalog
const path=require('node:path'); // หาตำแหน่งไฟล์
const catalog=JSON.parse(fs.readFileSync(path.join(__dirname,'..','catalog.json'),'utf8')); // อ่านโจทย์จริง
test('มี 18 ข้อและ Mock 6 ข้อ',()=>{assert.equal(catalog.length,18);assert.equal(catalog.filter(x=>x.category==='MOCK').length,6)}); // ตรวจจำนวน
test('ทุก Topic ขึ้นต้น 68070131/',()=>{for(const x of catalog){assert.ok(x.publish.startsWith('68070131/'));assert.ok(x.subscribe.startsWith('68070131/'))}}); // ตรวจ Prefix
test('ทุกโจทย์มีโค้ดและอุปกรณ์',()=>{for(const x of catalog){assert.ok(x.code.endsWith('.ino'));assert.ok(x.devices.length>=1)}}); // ตรวจรายการ
