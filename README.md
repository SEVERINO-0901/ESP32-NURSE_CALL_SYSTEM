## DADOS DO PROJETO
- **Nome:** ESP32-NurseCallSystem; 
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
 - Possuir a biblioteca 'SPIFFS(https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/tag/1.1)' instalada na IDE do Arduino;      

## DEMONSTRAÇÃO
- **![Figura 1]**

