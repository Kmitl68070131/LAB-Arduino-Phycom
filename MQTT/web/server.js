// Browser ใช้ HTTP/SSE; Node เชื่อม MQTT TCP 1883 แทน browser
const http = require('node:http');
const fs = require('node:fs');
const mqtt = require('mqtt');
const sensors = new Set(['TMP36','MCP9700','LM35','TTC05103','NTC','Potentiometer','LDR','Ultrasonic','PIR','ButtonEvents','Button','Keypad','DHT11','DHT22']); // อุปกรณ์ที่อนุญาต
const clients = new Set(); // SSE responses ที่ยังเปิด
const broker = mqtt.connect(process.env.MQTT_URL || 'mqtt://phycom.it.kmitl.ac.th:1883'); // broker connection
broker.on('connect', () => broker.subscribe(['68070131/mqtt/+/telemetry','68070131/mqtt/+/ack','68070131/stats/+/telemetry','68070131/stats/+/ack','68070131/collections/+/telemetry','68070131/collections/+/ack','68070131/collections/+/history']));
broker.on('error', error => console.error('MQTT:', error.message)); // error=สาเหตุเชื่อมต่อล้มเหลว
broker.on('message', (topic, payload) => { // topic=หัวข้อ; payload=MQTT buffer
  const packet = JSON.stringify({topic, payload:payload.toString()}); // SSE event
  for (const response of clients) response.write(`data: ${packet}\n\n`); // response=browser stream
});
const server = http.createServer((request, response) => { // request=HTTP ขาเข้า; response=HTTP ขาออก
  if (request.url === '/' && request.method === 'GET') {
    response.writeHead(200, {'Content-Type':'text/html; charset=utf-8'});
    return response.end(fs.readFileSync(__dirname + '/index.html'));
  }
  if (request.url === '/events' && request.method === 'GET') {
    response.writeHead(200, {'Content-Type':'text/event-stream','Cache-Control':'no-cache','Connection':'keep-alive'});
    response.write(': connected\n\n'); clients.add(response);
    request.on('close', () => clients.delete(response)); return;
  }
  if (request.url === '/command' && request.method === 'POST') {
    let body = ''; // เนื้อหา POST ที่สะสม; จำกัด 2 KB
    request.on('data', chunk => { body += chunk; if (Buffer.byteLength(body) > 2048) request.destroy(); }); // chunk=ส่วนข้อมูล
    request.on('end', () => {
      try {
        const {mode, sensor, command} = JSON.parse(body); // โหมด, อุปกรณ์, JSON คำสั่ง
        if (!['mqtt','stats','collections'].includes(mode) || !sensors.has(sensor) || !command || Array.isArray(command) || typeof command !== 'object') throw Error('Invalid target/command');
        const encoded = JSON.stringify(command); // JSON ที่ส่งไปบอร์ด
        if (Buffer.byteLength(encoded) > 512) throw Error('Command >512 bytes');
        if (!broker.connected) { response.writeHead(503); return response.end('Broker offline'); }
        broker.publish(`68070131/${mode}/${sensor}/command`, encoded, {retain:false,qos:0}, error => {
          response.writeHead(error ? 502 : 202); response.end(error ? error.message : 'Published; wait for board ACK');
        }); // error=ผลส่ง broker ไม่ใช่ยืนยันบอร์ดรับ
      } catch (error) { response.writeHead(400); response.end(error.message); }
    }); return;
  }
  response.writeHead(404); response.end('Not found');
});
server.listen(Number(process.env.PORT || 3000), '127.0.0.1', () => console.log('Open http://127.0.0.1:3000')); // localhost สำหรับ lab
