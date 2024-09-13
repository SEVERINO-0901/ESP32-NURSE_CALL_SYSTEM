/*
NOME DO PROJETO: NurseCallSystem - Server
AUTOR: Leonardo Severino - leoseverino0901@gmail.com  
DATA: 31/07/2024 - 15:26:00
DESCRIÇÃO:
  Recebe e processa as chamadas de paciente, conectando-se a uma rede WiFi e iniciando um servidor na porta 80. Quando um cliente se conecta, ele registra os dados recebidos de timestamp, endereço MAC do cliente e ID do paciente que realizou a chamada, 
retornando em resposta ao cliente o seu próprio endereço MAC no formato de string. Por fim, ele acende o LED correspondente ao paciente que acionou o chamado, e desliga todos os LEDs ao pressionar o botão de desligar.
*/

//Inclusão das bibliotecas
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <esp_wifi.h>

//Pinos da placa
#define BUTTON 13 //Pino do botão de desligar
#define LED1 18 //Pino do LED 1
#define LED2 19 //Pino do LED 2
#define LED3 21 //Pino do LED 3

//Protótipo das funções
void ReadMACaddress();
bool ReadWifiData();
String MACtoString(byte ar[]);
void GetData(String data);
void TurnOnLed(int led);
bool ButtonPressed();
void TurnOffLeds();

//Estados do botão
int buttonLastState, buttonState; //Estado inicial e atual

//Dados que serão recebidos/enviados ao cliente
int pacient; //Número correspondente ao paciente que realizou o chamado
String data; //Buffer de dados
String timestamp; //Data e Horário do chamado no formato DD-MM-YY HH:MM:SS
String MACclient; //Endereço MAC do cliente
uint8_t MACaddress[6]; //Endereço MAC do esp32 no formato AA:BB:CC:DD:EE:FF 

//Credenciais da rede
const char* ssid  = "SEVERINO_01"; //Nome da rede WiFi
const char* password  = "a67a70l00"; //Senha da rede WiFi

WiFiServer server(80); //Servidor WiFi, inicializado na porta 80

void setup(){
  Serial.begin(115200); //Inicialização da serial
  delay(1000); //Delay de 1s
  Serial.println();
  pinMode(BUTTON, INPUT_PULLUP); //Habilita o pull-up interno no pino do botão
  buttonState = digitalRead(BUTTON); //Inicializa estado atual do botão
  //Incializa LEDs como saídas
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  //Inicializa variavéis
  pacient = 0;
  data = timestamp = MACclient = "";
  //Leitura do MAC address do esp32
  WiFi.mode(WIFI_STA); //Coloca o WiFi no modo Station
  WiFi.STA.begin();
  Serial.print("[DEFAULT]ESP32 Board MAC Address: ");
  ReadMACaddress(); //Lê o endereço MAC
  //Conexão na rede WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); //Inicia a conexão WiFi na rede 'ssid' com a senha 'password'
  while(WiFi.status() != WL_CONNECTED){ //Enquanto não conecta na rede
    Serial.print(".");
    delay(500); //Delay de 0.5s
  }
  Serial.println();
  Serial.println("WiFi connected.");
  //Imprime informacoes da rede
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin(); //Inicia o servidor
  Serial.println("Server Initiated!");
  
}

void loop(){
  //Verifica se recebeu dados 
  if(ReadWifiData()){ //Se recebeu dados do cliente 
    GetData(data); //Separa dados
    TurnOnLed(pacient); //Acende o LED correspondente ao paciente que realizou a chamada 
    //Imprime os dados no monitor serial
    Serial.print("TIMESTAMP: ");
    Serial.println(timestamp);
    Serial.print("CLIENT MACaddress: ");
    Serial.println(MACclient);
    Serial.print("PACIENT: ");
    Serial.println(pacient);        
  }
  else if(ButtonPressed()){ //Se o botão foi pressionado
    Serial.println("Turning LEDS OFF");
    TurnOffLeds(); //Desliga os LEDs
    data = " "; //Reseta 'data'  
  }
  else{ //Senão
    delay(500); //Delay de 0.5s 
  }
}

void ReadMACaddress(){ //Função para ler o endereço MAC do esp32
  esp_err_t ret; //Indicador da leitura do endereço MAC
  
  ret = esp_wifi_get_mac(WIFI_IF_STA, MACaddress); //Realiza a leitura do endereço MAC
  if(ret == ESP_OK){ //Se a leitura ocorreu com sucesso
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n", MACaddress[0], MACaddress[1], MACaddress[2], MACaddress[3], MACaddress[4], MACaddress[5]); //Exibe o endereço MAC no monitor serial
  }
  else{ //Senão
    Serial.println("Failed to read MAC address");
  }    
}

bool ReadWifiData(){ //Função para receber dados via WIFI
  WiFiClient client; //Cliente WiFi
  
  client = server.available(); //Verifica se há clientes
  if(client){ //Se um cliente se conectou
    Serial.println("New client connected");
    //Lê dados do cliente
    while(client.connected()){ //Enquanto o cliente está conectado
      if(client.connected()){ //Se o cliente ainda estiver conectado
        data = client.readStringUntil('\r'); //Realiza a leitura dos dados recebidos do cliente através de 'data'
        Serial.print("DATA RECEIVED: ");
        Serial.println(data); //Imprime os dados recebidos no monitor serial
        client.println(MACtoString((byte*) &MACaddress)); //Envia o endereço MAC do esp32 para o cliente
        client.println("Data received successfully");
        break; //Saí do loop      
      }
      else{ //Senão
        delay(1); //Delay de 1ms    
      }
    }
    delay(10); //Delay de 10ms
    client.stop(); //Fecha a conexão com o cliente
    Serial.println("Client disconnected");
  }
  else{ //Senão
    return false; //Retorna FALSE  
  }

  return true; //Retorna TRUE
}

String MACtoString(byte ar[]){ //Função para converter o endereço MAC para o formato de String, para ser enviado ao cliente
  String s; //Endereço MAC em formato de String
  char buf[3]; //buffer de dados
  
  for(byte i = 0; i < 6; ++i){ //Laço de repetição
    sprintf(buf, "%02X", ar[i]); //Converte byte para char 
    s += buf; //Acrescenta o char à String
    if(i < 5){ //Se Terminou de receber o byte  
      s += ':'; //Acrescenta ':'
    }
  }
  
  return s; //Retorna endereço MAC do esp32 em formato de String 
}

void GetData(String data){ //Função para separar a String recebida em dados individuais
  int commaIndex; //Índice da vírgula 

  commaIndex = data.indexOf(','); //Índice da primeira vírgula
  timestamp = data.substring(0, commaIndex); //Registra a data e horário do chamado em 'timestamp'
  data = data.substring(commaIndex + 1); //Índice da segunda vírgula
  MACclient = data.substring(0, commaIndex); //Registra o endereço MAC em 'MACclient'
  data = data.substring(commaIndex + 1); //Índice da terceira vírgula
  pacient = data.toInt(); //Registra o número correspondente ao paciente que realizou o chamado em 'pacient'
}

void TurnOnLed(int led){ //Função para ligar os LEDs
  if(led == 1){ //Se foi o paciente 1
    digitalWrite(LED1, HIGH); //Acende LED 1    
  }
  else if(led == 2){ //Se foi o paciente 2
    digitalWrite(LED2, HIGH); //Acende LED 2    
  }
  else if(led == 3){ //Se foi o paciente 3
    digitalWrite(LED3, HIGH); //Acende LED 3   
  }
}

bool ButtonPressed(){ //Função para verificar se o botão foi pressionado
  buttonLastState = buttonState; //Define o estado inicial do botão
  buttonState = digitalRead(BUTTON); //Atualiza o estado final do botão
  if(buttonLastState == HIGH and buttonState == LOW){ //Verifica se o botão foi pressionado
    return true; //Retorna TRUE
  }
  else{ //Se o botão não foi pressionado
    return false; //Retorna FALSE
  }
}

void TurnOffLeds(){ //Função para desligar os leds
  digitalWrite(LED1, LOW); //Desliga LED 1    
  digitalWrite(LED2, LOW); //Desliga LED 2
  digitalWrite(LED3, LOW); //Desliga LED 3    
}
