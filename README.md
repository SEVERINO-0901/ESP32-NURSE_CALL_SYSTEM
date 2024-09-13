## DADOS DO PROJETO
- **Nome:** NurseCallSystem; 
- **Autor:** Leonardo Severino - leoseverino0901@gmail.com;
- **Data de Criação:** 25/07/2024 - 18:37:00;
 
## SOBRE
- **Descrição:**
  - Implementação em Arduino de um sistema de chamada de enfermagem utilizando dois dispositivos ESP32, no qual um ESP32(Paciente) se comunica com outro ESP32(Servidor) para acionar uma luz quando um paciente solicita atendimento, além de armazenar as informações de endereço MAC dos dispositivos, ID do paciente que realizou a chamada e o timestamp da desta;
- **Funcionalidades:**
  - ESP32-Pacient: Realiza e registra chamadas de pacientes. Ele lê o endereço MAC do ESP32, conecta-se a uma rede Wi-Fi e a um servidor para enviar os dados do paciente que acionou o chamado. As informações incluem o endereço MAC do ESP32, o número do paciente e o timestamp do chamado. Além disso, o código utiliza o sistema de arquivos SPIFFS para armazenar logs localmente e tem a capacidade de formatar o sistema de arquivos ao pressionar o botão de RESET. Os botões no ESP32 são utilizados para registrar diferentes chamados e formatar o armazenamento;
  - ESP32-Server: Recebe e processa as chamadas de paciente, conectando-se a uma rede WiFi e iniciando um servidor na porta 80. Quando um cliente se conecta, ele registra os dados recebidos de timestamp, endereço MAC do cliente e ID do paciente que realizou a chamada, retornando em resposta ao cliente o seu próprio endereço MAC no formato de string. Por fim, ele acende o LED correspondente ao paciente que acionou o chamado, e desliga todos os LEDs ao pressionar o botão de desligar;
- **Requisitos:**
 - x2 dispositivos ESP32;
 - x5 push-buttons;
 - x3 Leds;
 - x3 Resistores de 220 ohms;
 - Possuir a biblioteca 'SPIFFS(https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/tag/1.1)' instalada na IDE do Arduino e inicializar uma imagem SPIFFS no ESP32 do Paciente;      

## DEMONSTRAÇÃO
![f1](https://github.com/user-attachments/assets/95a9901e-2559-446e-9af8-4daf5002f634)
![f2](https://github.com/user-attachments/assets/c150ec0c-1980-4714-92c2-60aa09e86461)
![f3](https://github.com/user-attachments/assets/91112a7e-3e46-42b2-8444-63f13ee91004)
![f4](https://github.com/user-attachments/assets/fe200c3c-56f5-49a8-9224-740161ca48fa)
![f5](https://github.com/user-attachments/assets/940c2fb6-5cea-4bb6-88b6-eefb19d2432f)
![f6](https://github.com/user-attachments/assets/359be2e3-8087-471a-920b-7c0ce48c851c)


