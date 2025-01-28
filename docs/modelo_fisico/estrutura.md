# Modelo 3D do CALMA-N

O desenvolvimento do modelo 3D da estrutura do robô foi realizado utilizando o software Autodesk Fusion. A estrutura é subdividida em níveis que são responsáveis por comportar os periféricos, descritos como: Estrutura de Base, Suporte da Eletrônica e Suporte do LIDAR.

# Estrutura de Base

A base é a parte da estrutura responsável por fixar os motores (e seus encoders), baterias e reguladores. A base do robô foi projetada com três funções principais: 
  • Fixar e alinhar os motores na estrutura, o que exigiu um suporte para ser parafusado a essa base, mantendo o alinhamento dos motores e fixando-os na estrutura; 
  • Acoplar duas baterias de forma que o peso ficasse distribuído de maneira simétrica, abaixando o centro de massa do robô, já que as baterias costumam ter um peso considerável e; 
  • Ser a parte da estrutura com maior densidade, contribuindo para que o centro de massa do robô esteja na parte inferior da estrutura.

A imagem a seguir mostra o projeto da Base e os modelos dos periféricos dispostos na base.

![image](https://github.com/user-attachments/assets/41b8c617-2d6f-43b7-b0d3-8e7bd61d84d1)


# Suporte da Eletrônica

O Suporte da Eletrônica é a parte da estrutura responsável pela organização dos principais dispositivos eletrônicos: _Raspberry Pi 4_, placa de controle, módulo Ponte H (L298N) e sensor Acelerômetro e Giroscópio (MPU-6050). Projetado como uma estrutura simples e plana, ela possui uma abertura central para facilitar o acesso à alimentação dos periféricos mencionados, bem como ao controle dos atuadores e à leitura dos _encoders_. O projeto do Suporte da Eletrônica e a disposição dos modelos dos periféricos são ilustrados na figura abaixo.
![image](https://github.com/user-attachments/assets/e6e3389d-da64-42c0-ab81-4da6f41063fa)


# Suporte do LIDAR

Como o próprio nome indica, o suporte do LIDAR é a parte superior do robô, cuja função principal é fixar o LIDAR na estrutura. Além disso, essa parte também será utilizada para incluir marcadores que facilitam a detecção e o rastreamento de posição a partir de uma câmera em terceira pessoa. Essa estrutura é ilustrada na proxíma figura com o projeto do Suporte do LIDAR e o modelo do YLIDAR X2L

![image](https://github.com/user-attachments/assets/5b9a237e-7b81-4a94-b5c5-a94202b3d5a2)


Com a união dessas partes, forma-se o modelo completo da estrutura do robô, desenvolvido como a idealização de uma plataforma de pesquisa robótica para integrar os sensores e periféricos ao sistema ROS2, a ser utilizada nas atividades do Grupo de Pesquisa em Robótica da UFS (GPR-UFS). As partes da estrutura são acopladas por meio de barras roscadas, garantindo estabilidade e firmeza. Isso pode ser visualizado na imagem a seguir. 

![calmanrend2](https://github.com/user-attachments/assets/dd564e04-4f02-44bf-9b13-45b1033e7699)


