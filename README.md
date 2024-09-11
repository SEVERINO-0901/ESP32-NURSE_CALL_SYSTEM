## DADOS DO PROJETO
- **Nome:** ESP32-NurseCallSystem; 
- **Autor:** Leonardo Severino - leoseverino0901@gmail.com;
- **Data de Criação:** 25/07/2024 - 18:37:00;
 
## SOBRE
- **Descrição:**
  - Implementação em Arduino de um sistema de chamada de enfermagem utilizando dois dispositivos ESP32, no qual um ESP32(Paciente) se comunica com outro ESP32(Servidor) para acionar uma luz quando um paciente solicita atendimento, além de armazenar as informações de MAC address dos dispositivos, ID do paciente que realizou a chamada e o timestamp da desta;
- **Funcionalidades:**
  - Leitura do MAC Address: O ESP32 do Paciente lê seu próprio MAC address;
  - Envio de dados via Web: As informações coletadas são enviadas para uma página web para registro;
  - Associação de dispositivos: Um programa recebe o MAC address do ESP32 do Paciente e pergunta o número do leito ao qual esse endereço deve ser associado;
  - Controle da Lâmpada: Com base no número do leito, o sistema identifica qual lâmpada deve ser acionada quando o paciente chama a enfermeira;
  - Armazenamento de dados: O ESP32 do Paciente grava o seu MAC address, o MAC address do Servidor, o número do leito e o timestamp em seu sistema de arquivos;  

## DEMONSTRAÇÃO
- **![Figura 1]**

