#include <ESP8266WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <MFRC522.h>

//pin connecction configuration

#define SS_PIN D4
#define RST_PIN D3

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

char ssid[] = "Wifi_Name";// Network Name
char pass[] = "Wifi_Password"; // Network Password
byte mac[6];

String code;
int x=0;

WiFiClient client;
MySQL_Connection conn((Client *)&client);

IPAddress server_addr(a,b,c,d); // MySQL server IP
char user[] = "server_login"; // MySQL user
char password[] = "server_login"; // MySQL password

//query instruction

//const char QUERY_POP[] = "SELECT card_no FROM arduino.entry WHERE card_no='content';";
char query[128];

void setup() {

//setup of mfrc522
Serial.begin(9600); // Initiate a serial communication
SPI.begin(); // Initiate SPI bus
mfrc522.PCD_Init(); // Initiate MFRC522
Serial.println("Approximate your card to the reader...");
Serial.println();
Serial.println("");
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, pass);

while (WiFi.status() != WL_CONNECTED) {
delay(200);
Serial.print(".");
}

Serial.println("");
Serial.println("WiFi Connected");


}

void loop() {

  while (!conn.connected()) {
      connectDB();
    }

// Look for new cards
if ( ! mfrc522.PICC_IsNewCardPresent())
{
return;
}
// Select one of the cards
if ( ! mfrc522.PICC_ReadCardSerial())
{
return;
}
//Show UID on serial monitor
Serial.print("UID tag :");
String content= "";
byte letter;
for (byte i = 0; i < mfrc522.uid.size; i++)
{
Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
Serial.print(mfrc522.uid.uidByte[i], HEX);
content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
content.concat(String(mfrc522.uid.uidByte[i], HEX));
}
Serial.println();
Serial.print("Message : ");
content.toUpperCase();
code=content;
//String QUERY_POP = "INSERT INTO arduino.entry (user,card_no,control) VALUES ('Rahul','" + code + "','admin');";
String QUERY_POP = "SELECT card_no FROM arduino.entry WHERE card_no='" + code + "';";

QUERY_POP.toCharArray(query, 100);

// Initiate the query class instance
MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
// Supply the parameter for the query
// Here we use the QUERY_POP as the format string and query as the
// destination. This uses twice the memory so another option would be
// to allocate one buffer for all formatted queries or allocate the
// memory as needed (just make sure you allocate enough memory and
// free it when you're done!).


// Execute the query
cur_mem->execute(query);
// Fetch the columns and print them
cur_mem->get_columns();


Serial.println();
// Read the rows and print them
row_values *row = NULL;
row = cur_mem->get_next_row();
if (row != NULL) {
  Serial.println("Access Granted");
  delay(5000);
}
else{
  Serial.println("Access Denied");
}
Serial.println();

// Deleting the cursor also frees up memory used
delete cur_mem;



}
void connectDB(){
  Serial.println("Connecting to database");

  while (conn.connect(server_addr, 3306, user, password) != true) {
  delay(200);
  Serial.print ( "." );
  }
  
  Serial.println("");
  Serial.println("Connected to SQL Server!");
}

