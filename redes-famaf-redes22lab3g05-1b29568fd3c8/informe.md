# Laboratorio 3: Transporte

## Indice
* [Parte 1](#markdown-header-parte-1)
    * [Caso 1](#markdown-header-caso-1)
    * [Caso 2](#markdown-header-caso-2)
    * [Conclusiones](#markdown-header-conclusiones-primer-parte)
* [Parte 2](#markdown-header-parte-2)
    * [Caso 1](#markdown-header-caso-1-parte-2)
    * [Caso 2](#markdown-header-caso-2-parte-2)
    * [Conclusiones](#markdown-header-conclusiones-segunda-parte)
* [Parte 2.1 Otra implementación](#markdown-header-parte-2.1-otra-implementación)
* [Conclusiones Generales](#markdown-header-conclusiones-generales)
* [Otros datos](#markdown-header-otros-datos)
* [Trabajo Futuro](#markdown-header-trabajo-futuro)
* [Observaciones](#markdown-header-obs)
* [Integrantes](#markdown-header-integrantes)

# Introducción

Analizaremos vía simulación con Omnetpp, qué sucede en una red con determinados cuellos de botella, pudiendo identificar problemas de congestión y de flujo.  
Para ello dividiremos el trabajo de análisis en dos partes:  

1. En la parte 1 nos encargaremos de analizar el rendimiento y diagnosticar la causa de la pérdida de paquetes en el protocolo sin control de congestión entregado por la cátedra.  

2. En la parte 2 buscaremos demostrar que el protocolo implementado por nosotros, los estudiantes, no tiene pérdida de paquetes. Analizaremos su costo en el rendimiento comparádolo con la parte 1 y explicaremos por que baja.

A continuación mencionaremos las modificaciones realizadas en el código esqueleto inicial brindado por la cátedra:

## Modelo de colas

El kickstarter inicial consta de tres partes básicas:

* Generador (gen): crea y transmite paquetes con intervalos dados por una distribución exponencial de media configurable.

* Cola (queue): atiende los paquetes bajo una misma distribución.

* Resumidero (sink): toma métricas (.vec y .sca) de la demora de la entrega de los paquetes.

## Modificaciones en network.ned

Generamos dos nuevos módulos compuestos, NodeTx y NodeRx.

* NodeTx: Generador + Queue
* NodeRx: Queue + Sink

## Modificaciones en clases C++

- Se modificó el objeto cMessage por cPacket.
- Se modificó la duración de la transmisión del paquete una vez ya encolado.
- Se modificó el código de Queue para que controle el tamaño del buffer.
- Se agregaron nuevas métricas para el análisis de Queue, una para los paquetes en el buffer y otra para la cantidad de paquetes descartados por buffer saturado.


# Tarea Análisis

Como bien mencionamos en la introducción para analizar el tráfico de red vamos a separar cada parte en dos casos de estudio:

* Caso de estudio 1:
    * NodeTx a Queue: datarate = 1 Mbps y delay = 100 s
    * Queue a NodeRx: datarate = 1 Mbps y delay = 100 s
    * Queue a Sink: datarate = 0.5 Mbps

* Caso de estudio 2:
    * NodeTx a Queue: datarate = 1 Mbps y delay = 100 s
    * Queue a NodeRx: datarate = 0.5 Mbps y delay = 100 s
    * Queue a Sink: datarate = 1 Mbps

Para los análisis se simulará durante un tiempo de 200 s, tamaños de paquete de 12500 bytes cada uno, capacidad de 200 paquetes tanto para el Queue de la red como para el buffer de NodeRx y capacidad de 2.000.000 paquetes para el buffer de NodeTx

Para cada caso de estudio iremos variando el intervalo de generación de paquetes, para que en algunos casos se generen más paquetes, en otros menos y ver cómo se comporta la red, los cuellos de botella y tamaños de buffers en dichas situaciones.  

Para poder obtener determinadas métricas fue necesario agregar tanto vectores (Lista de datos), cómo escalares (número).
Dichas métricas fueron agregadas en los archivos _.cc_ y algunas pequeñas modificaciones para mostrar los buffer_size en _network.ned_ y para setear su valor modificamos _omnetpp.ini_.

Exportamos los resultados (_.csv_) y fueron guardados en el directorio _csvs+notebook+graficas_ en la raiz del proyecto.
Sin más que agregar pasamos a ver los distintos resultados arrojados por las simulaciones.


* * *


# Parte 1

El interés específico aquí es el de observar y diferenciar entre problemáticas de control de congestión y control de flujo.
Ver cómo se llenan buffers, cómo trabajan las colas y cada parte de la red, y realizarnos preguntas para poder comprender dicha red. 
Cómo por ejemplo *¿Cuánto tarda en llegar al destino un paquete?* y demás, para ello realizamos gráficas que presentaremos a continuación, y luego de eso la conclusión obtenida de esas gráficas.

## Caso 1

**Primer análisis con generation interval de 0,1**

Cantidad de paquetes enviados, dropeados y recibidos:

* NodeTx: paquetes enviados   = 1.979
* Network: paquetes recibidos = 1.970
* NodeRx: paquetes dropeados  = 770
* Sink: paquetes recibidos    = 998

- Mayor tamaño alcanzado en la queue del nodo Tx en 200s: 33 pkt.

- Mayor tamaño alcanzado en la queue del nodo Rx en 200s: 200 pkt.

- Mayor tamaño alcanzado en la queue den Network en 200s: 1 pkt.

- Mayor tiempo de un paquete en llegar al sink: 43,50 s

- Tiempo promedio de un paquete en llegar al sink: 32,88 s  

Como la única queue que alcanza su capacidad máxima es la de NodeRx eso implica que todos los paquetes perdidos por saturación de los buffers se producen aquí, en ese caso uno podría esperar que se cumpla la siguiente igualdad:  

    (paquetes dropeados por NodeRx) + (paquetes recibidos por Sink) = (paquetes enviados por NodeTx)  
    770 + 998 <  1979
  
¿Por qué? Porque no estamos teniendo en cuenta los paquetes que aún siguen en las queue al terminar la simulación (en este caso son 211).

Dicha saturación se produce porque a NodeRx recibe paquetes a una velocidad aproximada de 1Mbps mientras que su capacidad de salida es de 0.5 Mbps, generándose un cuello de botella.

**Segundo análisis con generation interval de 0,2**

Cantidad de paquetes enviados, dropeados y recibidos:

* NodeTx:  paquetes enviados  = 989
* Network: paquetes recibidos = 989
* NodeRx:  paquetes dropeados = 0
* Sink:    paquetes recibidos = 976

- Mayor tamaño alcanzado en la queue del nodo Tx 4 pkt.

- Mayor tamaño alcanzado en la queue del nodo Rx: 32 pkt.

- Mayor tamaño alcanzado en la queue del Network: 1 pkt.

- Mayor tiempo de un paquete en llegar al sink: 6,88 s

- Tiempo promedio de un paquete en llegar al sink: 2,40 s

Podemos notar que un generation interval de 0,2 ya es lo suficientemente lento como para que todas las queue estén lejos de alcanzar su capacidad máxima, por lo que todos los 13 paquetes que fueron enviados por NodeTx pero no son contados como recibidos por Sink se deben encontrar en el buffer de NodeRx

> A partir de aquí podemos pensar que será repetitivo para el resto de generation interval que tomemos = { 0,3; 0,4; 0,5; 0,6; 0,7; 0,8; 0,9; } tomaremos el quinto caso sólamente pues será el mostrado en las gráficas.

**Quinto análisis con generation interval de 0,5**

Cantidad de paquetes enviados, dropeados y recibidos:

* NodeTx:  paquetes enviados = 390
* Network: paquetes recibidos = 390
* NodeRx:  paquetes dropeados = 0
* Sink:    paquetes recibidos = 390

- Mayor tamaño alcanzado en la queue del Network: 2 pkt.

- Mayor tamaño alcanzado en la queue del Network: 3 pkt.

- Mayor tamaño alcanzado en la queue del Network: 1 pkt.

- Mayor tiempo de un paquete en llegar al sink: 1,05 s

- Tiempo promedio de un paquete en llegar al sink: 0,46 s

Observando que nuevamente como la generación de paquetes es más amplia, entonces eso significa que habrá menos paquetes, en este caso no tenemos pérdidas, en ningún nodo.

## Gráficas Primer Caso 

![Parte 1 Caso 1 generation intervals 0,1;0,2;0,5 BufferSizes](/Parte1/graficas/p1c1g01g02g05.png)

![Parte 1 Caso 1 generation intervals 0,1;0,2;0,5 Delay](/Parte1/graficas/delayp1c1g01g02g05.png)

- [Return](#markdown-header-indice)

## Caso 2

**Primer análisis con generation interval de 0,1**

Cantidad de paquetes enviados, dropeados y recibidos:

* NodeTx:  paquetes enviados  = 1979
* Network: paquetes dropeados = 771
* NodeRx:  paquetes recibidos = 999
* Sink:    paquetes recibidos = 998


Esto era esperable pues hay un cuello de botella del Net.Queue a NodeRx: datarate = 0.5 Mbps esto produce que rebalse el buffer de Net.Queue y empiece a dropear paquetes.

- Mayor cantidad de paquetes encolados en la queue del nodo Tx en 200ms: 33 pkt.

-  Mayor cantidad de paquetes encolados en la queue del nodo Rx en 200ms: 1 pkt.

-  Mayor cantidad de paquetes encolados en la queue del Networken 200ms: 200 pkt.

- Mayor tiempo de un paquete en llegar al sink: 43,50 s

- Tiempo promedio de un paquete en llegar al sink: 32,88 s

**Segundo análisis con generation interval de 0,2**

Cantidad de paquetes enviados, dropeados y recibidos:

* NodeTx:  paquetes enviados  = 989
* Network: paquetes dropeados = 0
* NodeRx:  paquetes recibidos = 977
* Sink:    paquetes recibidos = 976

Teniendo así la pérdida de 0 paquetes del NodeTx al buffer del Net.Queue, perdiendo 12 paquete del Net.Queue al buffer del NodeRx, y luego perdiendo 1 paquete desde el NodeRx al Sink.
Esto era esperable pues hay un cuello de botella del Net.Queue a NodeRx: datarate = 0.5 Mbps esto produce que rebalse el buffer de Net.Queue y empiece a dropear paquetes.

- Mayor cantidad de paquetes encolados en la queue del nodo Tx en 200ms: 4 pkt.

- Mayor cantidad de paquetes encolados en la queue del nodo Rx en 200ms: 1 pkt.

- Mayor cantidad de paquetes encolados en la queue den Network en 200ms: 32 pkt.

- Mayor tiempo de un paquete en llegar al sink: 6,88 s

- Tiempo promedio de un paquete en llegar al sink: 2,40 s

> El caso de generation interval de 0,5 no lo pusimos aquí, pero sí lo graficamos, nos pareció que hace una lectura más larga inecesariamente.

## Gráficas Segundo Caso 

![Parte 1 Caso 2 generation intervals 0,1;0,2;0,5 BufferSizes](/Parte1/graficas/p1c2g01g02g05.png)

![Parte 1 Caso 2 generation intervals 0,1;0,2;0,5 Delay](/Parte1/graficas/delayp1c2g01g02g05.png)

- [Return](#markdown-header-indice)

**1. ¿Qué diferencia observa entre el caso de estudio 1 y 2? ¿Cuál es la fuente limitante en cada uno? Investigue sobre la diferencia entre control de flujo y control de congestión.**

Los caso 1 y 2 son muy similares, por eso nuestros analisis dieron resultados casi identicos, la única
diferencia destacable es donde se produce el cuello de botella (y por tanto los problemas de congestion para ciertos intervalos de generación), en el caso 1 se encuentra encuentra en el buffer de sink mientras que en el caso 2 en la queue de Network, ambos teniendo una entrada de 1Mbps y una salida de 0.5 Mbps.

La diferencia entre control de flujo y control de congestión esta en que: aunque el fin úlitimo de ambos es que la mayor cantidad de paquetes posibles lleguen a su destino, el control de flujo lo logra reenviando los paquetes perdidos y encontrando una velocidad de transmisión con la que sus paquetes no sean dropeados, mientras que el control de congestion suele lograrlo encontrando nuevas rutas cuando se desconecta un enrutador o para prevenir la congestión de la red.

* * *

# Conclusiones Primer Parte

Como se pudo observar en las distintas gráficas con sus distintos intervalos de generación, dependiendo en donde generemos el cuello de botella (donde el data rate sea más bajo que en el resto del network) será el lugar donde "perdamos" o se dropeen paquetes con sus respectivas cantidades.
Al ir aumentando el intervalo de generación de paquetes, se generan menos y por lo tanto no se congestiona la red y no se pierden paquetes, éso sí, se envían menos y llegan menos, pero aumenta la eficacia de entrega de paquetes.

## Caso 1:
|Generation Interval | Paquetes Enviados  | Paquetes Recibidos | Paquetes Perdidos | Paquetes Dropeados | Porcentaje de Recibidos (%) |
| -- | -- | -- | -- | -- | -- |
| 0,1 | 1.979 | 998 | 211 | 770 | 50,43 |
| 0,2 | 989  | 976 | 13 | 0 | 98,69 |
| 0,5 | 390 |  390 | 0 | 0 | 100 |

## Caso 2:
|Generation Interval | Paquetes Enviados  | Paquetes Recibidos | Paquetes Perdidos | Paquetes Dropeados | Porcentaje de Recibidos (%) |
| -- | -- | -- | -- | -- | -- |
| 0,1 | 1.979 | 998 | 210 | 771 | 50,43 |
| 0,2 | 989  | 976 | 13 | 0 | 98,69 |
| 0,5 | 390 |  390 | 0 | 0 | 100 |

Con éste cuadro queda muy claro lo mencionado anteriormente, y se visualiza que ambos casos arrojan mismos resultados, y que el problema sigue estando, lo que cambia es el lugar en el que lo tenemos. Explicitando lo difícil que puede ser encontrar un cuello de botella.

**¿En qué nos interesan los paquetes perdidos y dropeados?**

Para poder saber cómo se transporta nuestro flujo, para esto existirá un invariante que queremos se cumpla en nuestra network:

PaquetesEnviados = (PaquetesRecibidos + PaquetesDropeados + PaquetesPerdidos)

Dependiendo qué prioricemos, habrá distintos valores que podemos considerar como mejores casos:

* Velocidad? --> entonces no nos interesará demaciado 'PaquetesDropeados' y 'PaquetesPerdidos'.
* Eficacia? --> entonces nos interesará que 'PaquetesDropeados' y 'PaquetesPerdidos' sean lo más cercanos a cero que se pueda.
* Eficacia y velocidad? --> Buscar un "medio".

Y así se puede continuar dependiendo las prioridades.

**¿Porqué se pierden paquetes?**

Vamos a dividir casos en los cuales los paquetes pueden perderse¹ teóricamente de los que puedan perderse dentro de la simulación de Omnet++.
Sólo nos preocuparemos en definir las situaciones en las que se pierdan paquetes en la simulación, que pueden ser las siguientes:

* La simulación tenía tiempo = t, y en ese tiempo t el paquete no llegó al resumidero, por lo tanto quedó "perdido" ó almacenado en un buffer.
* Alguno de los buffers estaba lleno al recibir el paquete

**¿Qué significa que se dropeen paquetes?**

Al tener distintos data rate, formarse cuellos de botella y tener un tamaño limitado del buffer, este al llenarse, debe empezar a "tirar"/"dropear" paquetes ya que no puede guardarlos, a esto llamamos buffer overflow.

**¿Cuál es la diferencia entre paquete droppeado y perdido?**

La diferencia entre dropeados y perdidos es que, el dropeado es un paquete que llegó a su destino, pero se decidió tirarlo, en cambio el perdido, es un paquete que no llegó a su destino por cuestiones varias.

1: "Se pueden llegar a tener pérdidas de paquetes por interferencias o ruido, y problemas de capa física."


* * *

# Parte 2

Para ésta segunda parte volvemos a aplicar ambos casos de estudio, pero para ésta parte debíamos implementar un algoritmo que trabaje con el control de congestión y control de flujo entre el generador y el destino.
Al haber apertura para que diseñemos el que el grupo quiera, decidimos implementar el algoritmo de Stop and Waiting, que es utilizado para el control de flujo, pero en éste caso al sólo preocuparnos por la capa de transporte, con dicha implementación podemos solucionar el control de congestión.
Y al implementar éste algoritmo ya no tendrán demasiado interés las gráficas en casos distintos, pues al enviar de a un paquete y esperar que regrese, no debería haber congestión, ni más de 1 paquete en buffers.
Interés respecto a comparación de escenarios Parte 1 y Parte 2, ya que no se visualizarán los cuellos de botella.

Las gráficas vinculadas a ésta parte y de ambos casos están en la notebook, eran constantes por lo tanto no las incluímos.

Para generar un análisis y visualizar las diferencias entre ambas partes, incluiremos una pequeña tabla comparativa entre ambas partes, caso 1 pero es indistinto el caso 2.

Ver [Conclusiones](#markdown-header-conclusiones-segunda-parte)

- [Return](#markdown-header-indice)

**1. ¿Cómo cree que se comporta su algoritmo de control de flujo y congestión? ¿Funciona para el caso de estudio 1 y 2 por igual? ¿Por qué?**

Nuestro algoritmo evita la congestión, ya que implementamos el algoritmo de parada y espera, el mismo sólo manda de a un paquete.
Funciona para ambos casos, ya que se comporta igual, y el data rate no le afecta. Porque siempre envía de a un paquete y espera que llegue a su destino.
No es eficaz si nuestra prioridad es envíar el máximo posible de paquetes, pero es un algoritmo de capa de transporte que es eficaz en la entrega de paquetes. Depende de la perspectiva que evaluemos será su eficacia.


* * *


# Parte 2.1 otra implementación

Hicimos una Parte 2.1 para poder mostrar el funcionamiento del stop and waiting con pérdida de paquetes, ya que en la implementación de la parte 2 ésto no pasaba.
Con ésta implementacion se vuelve más interesante el análisis sobre el mismo. Dejaremos sólo el gráfico de delays pues los buffers sizes serán los mismos. Los delays serán los mismos en ambos casos.

![Parte 2.1 Caso 1 y 2 generation intervals 0,1;0,2;0,5 Delay](/Parte2.1/grafica/delayp21c12g01g02g05.png)


* * *

# Conclusiones Segunda Parte

Logramos implementar el algoritmo de manera correcta y respaldamos su implementación con los datos brindados por las simulaciones.
Viendo que no se pierden paquetes en la Parte 2, viendo que no hay congestión, y luego implementamos la Parte 2.1 para poder mostrar un stop and waiting con un poco más de irregularidades, para mostrar los delays que se pueden llegar a tener.

## Parte 1:
|Generation Interval | Paquetes Enviados  | Paquetes Recibidos | Paquetes Perdidos | Paquetes Dropeados | Porcentaje de Recibidos (%) |
| -- | -- | -- | -- | -- | -- |
| 0,1 | 1.979 | 998 | 211 | 770 | 50,43 |
| 0,2 | 989  | 976 | 13 | 0 | 98,69 |
| 0,5 | 390 |  390 | 0 | 0 | 100 |

## Parte 2:
|Generation Interval | Paquetes Enviados  | Paquetes Recibidos | Paquetes Perdidos | Paquetes Dropeados | Porcentaje de Recibidos (%) |
| -- | -- | -- | -- | -- | -- |
| 0,1 | 498 | 497 | 1 | 0 | 99,8 |
| 0,2 | 466  | 465 | 1 | 0 | 99,8 |
| 0,5 | 319 |  319 | 0 | 0 | 100 |

Como se visualiza en las tablas comparativas, vemos cómo obtenemos una eficiencia en entrega de paquetes totalmente notoria de la Parte 2 comparada con la Parte 1.
Pero lo que también vemos, es la diferencia en comparativas de cantidad de paquetes enviados:

|       | Paquetes envíados  |
|--|--|
|Parte 1 genInt 0,1 | 1.979 |
|Parte 2 genInt 0,1 | 498 |

> Siendo los enviados en la Parte 2, un 25% de enviados a comparación con la Parte 1.

## Conclusiones Parte 2.1

Gráfica para poder ver lo que ocurre en la red con ésta nueva implementación.

## Parte 2.1:
|Generation Interval | Paquetes Enviados  | Paquetes Recibidos | Paquetes Perdidos | Paquetes Dropeados | Porcentaje de Recibidos (%) |
| -- | -- | -- | -- | -- | -- |
| 0,1 | 192 | 191 | 1 | 0 | 99,8 |
| 0,2 | 188  | 187 | 1 | 0 | 99,8 |
| 0,5 | 170 |  169 | 1 | 0 | 99,8 |

> Siendo los enviados en la Parte 2.1, un 38,5% de enviados a comparación con la Parte 2.



Ver [notebook](/csvs%2Bnotebook%2Bgraficas/redes%20lab%203.ipynb) 

- [Return](#markdown-header-indice)

* * *

# Conclusiones Generales

Para los siguientes gráficos fueron evaluados los generation interval:

_{{0,9},{0,5},{0,25},{0,23},{0,21},{0,19},{0,17},{0,15},{0,13},{0,11},{0,1}}_

aplicado a las distintas partes y casos.

**Carga útil y Carga ofrecida**

![Carga_util_ofrecida](/graficas/carga_ofrecida_util_py.png)

![Delay](/graficas/delay_gen_py.png)


# Otros datos

Ver el [notebook](/csvs%2Bnotebook%2Bgraficas/redes%20lab%203.ipynb) 

- [Return](#markdown-header-indice)

# Trabajo Futuro

* Lograr que se puedan envíar más paquetes manteniendo un porcentaje bajo o nulo de paquetes perdidos, sin saturar buffers.

# Obs

Para poder trabajar cómodamente en el .ipynb es necesario instalar pandas y seaborn

> pip install pandas

> pip install seaborn

Desde el repositorio de BitBucket no se puede previsualizar el notebook por lo tanto, se aconseja descargarlo.

# Bibliografía

* [Manual Omnetpp](https://omnetpp.org/doc/omnetpp/manual/)
* [Graficas en Py](https://colab.research.google.com/drive/1_uT3BfnePUV1L-NAM0qmzWjLNb4xTUsB?usp=sharing#scrollTo=FZEUCUq82c31)
* [Markdown en BitBucket](https://bitbucket.org/tutorials/markdowndemo/src/master/README.md#markdown-header-span-elements)

# Integrantes

* JUAN MANUEL KERSUL : juan.kersul@mi.unc.edu.ar

* ANDRÉS DAVID DOCTORS SIMMONS : andresdaviddoctors@mi.unc.edu.ar

* JUAN YORNET BARBIERI : juan.yornet@mi.unc.edu.ar
