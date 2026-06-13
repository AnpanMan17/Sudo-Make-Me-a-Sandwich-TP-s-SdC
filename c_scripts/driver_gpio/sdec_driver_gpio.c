#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h> // Para controlar los GPIOs

#define GPIO_SEÑAL_0 535  // (512 de base + 23 del GPIO)
#define GPIO_SEÑAL_1 536  // (512 de base + 24 del GPIO)

static dev_t first; 
static struct cdev c_dev; 
static struct class *cl; 

// Variables de estado del dispositivo
static int canal_seleccionado = 0; 

static int my_open(struct inode *i, struct file *f) {
    printk(KERN_INFO "SdeC_SudoMakeMe: open() convocado\n");
    return 0;
}

static int my_close(struct inode *i, struct file *f) {
    printk(KERN_INFO "SdeC_SudoMakeMe: close() convocado\n");
    return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    char mensaje_usuario[32];
    int longitud_msg;
    int valor_final;
    int estado_pin;

    if (*off > 0) return 0; // EOF para la aplicación de usuario

    // MODIFICACIÓN CLAVE: Leemos el estado físico real de los pines GPIO
    if (canal_seleccionado == 0) {
        estado_pin = gpio_get_value(GPIO_SEÑAL_0); // Retorna 0 o 1 según el voltaje real en el pin 23
        
        // Convertimos el bit físico en una escala coherente de Temperatura (°C)
        valor_final = estado_pin ? 26 : 23; 
    } else {
        estado_pin = gpio_get_value(GPIO_SEÑAL_1); // Retorna 0 o 1 según el voltaje real en el pin 24
        
        // Convertimos el bit físico en una escala coherente de Presión (hPa)
        valor_final = estado_pin ? 1016 : 1009; 
    }

    // Convertimos el número a texto plano para el espacio de usuario
    longitud_msg = snprintf(mensaje_usuario, sizeof(mensaje_usuario), "%d\n", valor_final);

    // Transfiere los datos de forma segura desde el espacio de kernel al de usuario
    if (copy_to_user(buf, mensaje_usuario, longitud_msg) != 0) {
        return -EFAULT;
    }

    *off += longitud_msg;
    return longitud_msg; 
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
    char opcion;

    if (copy_from_user(&opcion, buf, 1) != 0) {
        return -EFAULT;
    }

    if (opcion == '0') {
        canal_seleccionado = 0;
        printk(KERN_INFO "SdeC_SudoMakeMe: Conmutado a Canal 0 (GPIO 23)\n");
    } else if (opcion == '1') {
        canal_seleccionado = 1;
        printk(KERN_INFO "SdeC_SudoMakeMe: Conmutado a Canal 1 (GPIO 24)\n");
    } else {
        printk(KERN_INFO "SdeC_SudoMakeMe: Opción inválida ignorada\n");
    }

    return len; 
}

static struct file_operations prueba_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write
};

static int __init prueba_init(void) {
    int ret;
    printk(KERN_INFO "SdeC_SudoMakeMe: Registrando modulo e inicializando GPIOs\n");

    // Reservamos y configuramos los pines físicos en el Kernel
    if ((ret = gpio_request(GPIO_SEÑAL_0, "SdeC_Sig0")) < 0) return ret;
    gpio_direction_input(GPIO_SEÑAL_0);

    if ((ret = gpio_request(GPIO_SEÑAL_1, "SdeC_Sig1")) < 0) {
        gpio_free(GPIO_SEÑAL_0); // Deshacemos el anterior si este falla
        return ret;
    }
    gpio_direction_input(GPIO_SEÑAL_1);

    // Registro del dispositivo de caracteres
    if ((ret = alloc_chrdev_region(&first, 0, 1, "SdeC_SudoMakeMe")) < 0) {
        gpio_free(GPIO_SEÑAL_0);
        gpio_free(GPIO_SEÑAL_1);
        return ret;
    }
    
    if (IS_ERR(cl = class_create("chardrv"))) {
        unregister_chrdev_region(first, 1);
        gpio_free(GPIO_SEÑAL_0);
        gpio_free(GPIO_SEÑAL_1);
        return PTR_ERR(cl);
    }
    
    if (IS_ERR(device_create(cl, NULL, first, NULL, "SdeC_SudoMakeMe"))) {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        gpio_free(GPIO_SEÑAL_0);
        gpio_free(GPIO_SEÑAL_1);
        return PTR_ERR(cl);
    }

    cdev_init(&c_dev, &prueba_fops);
    if ((ret = cdev_add(&c_dev, first, 1)) < 0) {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        gpio_free(GPIO_SEÑAL_0);
        gpio_free(GPIO_SEÑAL_1);
        return ret;
    }
    return 0;
}

static void __exit prueba_exit(void) {
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);

    // MODIFICACIÓN CRÍTICA: Liberamos los pines para que puedan volver a usarse
    gpio_free(GPIO_SEÑAL_0);
    gpio_free(GPIO_SEÑAL_1);

    printk(KERN_INFO "SdeC_SudoMakeMe: Modulo removido y GPIOs liberados\n");
}

module_init(prueba_init);
module_exit(prueba_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saqib Daniel");
MODULE_DESCRIPTION("Driver CDD con lectura GPIO real");