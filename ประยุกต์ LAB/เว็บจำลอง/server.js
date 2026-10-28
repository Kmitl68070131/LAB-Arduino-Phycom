// สะพาน MQTT TCP -> WebSocket สำหรับเว็บ; ไม่เปลี่ยน Topic Arduino
const http=require('node:http');
const fs=require('node:fs');
const path=require('node:path');
const mqtt=require('mqtt');
const {WebSocketServer,WebSocket}=require('ws');
const PORT=Number(process.env.PORT||3001);
const HOST=process.env.MQTT_HOST||'phycom.it.kmitl.ac.th';
const PREFIX='68070131/applied/';
const client=mqtt.connect('mqtt://'+HOST+':1883',{clientId:'phycom-web-'+process.pid+'-'+Math.random().toString(16).slice(2,8),reconnectPeriod:2000,connectTimeout:7000});
let connected=false;
const server=http.createServer((req,res)=>{
  let p=req.url==='/'?'/index.html':(req.url||'/');
  p=decodeURIComponent(p.split('?')[0]);
  if(p!=='/index.html'){res.writeHead(404);res.end('Not found');return;}
  fs.readFile(path.join(__dirname,'index.html'),(err,b)=>{if(err){res.writeHead(500);res.end('error');return;}res.writeHead(200,{'Content-Type':'text/html; charset=utf-8','Cache-Control':'no-store'});res.end(b);});
});
const wss=new WebSocketServer({server,path:'/ws'});
const broadcast=(message)=>{const s=JSON.stringify(message);for(const ws of wss.clients)if(ws.readyState===WebSocket.OPEN)ws.send(s);};
client.on('connect',()=>{connected=true;client.subscribe(PREFIX+'#');broadcast({type:'status',connected:true});});
client.on('reconnect',()=>broadcast({type:'status',connected:false}));
client.on('close',()=>{connected=false;broadcast({type:'status',connected:false});});
client.on('error',err=>{connected=false;console.error('MQTT:',err.message);broadcast({type:'status',connected:false});});
client.on('message',(topic,payload)=>{broadcast({type:'message',topic,value:payload.toString('utf8'),time:new Date().toISOString()});});
wss.on('connection',ws=>{
  ws.send(JSON.stringify({type:'status',connected}));
  ws.on('message',raw=>{
    let m;try{m=JSON.parse(raw.toString());}catch{return;}
    if(!m||m.type!=='publish'||typeof m.value!=='string'||typeof m.topic!=='string')return;
    if(!/^68070131\/applied\/lab(0[1-9]|1[0-2])\/(sub|both)\/set$/.test(m.topic))return;
    if(m.value.length>32||m.value.length===0||!connected){ws.send(JSON.stringify({type:'error',error:'Broker offline or invalid payload'}));return;}
    client.publish(m.topic,m.value,{qos:0,retain:false},err=>{if(err)ws.send(JSON.stringify({type:'error',error:err.message}));});
  });
});
server.listen(PORT,()=>console.log('Web dashboard: http://localhost:'+PORT+' | Broker '+HOST+':1883'));
