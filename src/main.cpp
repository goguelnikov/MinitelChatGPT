/*******************************************************************
    A ChatGPT terminal for Minitel ESP32 ;-)

    Minitel ESP32 by iodeo
    https://hackaday.io/project/180473-minitel-esp32
    https://www.tindie.com/products/iodeo/minitel-esp32-dev-board/?utm_source=hackadayio&utm_medium=link&utm_campaign=project_buynow
    github: https://github.com/iodeo/Minitel-ESP32 
    
    Minitel library by eserandour
    github: https://github.com/eserandour/Minitel1B_Hard
    
    Project ChatGPT Client For ESP32 by Eric Nam
    github: https://github.com/0015/ChatGPT_Client_For_Arduino

 *******************************************************************/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ChatGPT.hpp> // see https://github.com/0015/ChatGPT_Client_For_Arduino
#include <Minitel1B_Hard.h>  // see https://github.com/eserandour/Minitel1B_Hard

// init Minitel ESP32
Minitel minitel(Serial2);    // second serial port (RXD1 TXD1).
#define TITRE "3615 ChatGPT"
String texte = "";
int nbCaracteres = 0;
const int PREMIERE_LIGNE = 2;
const int NB_LIGNES = 4;
const String VIDE = ".";
unsigned long touche;
String intro = "Moi - ";

// Wifi network station credentials
#define WIFI_SSID "XXXXXXXXXXXXXX"
#define WIFI_PASSWORD "XXXXXXXXXXXXXX"

// ChatGPT Token (Get from https://platform.openai.com/account/api-keys)
#define GPT_TOKEN "sk-XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

WiFiClientSecure client;

// ChatGPT client
ChatGPT<WiFiClientSecure> chat_gpt(&client, "v1", GPT_TOKEN);
String result;

void champVide(int premiereLigne, int nbLignes)
{
  minitel.noCursor();

  //minitel.moveCursorDown(1);
  //minitel.moveCursorXY(1,premiereLigne);
  //minitel.clearScreenFromCursor();

  for (int j=0; j<nbLignes; j++) {
    minitel.attributs(CARACTERE_BLEU);
    minitel.print(VIDE);
    minitel.repeat(39);
    minitel.attributs(CARACTERE_BLANC);
  }

  for (int j=0; j<nbLignes; j++) {
    minitel.moveCursorUp(1);
  }

  //texte="";
  //nbCaracteres=0;

  //minitel.moveCursorXY(31,24);
  //minitel.print("puis ");
  //minitel.attributs(INVERSION_FOND);
  //minitel.print("ENVOI");
  //minitel.attributs(FOND_NORMAL);
  //minitel.moveCursorDown(premiereLigne-1);
  //minitel.moveCursorXY(1,premiereLigne);
  minitel.cursor();

}

void newPage(String titre) {
  minitel.newScreen();
  minitel.attributs(INVERSION_FOND);
  minitel.attributs(CARACTERE_BLEU);
  //minitel.attributs(CARACTERE_ROUGE);  
  for (int i=0; i<40; i++) {
    minitel.print(" ");
  }
  for (int i=0; i<14; i++) {
    minitel.print(" ");
  }
  minitel.print(titre);
  for (int i=0; i<14; i++) {
    minitel.print(" ");
  }
  for (int i=0; i<40; i++) {
    minitel.print(" ");
  }
  //minitel.println(" ");
  minitel.attributs(CARACTERE_BLANC);
  minitel.attributs(FOND_NORMAL);
  minitel.println();
  //for (int i=1; i<=40; i++) {
  //  minitel.writeByte(0x7E);
  //}
}

void eraseLines(int nbLine)
{
  for (int j=0; j<nbCaracteres/40; j++) {
    minitel.moveCursorUp(1);
  }
  for (int j=0; j<nbCaracteres; j++) {
    minitel.moveCursorLeft(1);
  }

  for (int j=0; j<nbLine; j++) {
    //minitel.attributs(CARACTERE_BLEU);
    minitel.print(" ");
    minitel.repeat(39);
    //minitel.attributs(CARACTERE_BLANC);
  }
}

void displayResponse(String response) {
  //Serial.println(F("handleNewMessages"));
  Serial.print(F("Response "));
  Serial.println(response);
  //eraseLines(text.length()/40);
  minitel.attributs(INVERSION_FOND);
  //minitel.attributs(CARACTERE_BLEU);
  minitel.attributs(CARACTERE_ROUGE);
    
  //minitel.print(from_name);
  //minitel.print(" - ");
  //minitel.println(text);
  minitel.println();
  minitel.print("GPT - ");  
  minitel.print(response);
  minitel.attributs(CARACTERE_BLANC);
  minitel.attributs(FOND_NORMAL);
  //minitel.bip();
  //champVide(PREMIERE_LIGNE, NB_LIGNES);
  //minitel.print(intro);
  //minitel.print(texte);
  //minitel.moveCursorXY(1,10);
  //minitel.moveCursorReturn(1);
 
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  // Ignore SSL certificate validation
  client.setInsecure();

  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  Serial.print("Initialising Minitel...");  
  minitel.changeSpeed(minitel.searchSpeed());
  //minitel.modeMixte();
  minitel.smallMode();
  minitel.scrollMode();
  newPage(TITRE);
  champVide(PREMIERE_LIGNE, NB_LIGNES);
  Serial.println(" done"); 
  //Serial.print("Mintel type: "); 
  //Serial.println(minitel.identifyDevice()); 
  minitel.print(intro);
}

void loop()
{
  // Minitel Part
    // reading key
    
    touche = minitel.getKeyCode();

    if ((touche != 0) &&
        (touche != CONNEXION_FIN) &&
        (touche != SOMMAIRE) &&
        (touche != ANNULATION) &&
        (touche != RETOUR) &&
        (touche != REPETITION) &&
        (touche != GUIDE) &&
        (touche != CORRECTION) &&
        (touche != SUITE) &&
        (touche != ENVOI)) {

      if (nbCaracteres < 40 * NB_LIGNES) {
        nbCaracteres++;
        texte += char(touche);
      }

      if (nbCaracteres == 40 * NB_LIGNES) {
        minitel.moveCursorXY(40,(PREMIERE_LIGNE - 1) + NB_LIGNES);
      }

      //Serial.print("Current text: ");
      //Serial.println(texte);
      //Serial.print("nb Car: ");
      //Serial.println(nbCaracteres);

    }

    if (touche == ENVOI) {
          // send request
          Serial.println("[ChatGPT] Only print a content message");
          if (chat_gpt.simple_message("gpt-3.5-turbo-0301", "user", texte, result)) {
            Serial.println("===OK===");
            Serial.println(result);
          } else {
            Serial.println("===ERROR===");
            Serial.println(result);
          }
        displayResponse(result);
        //minitel.print("ChatGPT - ");  
        //minitel.print(result);
        texte = "";
        nbCaracteres = 0;

        //minitel.moveCursorXY(1,10);
        champVide(PREMIERE_LIGNE, NB_LIGNES);
        minitel.moveCursorReturn(1);
        minitel.print(intro);
    }

    if (touche == CORRECTION) {
      if ((nbCaracteres > 0) && (nbCaracteres <= 40 * NB_LIGNES)) {
        if (nbCaracteres != 40 * NB_LIGNES) { 
          minitel.moveCursorLeft(1); 
        }
        minitel.attributs(CARACTERE_BLEU);
        minitel.print(VIDE);
        minitel.attributs(CARACTERE_BLANC);
        minitel.moveCursorLeft(1);
        if (nbCaracteres == 40*(NB_LIGNES-1)+1) { 
          minitel.moveCursorLeft(1); 
          //currRow--;
        }
        texte = texte.substring(0,texte.length()-1);
        nbCaracteres--;
      }
    }
    // switch (touche) {
    //   case ENVOI :
    //     // send message
    //     break;
    //   case CORRECTION :

    //     break;
    //   case SOMMAIRE : 
    //     break;
    //   case SUITE : 
    //     break;
    //   case ANNULATION :
    //     //newPage(TITRE);
    //     //champVide(PREMIERE_LIGNE, NB_LIGNES);
    //     break;
    //   case GUIDE :
    //     break;
    //   case REPETITION :
    //     break;
    // }    


  // Message de fin
  //minitel.textMode();
  //minitel.attributs(CARACTERE_NOIR);
  //minitel.moveCursorXY(7,23);
  //minitel.print("Merci de votre participation");
  //minitel.noCursor();
  //delay(4000);

}

