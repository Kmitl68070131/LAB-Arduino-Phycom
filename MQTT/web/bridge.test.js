// ทดสอบ bridge จริงผ่าน HTTP โดยชี้ broker ไป port ปิด ไม่ส่งข้อมูลเข้า broker ห้องเรียน
const {test}=require('node:test');
const assert=require('node:assert/strict');
const {spawn}=require('node:child_process');
test('serves UI and rejects malformed/offline commands',async()=>{
 const child=spawn(process.execPath,['server.js'],{cwd:__dirname,env:{...process.env,PORT:'32187',MQTT_URL:'mqtt://127.0.0.1:1'},stdio:['ignore','pipe','ignore']});
 try{
  await new Promise((resolve,reject)=>{child.stdout.once('data',resolve);child.once('error',reject);setTimeout(()=>reject(Error('Startup timeout')),5000).unref();});
  const url='http://127.0.0.1:32187';
  assert.match(await (await fetch(url)).text(),/MQTT JSON/);
  const post=body=>fetch(url+'/command',{method:'POST',body});
  assert.equal((await post('{broken')).status,400);
  assert.equal((await post(JSON.stringify({mode:'stats',sensor:'unknown',command:{}}))).status,400);
  assert.equal((await post(JSON.stringify({mode:'stats',sensor:'TMP36',command:{text:'x'.repeat(600)}}))).status,400);
  assert.equal((await post(JSON.stringify({mode:'stats',sensor:'TMP36',command:{text:'hi'}}))).status,503);
 }finally{child.kill();}
});
