## Sistema do robô 

O protótipo de robô móvel foi desenvolvido para atender às necessidades de atualização de um sistema prévio que utilizava o padrão de comunicação por rádio com uma placa de controle baseada no microcontrolador STM32 _Bluepill_ e no rádio NRF24. Esse sistema exigia uma comunicação serial entre o MATLAB e uma placa de circuito com o STM32 e rádio, exclusivamente para o envio de comandos de velocidade e a leitura de medições dos encoders, além de realizar estimativas de posição pela visão em terceira pessoa.

Diante dessas limitações, o sistema descrito na imagem abaixo é uma atualização para a plataforma de pesquisa robótica, agora focada no uso do ROS 2.
![image](https://github.com/user-attachments/assets/484a1fbd-c4ea-48d8-8331-10f56d143634)

Como visto, o sistema embarcado do robô contém a estrutura do sistema prévio e inclui a utilização do Raspberry Pi 4 como computador de bordo, portando o ROS 2 e o sensor LIDAR, o que permite uma melhora significativa na capacidade de sensoramento do sistema. Além disso, a integração do ROS 2 agora serve como o principal link de comunicação entre o PC que executa o algoritmo de navegação e controle e o _Raspberry Pi_, que centraliza as informações dos sensores e periféricos do sistema do robô.
