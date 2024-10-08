/*
NOME DO PROJETO: NurseCallSystem - Pacient
AUTOR: Leonardo Severino - leoseverino0901@gmail.com
DATA: 25/07/2024 - 18:37:00
DESCRIÇÃO:
  Programa em Arduino que Realiza e registra chamadas de pacientes.Ele lê o MAC address do ESP32, conecta-se a uma rede Wi-Fi e a um servidor para enviar os dados do paciente que acionou o chamado.As informações incluem o endereço MAC do ESP32, o número do paciente e o
timestamp do chamado. Além disso, o código utiliza o sistema de arquivos SPIFFS para armazenar logs localmente e tem a capacidade de formatar o sistema de arquivos. Os botões no ESP32 são utilizados para registrar diferentes chamados e formatar o armazenamento.
*/

//Inclusão das bibliotecas
#include <WiFi.h>
#include <WiFiClient.h>
#include <time.h>
#include <esp_wifi.h>
#include <SPIFFS.h>
#include <FS.h>

//Pinos dos botões
#define BUTTON1 5 //Pino do botão 1
#define BUTTON2 18 //Pino do botão 2
#define BUTTON3 19 //Pino do botão 3
#define BUTTON4 21 //Pino do botão 4(RESET)

//Protótipo das funções
int ButtonPressed();
String GetTime();
void ReadMACaddress();
bool SendWifiData();
String MACtoString(byte ar[]);
bool ReadFile(String path);
bool WriteFile(String path, String message);
bool AppendFile(String path, String message);
bool FormatSPIFFS();

//Dados que serão recebidos/enviados para o servidor/Salvas em arquivo
int pacient; //Número correspondente ao paciente que realizou o chamado
String timestamp; //Data e Horário do chamado no formato DD-MM-YY HH:MM:SS
String MACserver; //Endereço MAC do servidor
uint8_t MACaddress[6]; //Endereço MAC do esp32 no formato AA:BB:CC:DD:EE:FF

//Estados dos botões
int button1LastState, button1State; //Estado inicial e atual do botão 1
int button2LastState, button2State; //Estado inicial e atual do botão 2
int button3LastState, button3State; //Estado inicial e atual do botão 3
int button4LastState, button4State; //Estado inicial e atual do botão 4

//Informacoes de DATA e HORA
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800;
const int   daylightOffset_sec = 0;

//Credenciais da rede
const char* ssid  = ""; //Nome da rede WiFi
const char* password  = ""; //Senha da rede WiFi

//Credenciais do servidor
const char* server_ip  = ""; //Endereço IP do servidor
const int server_port = 80; //Porta do servidor

WiFiClient server; //Cliente WiFi

void setup(){
  Serial.begin(115200); //Inicialização da serial
  delay(1000); //Delay de 1s
  Serial.println();
  //Habilita o pull-up interno no pino dos botões
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);
  //Inicializa estado atual dos botões
  button1State = digitalRead(BUTTON1);
  button2State = digitalRead(BUTTON2);
  button3State = digitalRead(BUTTON3);
  button4State = digitalRead(BUTTON4);
  //Inicializa variavéis
  pacient = 0;
  timestamp = MACserver = "";
  //Incializa DATA e HORA
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
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
  //Inicialização da partição SPIFFS sem formatar
  if(SPIFFS.begin(false)){ //Se a inicialização ocorreu com sucesso
    Serial.println("SPIFFS initialized!");
  }
  else{ //Senão
    Serial.println("Fail do initialize SPIFFS");
    while (1); //Loop infinito
  }
}

void loop(){
  int button; //Botão pressionado pelo usuário
  String path; //Caminho do arquivo a ser gravado
  String message; //Texto a ser gravado no arquivo

  path = "/log.txt"; //Define caminho do arquivo
  button = ButtonPressed(); //Verifica se houve um chamado, registrando em 'pacient' o número correspondente ao paciente que realizou o chamado
  if(button == 1 || button == 2 || button == 3){ //Se houve uma chamada
    timestamp = GetTime(); //Registra em 'timestamp' a data e horário do chamado
    pacient = button; //Registra o paciente que realizou a chamada
    //Envia dados via wifi
    if(SendWifiData()){ //Se o envio ocorreu com sucesso
      //Salva o chamado no arquivo 'log.txt'
      if(!SPIFFS.exists(path)){ //Se o arquivo ainda não existir
        message = "TIMESTAMP: " + timestamp + '\n' +
                  "SERVER MACaddress: " + MACserver + '\n' +
                  "CLIENT MACaddress: " + MACtoString((byte*) &MACaddress) + '\n' +
                  "PACIENT: " + String(pacient) + '\n' +
                  "\r---------------------------------------\n"; //Registra os dados do chamado em 'message'
        //Escreve o arquivo
        if(WriteFile(path, message)){ //Se a escrita ocorrer com sucesso
          Serial.println("------------------LOG------------------");
          if(ReadFile(path)){ //Mostra na Serial o conteúdo do arquivo
            Serial.println();
          }
        }
      }
      else{ //Se o arquivo já existir
        message = "TIMESTAMP: " + timestamp + '\n' +
                  "SERVER MACaddress: " + MACserver + '\n' +
                  "CLIENT MACaddress: " + MACtoString((byte*) &MACaddress) + '\n' +
                  "PACIENT: " + String(pacient) + '\n' +
                  "\r---------------------------------------\n"; //Registra os dados do chamado em 'message'
        //Anexa conteúdo ao arquivo
        if(AppendFile(path, message)){ //Se a anexação ocorreu com sucesso
          Serial.println("--------------------LOG-------------------");
          if(ReadFile(path)){ //Mostra na Serial o conteúdo do arquivo
            Serial.println();
          }
        }
      }
    }
  }
  else if (button == 4){ //Se houve solicitação para formatar a partição SPIFFS
    //Formata a partição SPIFFS
    if(FormatSPIFFS()){ //Se a formatação ocorrer com sucesso
      Serial.println("SPIFFS cleared");
    }
  }
  else{ //Senão
    delay(500); //Delay de 0.5s
  }
}

int ButtonPressed(){ //Função para verificar se um botão foi pressionado
  button1LastState = button1State; //Define o estado inicial do botão 1
  button2LastState = button2State; //Define o estado inicial do botão 2
  button3LastState = button3State; //Define o estado inicial do botão 3
  button4LastState = button4State; //Define o estado inicial do botão 4
  button1State = digitalRead(BUTTON1); //Atualiza o estado atual do botão 1
  button2State = digitalRead(BUTTON2); //Atualiza o estado atual do botão 2
  button3State = digitalRead(BUTTON3); //Atualiza o estado atual do botão 3
  button4State = digitalRead(BUTTON4); //Atualiza o estado atual do botão 4
  if((button1LastState == HIGH and button1State == LOW) xor (button2LastState == HIGH and button2State == LOW) xor (button3LastState == HIGH and button3State == LOW) xor (button4LastState == HIGH and button4State == LOW)){ //Verifica se um botão foi pressionado
    if(button1State == LOW){ //Se o botão 1 foi pressionado
      return 1; //Retorna 1
    }
    else if(button2State == LOW){ //Se o botão 2 foi pressionado
      return 2; //Retorna 2
    }
    else if(button3State == LOW){ //Se o botão 3 foi pressionado
      return 3; //Retorna 3
    }
    else{ //Se o botão 4 foi pressionado
      return 4; //Retorna 4
    }
  }
  else { //Se nenhum botão foi pressionado
    return 0; //Retorna 0
  }
}

String GetTime(){ //Função para ler data e hora atual
  struct tm timeinfo; //Informaçãoes de data e hora
  char local_time[20]; //buffer de dados

  //Registra DATA e HORA do chamado
  getLocalTime(&timeinfo);
  sprintf(local_time, "%02d-%02d-%02d %02d:%02d:%02d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year - 100, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec); //Registra DATA e HORA no formato DD-MM-YY HH:MM:SS na variavél 'local_time'

  return String(local_time); //Retorna data e hora do chamado no formato String
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

bool SendWifiData(){ //Função para enviar dados via WIFI
  String data; //Buffer de dados

  //Conexão ao servidor
  if(!server.connected()){ //Se não estava conectado ao servidor
    Serial.print("Connecting to ");
    Serial.println(server_ip);
    //Conecta-se ao servidor
    if(server.connect(server_ip, server_port)){ //Se conseguiu se conectar ao servidor
      Serial.print("Connected to ");
      Serial.println(server_ip);
      data = timestamp + ',' + MACtoString((byte*) &MACaddress) + ',' + String(pacient); //Dados que serão enviados ao servidor
      Serial.println("Sending data");
      server.println(data); //Envia dados para o sevidor
      //Aguarda resposta do servidor
      while(server.connected()){ //Enquanto estiver conectado ao servidor
        if(server.available()){ //Se o servidor estiver disponível
          MACserver = server.readStringUntil('\n'); //Lê o endereço MAC do servidor e registra em 'MACserver'
          Serial.print("DATA RECEIVED: ");
          Serial.println(MACserver); //Exibe o endereço MAC do servidor no monitor serial
          Serial.println(server.readStringUntil('\r')); //Exibe dados recebidos do servidor no monitor serial
          break; //Saí do loop de repetição
        }
      }
      //Fecha a conexão com o servidor
      server.stop();
      Serial.println("Connection to server closed");
    }
    else{ //Se a conexão falhou
      Serial.println("Failed to Connect to Server");
      return false; //Retorna FALSE
    }
  }
  delay(1000); //Delay de 1s

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

bool ReadFile(String path){ //Função para ler conteúdo de um arquivo
  File file; //Arquivo a ser lido

  file = SPIFFS.open(path); //Abre o caminho do arquivo da SPIFFS e passa o retorno para 'file'
  if(!file){ //Se houver falha ao abrir o caminho
    return false; //Retorna FALSE
  }
  else{ //Senão
    while(file.available()){ //Enquanto houver algum byte disponível para leitura de um arquivo
      Serial.write(file.read()); //Escreve o conteúdo do arquivo no monitor serial
    }
    file.close(); //Fecha o arquivo
  }

  return true; //Retorna TRUE
}

bool WriteFile(String path, String message){ //Função para criar e escrever conteúdo em um arquivo
  File file; //Arquivo a ser criado

  file = SPIFFS.open(path, FILE_WRITE); //Abre o arquivo, no modo escrita, onde será gravado o conteúdo, e passa o retorno para 'file'
  if(!file){ //Se houver falha ao abrir o caminho
    return false; //Retorna FALSE
  }
  else{ //Senão
    if(!file.print(message)){ //Se a criação e escrita do conteúdo no arquivo falhar
      return false; //Retorna FALSE
    }
    else{ //Senão
      Serial.println("Data written successfully");
    }
  }
  file.close(); //Fecha o arquivo

  return true; //Retorna TRUE
}

bool AppendFile(String path, String message){ //Função para anexar conteúdo a um arquivo
  File file; //Arquivo ao qual será anexado o conteúdo

  file = SPIFFS.open(path, FILE_APPEND); //Abre o arquivo, no modo anexar, onde será adicionado conteúdo, e passa o retorno para 'file'
  if(!file){ //Se houver falha ao abrir o caminho
    return false; //Retorna FALSE
  }
  else{ //Senão
    if(!file.print(message)){ //Se a anexação do conteúdo ao arquivo der errado
      return false; //Retorna FALSE
    }
    else{ //Senão
      Serial.println("Data written successfully");
    }
  }
  file.close(); //Fecha o arquivo

  return true; //Retorna TRUE
}

bool FormatSPIFFS(){ //Função para formatar o sistema de arquivos SPIFFS
  if(!SPIFFS.format()){ //Se a formatação falhar
    return false; //Retorna FALSE
  }

  return true; //Retorna TRUE
}
