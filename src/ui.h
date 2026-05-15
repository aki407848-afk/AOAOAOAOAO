// src/ui.h - HTML интерфейс вынесен отдельно
#ifndef UI_H
#define UI_H

const char PAGE_MAIN[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>ChiperOS</title><style>
*{margin:0;padding:0;box-sizing:border-box}body{font:13px monospace;background:#0a0a0a;color:#c0c0c0}
.hd{text-align:center;padding:15px;border-bottom:2px solid #4a1c6e;margin-bottom:10px}
.kc{font-size:2em;color:#9b6eb5;text-shadow:0 0 8px #4a1c6e}.vr{color:#666;font-size:0.9em}
.bd{color:#9b6eb5;font-size:0.8em;background:#1a1a1a;display:inline-block;padding:3px 8px;border:1px solid #4a1c6e;border-radius:3px;margin-top:5px}
.ct{max-width:700px;margin:0 auto;padding:10px}
.tabs{display:flex;gap:5px;margin-bottom:10px}
.tab{flex:1;background:#1a1a1a;color:#888;border:1px solid #333;padding:8px;cursor:pointer;border-radius:4px;text-align:center;font-weight:bold}
.tab:hover{background:#252525}.tab.a{background:#4a1c6e;color:#fff}
.sc{background:#111;border:1px solid #333;border-radius:4px;padding:10px;display:none;margin-bottom:8px}.sc.a{display:block}
h3{color:#9b6eb5;border-bottom:1px solid #333;padding-bottom:5px;margin-bottom:8px;font-size:1em}
.btn{background:#4a1c6e;color:#fff;border:none;padding:8px 14px;margin:3px;cursor:pointer;border-radius:3px;font-weight:bold}
.btn:hover{background:#5a2c7e}.btn-r{background:#6e1c4a}.btn:disabled{background:#333;color:#666;cursor:not-allowed}
.log{background:#080808;padding:6px;height:100px;overflow-y:auto;font-size:0.85em;margin-top:6px;border:1px solid #222;border-radius:3px}
.row{display:flex;gap:6px;flex-wrap:wrap;align-items:center;margin:5px 0}
.net{padding:5px 6px;border-bottom:1px solid #222;display:flex;justify-content:space-between;cursor:pointer;font-size:0.9em}
.net:hover{background:#1a1a1a}.net.s{background:#4a1c6e}
.tag{background:#2a2a2a;padding:1px 4px;border-radius:2px;font-size:0.75em;color:#9b6eb5}
.func{background:#151515;border:1px solid #2a2a2a;padding:8px;margin:4px 0;border-radius:3px}
.ft{color:#9b6eb5;font-weight:bold;margin-bottom:4px;font-size:0.9em}
#st{font-weight:bold;color:#0ff}
</style></head><body>
<div class='hd'><div class='kc'>KOSMOC CREATOR</div><div class='vr'>ChiperOS v21.4</div><div class='bd'>ESP32-S3</div></div>
<div class='ct'>
<div class='tabs'>
<div class='tab a' onclick='sw("w")'>WIFI</div><div class='tab' onclick='sw("b")'>BLE</div><div class='tab' onclick='sw("a")'>AP</div>
</div>
<div id='w' class='sc a'><h3>WIFI DEAUTH</h3>
<div class='func'><div class='ft'>1. Scanner</div><div class='row'><button class='btn' onclick='scn()'>SCAN</button><span id='sn'></span></div><div id='nl' class='log'>Ready</div></div>
<div class='func'><div class='ft'>2. Target: <span id='tg'>None</span> | <span id='lk'>UNLOCKED</span></div></div>
<div class='func'><div class='ft'>3. MAX JAM</div>
<div class='row'><button class='btn btn-r' id='jg' onclick='jam(1)'>START</button><button class='btn' id='js' onclick='jam(0)' disabled>STOP</button></div>
<div id='jl' class='log'>Select target first</div></div></div>
<div id='b' class='sc'><h3>BLE SPAM</h3>
<div class='func'><div class='ft'>1. Scan BLE</div><div class='row'><button class='btn' onclick='bleScan()'>SCAN</button></div><div id='bl' class='log'>Tap to select</div></div>
<div class='func'><div class='ft'>2. Target: <span id='btgt'>None</span></div></div>
<div class='func'><div class='ft'>3. Connection Flood</div>
<div class='row'><button class='btn btn-r' id='bg' onclick='bleJ(1)'>START</button><button class='btn' id='bs2' onclick='bleJ(0)' disabled>STOP</button></div>
<div id='bjl' class='log'>Select BLE target</div></div></div>
<div id='a' class='sc'><h3>FAKE AP</h3>
<div class='func'><div class='ft'>1. Spam 350 APs</div>
<div class='row'><button class='btn' id='ag' onclick='apS(1)'>START</button><button class='btn' id='as' onclick='apS(0)' disabled>STOP</button></div>
<div id='al' class='log'>Names: ХАХАХА Я СПАМЕР1..350</div></div></div>
</div>
<script>
const $=id=>document.getElementById(id);
function sw(t){document.querySelectorAll('.tab').forEach(x=>x.classList.remove('a'));document.querySelectorAll('.sc').forEach(x=>x.classList.remove('a'));event.target.classList.add('a');$(t).classList.add('a');}
async function api(e){try{const r=await fetch(e);return r.ok?await r.json():null}catch{return null}}
function lg(id,m){const l=$(id);l.innerHTML=`[${new Date().toLocaleTimeString()}] ${m}<br>`+l.innerHTML;}
async function scn(){$('nl').innerHTML='Scanning...';await fetch('/sc');for(let i=0;i<15;i++){await new Promise(r=>setTimeout(r,400));const d=await api('/sc');if(d&&!d.scanning){let h='';d.forEach(n=>{h+=`<div class="net" onclick="lock('${n.b}')"><span>${n.s||'?'}<span class='tag'>${n.b}</span></span><span>${n.r}dBm</span></div>`;});$('nl').innerHTML=h||'None';$('sn').textContent=d.length+' nets';return;}}$('nl').innerHTML='Timeout';}
async function lock(b){const r=await api('/lock?b='+b);if(r&&r.locked){$('tg').innerHTML=b;$('lk').innerHTML='LOCKED';$('lk').style.color='#0f0';lg('nl','Locked');}}
async function jam(s){$('jg').disabled=s;$('js').disabled=!s;const r=await api('/jam?on='+(s?1:0));if(!r.ok&&s){lg('jl',r.msg||'SELECT TARGET');$('jg').disabled=false;$('js').disabled=true;return;}lg('jl',s?'DEAUTH ACTIVE':'Stopped');}
async function bleScan(){$('bl').innerHTML='Scanning...';const d=await api('/ble_scan');if(!d)return;let h='';d.forEach(n=>{h+=`<div class="net" onclick="bleSel('${n.m}')"><span>${n.n||n.m}<span class='tag'>${n.m}</span></span><span>${n.r}dBm</span></div>`;});$('bl').innerHTML=h||'No devices';}
async function bleSel(m){await api('/ble_sel?m='+m);$('btgt').innerHTML=m;lg('bl','Selected');}
async function bleJ(s){$('bg').disabled=s;$('bs2').disabled=!s;const r=await api('/ble_jam?on='+(s?1:0));if(!r.ok&&s){lg('bjl',r.msg||'SELECT BLE');$('bg').disabled=false;$('bs2').disabled=true;return;}lg('bjl',s?'BLE FLOOD':'Stopped');}
async function apS(s){$('ag').disabled=s;$('as').disabled=!s;await api('/ap?on='+(s?1:0));lg('al',s?'AP SPAM':'Stopped');}
async function st(){const d=await api('/st');if(!d)return;$('st').textContent=d.mode;$('jg').disabled=(d.mode!='IDLE');$('js').disabled=(d.mode=='IDLE');$('bg').disabled=(d.mode!='IDLE');$('bs2').disabled=(d.mode=='IDLE');$('ag').disabled=(d.mode!='IDLE');$('as').disabled=(d.mode=='IDLE');}
setInterval(st,1000);st();
</script></body></html>
)rawliteral";

#endif
