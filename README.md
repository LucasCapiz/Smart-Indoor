# Smart-Indoor
Este proyecto consiste en un sistema inteligente para interiores que controla la temperatura, humedad y luces. Está desarrollado en lenguaje C.

El proyecto utiliza un microcontrolador MC9S08SH8 de 8 bits, que registra los valores de los siguientes sensores:

- Termocupla tipo K junto con un Max6675 (conversor ADC)
- Módulo YL-38: Sensor de humedad del suelo

Estos sensores controlan los siguientes actuadores:

- Ventilador de 12V
- Bomba de 5V
- Luces

El sistema cuenta con tres pantallas que se seleccionan mediante un primer pulsador:
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Control de temperatura:

Muestra 3 estados:
- Clima óptimo
- Moderada
- Alerta roja
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Control de humedad:

Activa o desactiva una bomba según el rango de valores predeterminado.
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Control lumínico:

Tiene 2 modos controlados por un segundo pulsador:
- Floración: cuenta regresiva de 12 horas
- Vegetación: cuenta regresiva de 18 horas

Se puede iniciar y pausar la cuenta regresiva mediante un tercer pulsador. Si el contador no está pausado, las luces permanecerán encendidas.


