#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define HOME
//#define WORK
//#define APONLY false;
//#define OAT false;
//#define fourswitch

//#define SMTP
#define POSTMARK
//#define NOEMAIL

#define REDIRECT
#define POWERON

bool allowSendEmail = true;

#if defined(OTA)
#include <ArduinoOTA.h>
#endif

//#define DEBUG

MDNSResponder mdns;

//-------------------------------------------------------------------------------

// Replace with your network credentials
#if defined(HOME)

const char* ssid = "MYSSID";
const char* password = "MYPASS";
const char* host = "10.1.1.20";
IPAddress ip(10, 1, 1, 20);
IPAddress gateway(10,1,1,69);
IPAddress subnet(255,255,255,0);
const char* myhostname = "MYesp8266-1";
#endif 
//-------------------------------------------------------------------------------
  
#if defined(WORK)
const char* ssid = "MYSSID";
const char* password = "MYPASS";
const char* host = "192.168.2.10";
IPAddress ip(192, 168, 2, 10);
IPAddress gateway(192,168,2,2);
IPAddress subnet(255,255,255,0);
const char* myhostname = "workesp8266";
#endif


//-------------------------------------------------------------------------------



//-----------------------WiFi AP Info--------------------------------------------------------

const char WiFiAPPSK[] = "esp8266fun";
bool isWifiAP = false;

//-------------------------WebServer Info------------------------------------------------------

ESP8266WebServer server(80);

const char* www_username = "user1";
const char* www_password = "password1";

const char* www1_username = "user2";
const char* www1_password = "password2";


String AuthedUser="";
String ConnectedIP="";

String webPage = "";
int cmdcounter = 0;
//------------------------------------------------------------------------------------

#if defined (POSTMARK)
#define HOST     "api.postmarkapp.com"   
#define PORT     80       

 #endif
//---------------------------------------------------


//-----------------Hardware Info----------------------------------------------

int gpio_garage1in = 14;
int gpio_garage2in = 5;

int garage1state = HIGH;
int garage2state = HIGH;


int gpio_powerpin = 12;
int gpio0_pin = 15;
int gpio1_pin = 13;
bool switch1state = false;
bool switch2state = false;
#if defined(fourswitch)
int gpio0_pin = 15;
int gpio1_pin = 13;
int gpio2_pin = 12;
int gpio3_pin = 14;
bool switch3state = false;
bool switch4state = false;
#endif

int led_pin = 2; //LED
int testpin = 4;

bool ledstate = true;  // on by default


#define SWHIGH LOW
#define SWLOW HIGH
//---------------- Misc---------------------------------------------------------

WiFiClient client;

int count = 0; // we'll use this int to keep track of which command we need to send next
bool send_flag = false; // we'll use this flag to know when to send the email commands
int OnOff=0;



void blink(int count) {

  if (ledstate) {
    for (int x=0;x < count;x++){
      digitalWrite(led_pin, HIGH);
      delay(100);
      digitalWrite(led_pin, LOW);
      delay(200);
    }
    
  }else {

  for (int x=0;x < count;x++){
      digitalWrite(led_pin, LOW);
      delay(100);
      digitalWrite(led_pin, HIGH);
      delay(200);
    }
  }
  
   Serial.println("Blink");
}

void setWebPage(bool refresh) {


  String mycmd=server.arg("counter");
  int mycdmint = mycmd.toInt();
  
  if (mycdmint != 999999 && refresh){
     cmdcounter++;
  }
  
  webPage = "<html><head>";
  if (refresh){
#if defined (REDIRECT)
    webPage += "<meta http-equiv=\"refresh\" content=\"2; url=/\" />";
#endif
  }
  webPage += "</head><body>";
  
  webPage += "<h1>";
  webPage += myhostname;
  webPage += " Web Server</h1><br>";

  webPage += "Door Status:<br>";
  webPage += "Door 1:[";
  if (garage1state == HIGH) {
    webPage += "OPEN";
  }else {
    webPage += "CLOSED";
  }
  webPage +="]<br>Door 2:[";
  if (garage2state == HIGH) {
    webPage += "OPEN";
  }else {
    webPage += "CLOSED";
  }
  webPage +="]<br><br>";
  
  
  webPage += "<p>LED :[";
  if (ledstate == 1){
    webPage += "&nbsp;On";
  }else {
    webPage += "Off";
  }
  webPage += "] <a href=\"ledOn?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>ON</button></a>&nbsp;<a href=\"ledOff?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>OFF</button></a>&nbsp;<a href=\"ledToggle?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>Toggle</button></a>&nbsp;<a href=\"ledBounce?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>Bounce</button></a> </p>"; 
  
  webPage += "<p>Switch #1 [";
  if (switch1state == 1){
    webPage += "&nbsp;On";
  }else {
    webPage += "Off";
  }
  webPage += "] <a href=\"switch1On?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>ON</button></a>&nbsp;<a href=\"switch1Off?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>OFF</button></a>&nbsp;<a href=\"switch1Toggle?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>Toggle</button></a>&nbsp;<a href=\"switch1Bounce?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>Bounce</button></a></p>";
  
  
  webPage += " <p>Switch #2 [";
   if (switch2state == 1){
    webPage += "&nbsp;On";
  }else {
    webPage += "Off";
  }
  webPage += "]<a href=\"switch2On?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>ON</button></a>&nbsp;<a href=\"switch2Off?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>OFF</button></a>&nbsp;<a href=\"switch2Toggle?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>Toggle</button></a>&nbsp;<a href=\"switch2Bounce?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>Bounce</button></a></p>";

#if defined(fourswitch)

webPage += " <p>Switch #3 [";
   if (switch3state == 1){
    webPage += "&nbsp;On";
  }else {
    webPage += "Off";
  }
  webPage += "]<a href=\"switch3On?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>ON</button></a>&nbsp;<a href=\"switch3Off?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>OFF</button></a>&nbsp;<a href=\"switch3Toggle?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>Toggle</button></a>&nbsp;<a href=\"switch3Bounce?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>Bounce</button></a></p>";


webPage += " <p>Switch #4 [";
   if (switch4state == 1){
    webPage += "&nbsp;On";
  }else {
    webPage += "Off";
  }
  webPage += "]<a href=\"switch4On?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>ON</button></a>&nbsp;<a href=\"switch4Off?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>OFF</button></a>&nbsp;<a href=\"switch4Toggle?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>Toggle</button></a>&nbsp;<a href=\"switch4Bounc?counter=";
  webPage += cmdcounter;
  webPage += "e\"><button>Bounce</button></a></p>";




#endif
   
   
  webPage += "</br></br> <a href=\"reset?counter=";
  webPage += cmdcounter;
  webPage += "\"><button>RESET</button></a>";
  webPage += "</body></html>";
}
void nocache(){

 server.sendHeader("Cache-Control", "no-store, must-revalidate");
 server.sendHeader("Pragma","no-cache");
 server.sendHeader("Expires","0");
 
}

void initHardware() {

  pinMode(gpio_garage1in, INPUT_PULLUP);
  pinMode(gpio_garage2in, INPUT_PULLUP);
  garage1state=HIGH;
  garage2state=HIGH;


//Power Pin
  pinMode(gpio_powerpin, OUTPUT);
  digitalWrite(gpio_powerpin, SWLOW);
  switch1state=false;
  
 //Switch 1
  pinMode(gpio0_pin, OUTPUT);
  digitalWrite(gpio0_pin, SWLOW);
  switch1state=false;
//Switch 2
  pinMode(gpio1_pin, OUTPUT);
  digitalWrite(gpio1_pin, SWLOW);
  switch2state=false;

#if defined (fourswitch)
 //Switch 1
  pinMode(gpio2_pin, OUTPUT);
  digitalWrite(gpio2_pin, SWLOW);
  switch3state=false;
//Switch 2
  pinMode(gpio3_pin, OUTPUT);
  digitalWrite(gpio3_pin, SWLOW);
  switch4state=false;
#endif

  
 //LED Pin
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
//Test Pin
  pinMode(testpin, OUTPUT);
  digitalWrite(testpin, LOW);

  
  
}
void turnOnRelay() {
  digitalWrite(gpio_powerpin, HIGH);
}
void turnOffRelay() {
  digitalWrite(gpio_powerpin, LOW);
}
void getDoorStatus() {

garage1state = digitalRead(gpio_garage1in);
garage2state = digitalRead(gpio_garage2in);

//Serial.print("Door Status: 1:[");
//Serial.print(garage1state);
//Serial.print("] 2:[");
//Serial.print(garage2state);
//Serial.println("]");
}


boolean initWifiClient() {

  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);   
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  byte mac[6];
  WiFi.macAddress(mac); 

  Serial.print("MAC: ");
  Serial.print(mac[0],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.println(mac[5],HEX);
  Serial.println(" Wifi Client Begin:");

  // Wait for connection
  int clientcounter=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1500);
    blink(3);
    clientcounter++;
    Serial.print("Client wait: [");
    Serial.print(clientcounter);
    Serial.println("]");
    
    if (clientcounter >= 120 ) {
      return false;
    }
    
  }
 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  return true;
  
}
void initWifiAP(){

  Serial.println(" WifI AP Init Begin:");
  WiFi.mode(WIFI_AP);
  
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
               String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;
  
  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);
  
  for (int i=0; i<AP_NameString.length(); i++)
  AP_NameChar[i] = AP_NameString.charAt(i);


  
  WiFi.softAP(AP_NameChar, WiFiAPPSK);
  IPAddress myIP = WiFi.softAPIP();
  
  isWifiAP = true;


   Serial.print(" WifI AP SSID:[");
   Serial.print(AP_NameChar);
   Serial.print("]  IP Address:[");
   Serial.print(myIP);
   Serial.println("]");
   
  
}
boolean checkAuth() {

if (server.authenticate(www_username, www_password)){
  AuthedUser=www_username;
  return true ;
}
else if (server.authenticate(www1_username, www1_password))
{
  AuthedUser=www1_username;
  return true ;
}

 return false; 
}


void processSwitch(String myswitch, String mycommand, int gpio ){


  String mycmd=server.arg("counter");
  Serial.print ("CMD:[");
  Serial.print (mycmd);
  Serial.println ("]");
  int mycdmint = mycmd.toInt();
  if (mycdmint != cmdcounter && mycdmint != 999999) {
    Serial.print("Error: Expected:[");
    Serial.print(cmdcounter);
    Serial.print("] Got:[");
    Serial.print(mycdmint);
    Serial.print("]");
    setWebPage(true);
    nocache();
    server.send(200, "text/html", webPage);
    delay(200);
        
    return;
  }

  
   bool switchstate = false;

    if (myswitch == "Switch 1") switchstate = switch1state ;
    if (myswitch == "Switch 2") switchstate = switch2state ;
#if defined(fourswitch)
    if (myswitch == "Switch 3") switchstate = switch3state ;
    if (myswitch == "Switch 4") switchstate = switch4state ;
#endif   

   
   ConnectedIP=server.client().remoteIP().toString();
   Serial.print("Client Connected:");
   Serial.println(ConnectedIP);
  if(!checkAuth())
      return server.requestAuthentication();
  String str = myswitch;
    
  if (mycommand == "on"){
      digitalWrite(gpio, SWHIGH);
      switchstate=true;
      setWebPage(true);
      nocache();
      server.send(200, "text/html", webPage);
      str += " On";
  } else if (mycommand == "off") {
      digitalWrite(gpio, SWLOW);
      switchstate=false;
      setWebPage(true);
      nocache();
      server.send(200, "text/html", webPage);
      str += " Off"; 
  } else if (mycommand == "toggle"){
      if (switchstate == true){
       digitalWrite(gpio, SWLOW);
        switchstate=false;
        setWebPage(true);
        nocache();
        server.send(200, "text/html", webPage);
        str += " On";
      }else {
       digitalWrite(gpio, SWHIGH);
        switchstate=true;
        setWebPage(true);
        nocache();
        server.send(200, "text/html", webPage);
        str += " Off";
      }
    } else if (mycommand == "bounce") {
      str += " Bounce";
      if (switchstate == true){
          digitalWrite(gpio, SWLOW);
          delay(500);
          digitalWrite(gpio, SWHIGH);
          setWebPage(true);
          nocache();
          server.send(200, "text/html", webPage);
          
        }else {
          digitalWrite(gpio, SWHIGH);
          delay(500);
          digitalWrite(gpio, SWLOW);
          setWebPage(true);
          nocache();
          server.send(200, "text/html", webPage);
          
        }
    }
   if (myswitch == "Switch 1") switch1state = switchstate;
   if (myswitch == "Switch 2") switch2state = switchstate;
#if defined(fourswitch)
   if (myswitch == "Switch 3") switch3state = switchstate;
   if (myswitch == "Switch 4") switch4state = switchstate;
 #endif   
Serial.println(str); 
sendEmail(str);
delay(200);
  
}


void initHttpServer(){

   setWebPage(false);
   

   server.on("/", [](){
   ConnectedIP=server.client().remoteIP().toString();
   Serial.print("Client Connected:");
   Serial.println(ConnectedIP);
    if(!checkAuth())
       return server.requestAuthentication();
    Serial.println("Authenticated");
    setWebPage(false);
    nocache();
    server.send(200, "text/html", webPage);
  });
 
 server.on("/reset", [](){
  ConnectedIP=server.client().remoteIP().toString();
   Serial.print("Client Connected:");
   Serial.println(ConnectedIP);
  Serial.println("Reseting");
    if(!checkAuth())
      return server.requestAuthentication();
    ESP.restart();
   
  });
  
 server.on("/ledOn", [](){
  ConnectedIP=server.client().remoteIP().toString();
   Serial.print("Client Connected:");
   Serial.println(ConnectedIP);
    if(!checkAuth())
      return server.requestAuthentication();
    digitalWrite(led_pin, LOW);
    digitalWrite(testpin, HIGH);
    ledstate=true;
    setWebPage(true);
    nocache();
    server.send(200, "text/html", webPage);
    sendEmail("LED On");
    Serial.println("LED On");
    delay(200);
   
  });
  server.on("/ledOff", [](){
    ConnectedIP=server.client().remoteIP().toString();
   Serial.print("Client Connected:");
   Serial.println(ConnectedIP);
    if(!checkAuth())
      return server.requestAuthentication();
    digitalWrite(led_pin, HIGH);
    digitalWrite(testpin, LOW);
    ledstate=false;
    setWebPage(true);
    nocache();
    server.send(200, "text/html", webPage);
    sendEmail("LED Off");
    Serial.println("LED Off");
    delay(200); 
  });
 server.on("/ledToggle", [](){
  ConnectedIP=server.client().remoteIP().toString();
   Serial.print("Client Connected:");
   Serial.println(ConnectedIP);
    if(!checkAuth())
      return server.requestAuthentication();
  
    if (ledstate == true){
      digitalWrite(led_pin, HIGH);
      digitalWrite(testpin, LOW);
      ledstate=false;
      setWebPage(true);
      nocache();
      server.send(200, "text/html", webPage);
      sendEmail("LED Off");
      Serial.println("LED Off");
    }else {
      digitalWrite(led_pin, LOW);
      digitalWrite(testpin, HIGH);
      ledstate=true;
      setWebPage(true);
      nocache();
      server.send(200, "text/html", webPage);
      sendEmail("LED On");
      Serial.println("LED On");   
    }
    delay(200); 
  });
  
server.on("/ledBounce", [](){
  ConnectedIP=server.client().remoteIP().toString();
   Serial.print("Client Connected:");
   Serial.println(ConnectedIP);
  if(!checkAuth())
      return server.requestAuthentication();
    if (ledstate == true){
      digitalWrite(led_pin, HIGH);
      digitalWrite(testpin, LOW);
      delay(500);
      digitalWrite(led_pin, LOW);
      digitalWrite(testpin, HIGH);
 
      setWebPage(true);
      nocache();
      server.send(200, "text/html", webPage);
      sendEmail("LED Bounce Off");
      Serial.println("LED Bounce Off");
    }else {
      digitalWrite(led_pin, LOW);
      digitalWrite(testpin, HIGH);
      delay(500);
      digitalWrite(led_pin, HIGH);
      digitalWrite(testpin, LOW);
      setWebPage(true);
      nocache();
      server.send(200, "text/html", webPage);
      sendEmail("LED Bounce On");
      Serial.println("LED Bounce On");   
    }
    delay(200); 

  });

  server.on("/switch1On", [](){
    processSwitch("Switch 1","on",gpio0_pin);
  });
  server.on("/switch1Off", [](){
    processSwitch("Switch 1","off",gpio0_pin);
  });
  server.on("/switch1Toggle", [](){
    processSwitch("Switch 1","toggle",gpio0_pin); 
  });
  server.on("/switch1Bounce", [](){
    processSwitch("Switch 1","bounce",gpio0_pin);
  });

  server.on("/switch2On", [](){
    processSwitch("Switch 2","on",gpio1_pin);
  });
  server.on("/switch2Off", [](){
    processSwitch("Switch 2","off",gpio1_pin);
  });
  server.on("/switch2Toggle", [](){
    processSwitch("Switch 2","toggle",gpio1_pin); 
  });
  server.on("/switch2Bounce", [](){
    processSwitch("Switch 2","bounce",gpio1_pin);
  });

#if defined(fourswitch) 

  server.on("/switch3On", [](){
    processSwitch("Switch 3","on",gpio2_pin);
  });
  server.on("/switch3Off", [](){
    processSwitch("Switch 3","off",gpio2_pin);
  });
  server.on("/switch3Toggle", [](){
    processSwitch("Switch 3","toggle",gpio2_pin); 
  });
  server.on("/switch3Bounce", [](){
    processSwitch("Switch 3","bounce",gpio2_pin);
  });

  server.on("/switch4On", [](){
    processSwitch("Switch 4","on",gpio3_pin);
  });
  server.on("/switch4Off", [](){
    processSwitch("Switch 4","off",gpio3_pin);
  });
  server.on("/switch4Toggle", [](){
    processSwitch("Switch 4","toggle",gpio3_pin); 
  });
  server.on("/switch4Bounce", [](){
    processSwitch("Switch 4","bounce",gpio3_pin);
  });



#endif


  
  server.begin();
  Serial.println("HTTP server started");
  
}



#if defined(OTA)


void initOTA () {

 //  Port defaults to 8266
  ArduinoOTA.setPort(8266);

 //Hostname defaults to esp8266-[ChipID]
   ArduinoOTA.setHostname("esp8266");

 //No authentication by default
  ArduinoOTA.setPassword((const char *)"124");
 

  ArduinoOTA.onStart([]() {
      Serial.println("Start");
   });
   ArduinoOTA.onEnd([]() {
     Serial.println("\nEnd");
   });
   ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
     Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
   });
   ArduinoOTA.onError([](ota_error_t error) {
     Serial.printf("Error[%u]: ", error);
     if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
     else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
     else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
     else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
     else if (error == OTA_END_ERROR) Serial.println("End Failed");
   });
  ArduinoOTA.begin();
}

#endif

void setup(void){
  
  Serial.begin(115200);
  initHardware();
  
#if defined(APONLY)
  Serial.println("AP Only Mode");
  initWifiAP();
#else
  Serial.println("Mixed Client/AP Mode");
  if (!initWifiClient()){
    initWifiAP();
  }
#endif
  
 
  delay(200);
  initHttpServer();
  
#if defined(OTA)
  initOTA();
#endif

turnOnRelay();


#if defined(POWERON)
  sendEmail("Device Power On");
#endif
  
  
 
}

void sendEmail(String message){

  if (!isWifiAP){
       if (allowSendEmail) {
#if defined (SMTP)
         message.toCharArray(EMAIL_CONTENT_SECOND, 16);
         sprintf(EMAIL_CONTENT_FINAL,"%s %s",EMAIL_CONTENT,EMAIL_CONTENT_SECOND);
         Serial.println(EMAIL_CONTENT_FINAL);
         send_flag = true;
#endif
#if defined (POSTMARK)

        sendPostmark(message);
#endif
       }
   }
}
void loop(void){

    #if defined(OTA)
      ArduinoOTA.handle();
    #endif
     
    //  Serial.print("Switch 1: [");
    //  Serial.print(switch1state);
    //  Serial.print("] Switch 2: [");
    //  Serial.print(switch2state);
    //  Serial.println("]");
      //Serial.println("HTTP waiting");
    
    if (!isWifiAP)
    {
#if defined (SMTP)
        if(send_flag){ // the send_flat is set, this means we are or need to start sending SMTP commands
           Serial.print("Running Command:[");
           Serial.print(count);
           Serial.println("]");
            if(do_next()){ // execute the next command
                Serial.println("Success");
                count++; // increment the count so that the next command will be executed next time.
            }
        } else {
           server.handleClient();
        }
#endif
#if defined (POSTMARK) 
      server.handleClient();
#endif
#if defined (NOEMMAIL)
      server.handleClient();
#endif
    } else {

       server.handleClient();

       
//    
//      WiFiClient client = server.available();
//  if (!client) {
//    return;
//  }

    }
    getDoorStatus();
} 

void wifisend(String command){
  Serial.print("Sending wifi:[");
  Serial.print(command);
  Serial.println("]");
  client.println(command);
}

String wifireceive(){
  String line = "";
  String response="";
  while (client.connected()) {
    line = client.readStringUntil('\n');
    response += line;
    #if defined(DEBUG)
    Serial.print("Line:[");
    Serial.print(line);
    Serial.println("]");
    #endif
    if (line == "") {
      break;
    }
  }
  return response;
}
  // do_next executes the SMTP command in the order required.
boolean wifisendrecv(String command, String response, int mydelay) {
  wifisend(command);
  delay(mydelay);
  String resp = wifireceive();
  Serial.print("Comparing Expected:[");
  Serial.print(response);
  Serial.print("] to: [");
  Serial.print(resp);
  Serial.print("[");
  return true;
  
}
#if defined (SMTP)
boolean do_next()
{
    boolean stat=false;
    String banner="";
    blink(2);
    switch(count){ 
    case 0:
        Serial.print("Connecting...");
        stat = client.connect(HOST, PORT);
        Serial.println(stat);
        banner = wifireceive();
        Serial.println(banner);
        return stat;
        break;
    case 1:
    Serial.print("Sendng Helo...");
        // send "HELO ip_address" command. Server will reply with "250" and welcome message
         stat =  wifisendrecv("EHLO FQDN HERE","250",500); // ideally an ipaddress should go in place 
        Serial.println(stat);
        return stat;
        break;
    case 2:
        // send "AUTH LOGIN" command to the server will reply with "334 username" base 64 encoded
        stat= wifisendrecv("AUTH LOGIN","334 B63USER",500);
        Serial.println(stat);
        return stat;
        break;
    case 3:
        // send username/email base 64 encoded, the server will reply with "334 password" base 64 encoded
        stat=wifisendrecv(EMAIL_FROM_BASE64,"334 B64PASS",500); 
        Serial.println(stat);
        return stat;
        break;
    case 4:
        // send password base 64 encoded, upon successful login the server will reply with 235.
        stat=wifisendrecv(EMAIL_PASSWORD_BASE64,"235 2.7.0 Authentication successful",3000);
        Serial.println(stat);
        return stat;
        break;
    case 5:{
        // send "MAIL FROM:<emali_from@domain.com>" command
        char mailFrom[50] = "MAIL FROM:<"; // If 50 is not long enough change it, do the same for the array in the other cases
        strcat(mailFrom,EMAIL_FROM);
        strcat(mailFrom,">");
        stat=wifisendrecv(mailFrom,"250 2.1.0 Ok",500);
        Serial.println(stat);
        return true;
        return stat;
        break;
    }
    case 6:{
        // send "RCPT TO:<email_to@domain.com>" command
        char rcptTo[50] = "RCPT TO:<";
        strcat(rcptTo,EMAIL_TO);
        strcat(rcptTo,">");
        stat=wifisendrecv(rcptTo,"250 2.1.5 Ok",500);
        Serial.println(stat);
        return true;
        return stat;
        break;
    }
    case 7:
        // Send "DATA"  command, the server will reply with something like "334 end message with \r\n.\r\n."
        stat=wifisendrecv("DATA","354 End data with <CR><LF>.<CR><LF>",500);
        Serial.println(stat);
        return true;
        break;
    case 8:{
        // apply "FROM: from_name <from_email@domain.com>" header
        char from[100] = "FROM: ";
        strcat(from,EMAIL_FROM);
        strcat(from," ");
        strcat(from,"<");
        strcat(from,EMAIL_FROM);
        strcat(from,">");  
        wifisend(from);
      
        return true;
        
        break;
    }
    case 9:{
        // apply TO header 
        char to[100] = "TO: ";
        strcat(to,EMAIL_TO);
        strcat(to,"<");
        strcat(to,EMAIL_TO);
        strcat(to,">");
      
        wifisend(to);
       
        return true;
        break;
    }
    case 10:{
        // apply SUBJECT header
        char subject[50] = "SUBJECT: Alert From ";
        strcat(subject, myhostname);
        wifisend(subject);
       
        return true;
        break;
    }
    case 11:
       
        wifisend("\r\n");
     
        return true;
        break;
    case 12:
       
        wifisend(EMAIL_CONTENT_FINAL);
       
        return true;
        break;
    case 13:
        stat=wifisendrecv("\r\n.\r\n","250 2.0.0 Ok: queued as ",500);
        Serial.println(stat);
        return true;
        break;
    case 14:
  
        stat=wifisendrecv("QUIT","221 2.0.0 Bye",500);
        Serial.println(stat);
        return true;
        break;
    case 15:
        client.stop();
        return true;
        break;
    case 16:
        Serial.println("Done");
        send_flag = false;
        count = 0;
        return false; // we don't want to increment the count
        break;
    default:
        return true;
        break;
        }
}
#endif
#if defined (POSTMARK)
void sendPostmark(String command){
//curl "https://api.postmarkapp.com/email" \
//  -X POST \
//  -H "Accept: application/json" \
//  -H "Content-Type: application/json" \
//  -H "X-Postmark-Server-Token: YOUR TOKEN HERE" \
//  -d "{From: 'sender@domain.com', To: 'receiver@domain.com', Subject: 'Hello from Postmark', HtmlBody: '<strong>Hello</strong> dear Postmark user.'}"
   #if defined(DEBUG)
  Serial.print("Connecting... Host:[");
  Serial.print(HOST);
  Serial.print("] Port:[");
   Serial.print(PORT);
  Serial.println("]");
  #endif
  boolean stat = client.connect(HOST, PORT);
   #if defined(DEBUG)
  Serial.println(stat);
  Serial.println("Sending...");
   #endif
  String PostData="{From: 'sender@domain.com', To: 'receiver@domain.com', Subject: '";
  PostData+="Alert From ";
  PostData+=myhostname;
  PostData+="', HtmlBody: '";
  PostData+="<strong>User:</strong>";
  PostData+=AuthedUser;
  PostData+="<br>";
  PostData+="<strong>Command:</strong>";
  PostData+=command;
  PostData+="<br>";
  PostData+="<strong>From:</strong>";
  PostData+=ConnectedIP;

  
  PostData+="'}";
   #if defined(DEBUG)
  Serial.println(PostData);
  #endif
  if (stat) {
    Serial.println("connected");
    client.println("POST /email HTTP/1.1");
    client.println("Host:  api.postmarkapp.com");
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Accept: application/json");
    client.println("Content-Type: application/json");
    client.println("X-Postmark-Server-Token: YOUR TOKEN HERE");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);
    String banner = wifireceive();
     #if defined(DEBUG)
    Serial.println("Post Response:");
    Serial.println(banner);
    Serial.println("--");
    #endif
     Serial.println("Postmark Sent.");
  } else {
  Serial.println("Unable to connect to postmark:");
    
  }
}

#endif
