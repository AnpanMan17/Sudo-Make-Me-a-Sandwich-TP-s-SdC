# Trabajo Práctico N°5: Device drivers

### Asignatura: Sistemas de Computación

**Facultad de Ciencias Exactas, Físicas y Naturales (UNC)**

---

* **Grupo:** Sudo Make A Sandwich
* **Profesores:** Miguel Angel Solinas y Javier Alejandro Jorge

---

### Integrantes y Contacto

| Nombre y Apellido | Correo Electrónico |
| :--- | :--- |
| **Sergio Andrés Fernández Segovia** | _sergio.fernandez.segovia@mi.unc.edu.ar_ |
| **Enzo Leonel Laura Surco** | _enzo.laura.surco@mi.unc.edu.ar_ |
| **Saqib Daniel Mohammad Cabrejos** | _saqib.mohammad@mi.unc.edu.ar_ |


# Desarrollo


Previamente se preparo el hardware que alojara nuestro modulo mediante una raspberry pi zero 2w con Raspberry OS Lite de 64 bits previamente flasheada en la micro SD. en donde nos conetamos a su terminal mediante SSH desde nuestra computadora de destino.


Este trabajo se decidio dividir en varias etapas primero 
Prueba de Concepto (Fase 1): compilar el driver con señales simuladas, cargarlo en la Raspberry Pi Zero 2W y verificar que responde correctamente a los comandos desde la terminal mediante cat y echo. Esto debido a que el metodo que utilizara es uno muy usado en sistema embebidos llamado Cross Compilation, compilando nuestro programa en una computadora Host y probandolo dentro de la arquitecura del procesasor de Destino.

el archivo en cuestion se llamara sdec_prueba.c 

La arquitectura de nuestra raspberry pi es de 64 bits lo cual nos servira:

![alt text](image.png)