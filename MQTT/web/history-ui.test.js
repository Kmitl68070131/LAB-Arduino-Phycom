// ทดสอบ script จริงของเว็บด้วย DOM adapter ไม่เชื่อม broker
const {test}=require('node:test');
const assert=require('node:assert/strict');
const fs=require('node:fs');
const vm=require('node:vm');
test('history UI separates DHT channels and builds selection/clear commands',()=>{
 const elements=new Map();
 const element=()=>({value:'',children:[],textContent:'',hidden:false,clicks:0,
   add(option){this.children.push(option);},append(child){this.children.push(child);},
   replaceChildren(){this.children=[];},click(){this.clicks++;}});
 const document={getElementById(id){if(!elements.has(id))elements.set(id,element());return elements.get(id);},createElement:element};
 let stream;
 class EventSource{constructor(){stream=this;}}
 const context={document,EventSource,Option:function(label,value){this.label=label;this.value=value;},setInterval(){},fetch(){throw Error('Unexpected network');}};
 const html=fs.readFileSync(__dirname+'/index.html','utf8');
 vm.runInNewContext(html.split('<script>')[1].split('</script>')[0],context);
 const ui=Object.fromEntries(elements);
 ui.mode.value='collections';ui.sensor.value='DHT22';ui.mode.onchange();
 assert.equal(ui.collectionsPanel.hidden,false);
 const receive=(channel,values)=>stream.onmessage({data:JSON.stringify({topic:'68070131/collections/DHT22/telemetry',payload:JSON.stringify({channel,history:{count:values.length,array:values,list:values}})})});
 receive('DHT22',[24,25]);receive('humidity',[60]);
 assert.equal(ui.historyRows.children.length,3);
 assert.equal(ui.historyRows.children[2].children[0].textContent,'humidity');
 assert.equal(ui.historyRows.children[2].children[2].textContent,'60');
 ui.structure.value='list';ui.historyIndex.value='1';ui.viewHistory.onclick();
 assert.deepEqual(JSON.parse(ui.command.value),{structure:'list',history_index:1,request_history:true});
 ui.clearHistory.onclick();assert.equal(JSON.parse(ui.command.value).clear_history,true);
 stream.onmessage({data:JSON.stringify({topic:'68070131/collections/DHT22/history',payload:JSON.stringify({primary:{count:0,array:[],list:[]},humidity:{count:0,array:[],list:[]}})})});
 assert.equal(ui.historyRows.children.length,0);
 ui.mode.value='mqtt';ui.mode.onchange();assert.equal(ui.collectionsPanel.hidden,true);
 assert.equal(ui.historyRows.children.length,0);
});
