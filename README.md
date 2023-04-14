# Smart-Indoor
Smart indoor con control de temperatura, humedad y luces.

El proyecto utiliza un MC9S08SH8, un Microcontrolador de 8Bits, el cual registra los valores de:

- Termocupla tipo en conjunto con un Max6675 ( Conversor ADC )
- Módulo YL-38 : Sensor de humedad de suelo

Los cuales controlan los siguientes actuadores:

- Ventilador 12V
- Bomba 5v

Existen tres pantallas que se cambian mediante 1° pulsador:

- Control temperatura:
. Da 3 estados :  - Clima optimo
                  - Moderada
                  - Alerta roja

- Control de humedad:
. Activa o desactiva una bomba

- Control lumínico
. 2 modos controlados por un 2° pulsador: - Floración, tiene un contador descendente de 12:00:00 HS
                                       - Vegetación, tiene un contador descendente de 18:00:00 HS
. Se inicia y pausa la cuenta mediante un 3° pulsador

. Si el contador no esta pausado las luces permanecerán encendidas.
                                       


