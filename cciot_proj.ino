#include "ThingSpeak.h"
#include <WiFi.h>

#include <Arduino.h>
#include "ESP32_MailClient.h"
#include "SD.h"


  
//----------- Enter you Wi-Fi Details---------//
char ssid[] = "vivo 1802";
char pass[] = "raja1234" ;// Password
//-------------------------------------------//
#define emailSenderAccount    "rajamk.vechalapu@gmail.com"    
#define emailSenderPassword   "Raja.v@123"
#define emailRecipient        "rajamkv123@gmail.com"
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort        465
#define emailSubject          "ESP32 Test"

// The Email Sending data object contains config and data to send
SMTPData smtpData;

// Callback function to get the Email sending status
void sendCallback(SendStatus info);
int redLed = 19;
int greenLed = 11;
int smokeA0 = 25;
// Your threshold value
int sensorThres = 4000;
int danger=0;

WiFiClient  client;

unsigned long myChannelField = 920842; // Channel ID
const int ChannelField = 1; // Which channel to write data
const char * myWriteAPIKey = "IAUGY9ZYMNYD4ZZA"; // Your write API Key

void setup()
{
   pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(smokeA0, INPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
   WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println("Setup done");
  ThingSpeak.begin(client);
}
void sendCallback(SendStatus msg)
{
  //Print the current status
  Serial.println(msg.info());

  //Do something when complete
  if (msg.success())
  {
    Serial.println("----------------");
  }
}
void loop()
{
 
   int Sensedvalue = analogRead(smokeA0);

  Serial.print("Pin A0: ");
  Serial.println(Sensedvalue);
  // Checks if it has reached the threshold value
  if (Sensedvalue > sensorThres)
  {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    if(danger==0)
    {
     Serial.println("Preparing to send email");
  Serial.println();
  
  // Set the SMTP Server Email host, port, account and password
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);

  // For library version 1.2.0 and later which STARTTLS protocol was supported,the STARTTLS will be 
  // enabled automatically when port 587 was used, or enable it manually using setSTARTTLS function.
  //smtpData.setSTARTTLS(true);

  // Set the sender name and Email
  smtpData.setSender("ESP32", emailSenderAccount);

  // Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
  smtpData.setPriority("High");

  // Set the subject
  smtpData.setSubject(emailSubject);

  // Set the message with HTML format
  smtpData.setMessage("<div style=\"color:#2f4468;\"><h1>Hello World!</h1><p>- Sent from ESP32 board</p></div>", true);
  // Set the email message in text format (raw)
  //smtpData.setMessage("Hello World! - Sent from ESP32 board", false);

  // Add recipients, you can add more than one recipient
  smtpData.addRecipient(emailRecipient);
  //smtpData.addRecipient("YOUR_OTHER_RECIPIENT_EMAIL_ADDRESS@EXAMPLE.com");

  smtpData.setSendCallback(sendCallback);

  //Start sending Email, can be set callback function to track the status
  if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());

  //Clear all data from Email object to free memory
  smtpData.empty();
  danger=1;
  }
  }
  else
  {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
  if( danger==1)
  {
    danger=0;
    }
  }
  delay(100);
  
  ThingSpeak.writeField(myChannelField, ChannelField,Sensedvalue , myWriteAPIKey);
  delay(1000);
}
