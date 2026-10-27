// GEN_Z MQTT Website Bridge: npm install && npm start
const express=require('express');
const mqtt=require('mqtt');
const app=express();
const clients=new Set();
const broker=mqtt.connect('mqtt://phycom.it.kmitl.ac.th:1883',{reconnectPeriod:3000});
app.use(express.json());
app.use(express.static(__dirname));
app.get('/',(req,res)=>res.redirect('/exercises.html'));
app.get('/events',(req,res)=>{res.setHeader('Content-Type','text/event-stream');res.setHeader('Cache-Control','no-cache');res.flushHeaders();clients.add(res);res.write(': connected\n\n');req.on('close',()=>clients.delete(res));});
app.post('/publish',(req,res)=>{const {topic,payload}=req.body||{};if(typeof topic!=='string'||!/^68070131\/[a-z0-9_]+$/.test(topic)||typeof payload!=='string'||payload.length>100)return res.status(400).json({error:'invalid topic or payload'});if(!broker.connected)return res.status(503).json({error:'broker offline'});broker.publish(topic,payload,err=>res.status(err?500:200).json({ok:!err}));});
broker.on('connect',()=>broker.subscribe('68070131/#'));
broker.on('message',(topic,data)=>{const msg='data: '+JSON.stringify({topic,payload:data.toString()})+'\n\n';for(const c of clients)c.write(msg)});
broker.on('error',e=>console.error(e.message));
setInterval(()=>{for(const c of clients)c.write(': ping\n\n')},25000);
app.listen(process.env.PORT||3000,()=>console.log('http://localhost:3000/exercises.html'));
