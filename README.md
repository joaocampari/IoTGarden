# IoT Garden
> Irrigação automática e monitoramento de horta residencial 

## Sobre o Projeto
Projeto de um sistema de irrigação automática e seu monitoramento para uma horta residencial. No desenvolvimento deste projeto tivemos como objetivo aplicar os conceitos de Internet das Coisas, utilizar sensores para a leitura da umidade do solo, associado a microcontroladores integrados a plataforma Node MCU com demais componentes eletrônicos que enviam os dados coletados para uma aplicação. Visando a crescente tendência de automação residencial e o elevado número de dispositivos conectados, o objetivo do projeto foi criar uma solução compacta e simples em sua utilização.


## Como utilizar?
Para a utilização deste sistema, será necessários os seguintes componentes:
- NodeMCU com ESP2866
- Válvula Solenoide 12v 
- Módulo relé	
- Higrômetro
- Fonte Externa 12V
- Amazon Echo Dot 2 (Opcional)

Utilizando o seguinte esquema elétrico de montagem:
![eletrico](https://user-images.githubusercontent.com/37225019/85235136-fc9aa800-b3e8-11ea-86e8-7242fd4d8792.png)



Neste projeto utilizamos o Node-Red para controle de fluxos, também será necessário a instalação do mesmo, [disponível neste link](https://github.com/node-red/node-red).

## MQTT
Neste projeto utilizamos o Broker Público de [MQTT Eclipse.ORG](https://mqtt.eclipse.org).

Com o Node-Red e MQTT e com a Alexa o usuário poderá alterar / acionar parâmetros como:
- Segundos para verificação da Umidade
- Quantidade de segundos de irrigação
- Umidade Adequada
- Habilitar Solenoide

Foi criado o seguinte fluxo que também está [disponível neste projeto](https://github.com/joaocampari/HomeGarden/blob/master/nodered/flow.json).

![node-red](https://user-images.githubusercontent.com/37225019/85235235-c90c4d80-b3e9-11ea-98e5-6bc806f0bedb.jpeg)

Com o fluxo acima criado o node-red gera automaticamente uma interface onde o usuário poderá modificar os parâmetros.
![ui](https://user-images.githubusercontent.com/37225019/85235256-eccf9380-b3e9-11ea-8145-f02ba24c98b3.jpeg)


## Bibliotecas Utilizadas
- [ESP8266Wifi](https://github.com/esp8266/Arduino)
- [PubSubClient](https://github.com/knolleary/pubsubclient)
