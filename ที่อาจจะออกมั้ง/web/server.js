const http=require('node:http'); // HTTP ของเว็บค้นหาโจทย์
const fs=require('node:fs'); // โหลด HTML และ catalog.json
const path=require('node:path'); // ประกอบ path ตามโฟลเดอร์
const mqtt=require('mqtt'); // MQTT TCP เชื่อมต่อ Broker
const {WebSocketServer,WebSocket}=require('ws'); // WebSocket ให้ Browser
const catalog=JSON.parse(fs.readFileSync(path.join(__dirname,'..','catalog.json'),'utf8')); // Topic ที่อนุญาตเท่านั้น
const byId=new Map(catalog.map(x=>[x.id,x])); // อ้างอิงโจทย์ตามเลขข้อ
const BROKER=process.env.MQTT_HOST||'phycom.it.kmitl.ac.th'; // <<< เปลี่ยน Host ตรงนี้
const MQTT_PORT=Number(process.env.MQTT_PORT||1883); // <<< เปลี่ยนพอร์ต MQTT
const HTTP_PORT=Number(process.env.PORT||3006); // พอร์ตเว็บในเครื่อง
let online=false; // สถานะ TCP MQTT
const client=mqtt.connect('mqtt://'+BROKER+':'+MQTT_PORT,{clientId:'guess-web-'+process.pid+'-'+Math.random().toString(16).slice(2,8),reconnectPeriod:2500}); // เชื่อมต่อ MQTT
function sendFile(res,file,type){fs.readFile(file,(err,data)=>{if(err){res.writeHead(404);res.end('Not found');return;}res.writeHead(200,{'Content-Type':type,'Cache-Control':'no-store'});res.end(data)})} // ตอบไฟล์เว็บ
const server=http.createServer((req,res)=>{const p=(req.url||'/').split('?')[0];if(p==='/'||p==='/index.html')return sendFile(res,path.join(__dirname,'index.html'),'text/html; charset=utf-8');if(p==='/catalog.json')return sendFile(res,path.join(__dirname,'..','catalog.json'),'application/json; charset=utf-8');res.writeHead(404);res.end('Not found')}); // HTTP มีแค่ไฟล์ที่กำหนด
const wss=new WebSocketServer({server,path:'/ws'}); // Browser เชื่อมต่อผ่าน /ws
function broadcast(message){const payload=JSON.stringify(message);for(const ws of wss.clients)if(ws.readyState===WebSocket.OPEN)ws.send(payload)} // ส่งข้อมูลให้ทุก Browser
client.on('connect',()=>{online=true;client.subscribe('68070131/#',(err)=>{if(err){online=false;console.error(err.message);}broadcast({type:'connection',online})})}); // <<< Subscribe 68070131/# สำหรับข้อมูลในชุดตัวอย่าง
client.on('close',()=>{online=false;broadcast({type:'connection',online:false})}); // สถานะออฟไลน์
client.on('error',err=>console.error('MQTT error:',err.message)); // Debug
client.on('message',(topic,payload)=>{broadcast({type:'message',topic,value:payload.toString('utf8')})}); // MQTT -> Browser
wss.on('connection',ws=>{ws.send(JSON.stringify({type:'connection',online}));ws.on('message',raw=>{let msg;try{msg=JSON.parse(raw.toString())}catch{return;}const item=byId.get(msg?.id);if(!item||msg.type!=='command'||typeof msg.value!=='string'||msg.value.length<1||msg.value.length>40||!online)return;client.publish(item.subscribe,msg.value,{qos:0,retain:false},err=>{if(err)ws.send(JSON.stringify({type:'error',message:err.message}))})})}); // <<< เว็บ Publish ที่ Topic subscribe ของแต่ละข้อใน catalog.json
server.listen(HTTP_PORT,'127.0.0.1',()=>console.log('http://127.0.0.1:'+HTTP_PORT)); // เปิดเว็บภายในเครื่อง
