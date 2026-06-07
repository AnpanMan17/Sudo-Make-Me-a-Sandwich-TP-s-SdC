#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h> // <-- Nueva cabecera para manejar los pines físicos

// Definimos los pines GPIO físicos que vas a usar (puedes cambiarlos por los que prefieras)
#define GPIO_SENAL_0  17
#define GPIO_SENAL_1  27

static dev_t first; 
static struct cdev c_dev; 
static struct class *cl; 

static int canal_seleccionado = 0; 

static int my_open(struct inode *i, struct file *f) {
    printk(KERN_INFO "SdeC_Sensor: Puerto abierto\n");
    return 0;
}

static int my_close(struct inode *i, struct file *f) {
    printk(KERN_INFO "SdeC_Sensor: Puerto cerrado\n");
    return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    char mensaje_usuario[32];
    int longitud_msg;
    int valor_real = 0;

    if (*off > 0) return 0; // EOF

    // LECTURA REAL DE HARDWARE:
    // gpio_get_value devuelve 0 (LOW) o 1 (HIGH) directamente desde el pin físico
    if (canal_seleccionado == 0) {
        valor_real = gpio_get_value(GPIO_SENAL_0); 
    } else {
        valor_real = gpio_get_value(GPIO_SENAL_1);
    }
    
    // Convertimos el estado digital (0 o 1) a texto para el espacio de usuario
    longitud_msg = snprintf(mensaje_usuario, sizeof(mensaje_usuario), "%d\n", valor_real);

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
        printk(KERN_INFO "SdeC_Sensor: Monitoreando GPIO %d\n", GPIO_SENAL_0);
    } else if (opcion == '1') {
        canal_seleccionado = 1;
        printk(KERN_INFO "SdeC_Sensor: Monitoreando GPIO %d\n", GPIO_SENAL_1);
    }

    return len; 
}

static struct file_operations sensor_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write
};

static int __init sensor_init(void) {
    int ret;
    printk(KERN_INFO "SdeC_Sensor: Inicializando e interrumpiendo hardware\n");

    // 1. Validar y solicitar los pines GPIO al Kernel de Linux
    if (!gpio_is_valid(GPIO_SENAL_0) || !gpio_is_valid(GPIO_SENAL_1)) {
        printk(KERN_ERR "SdeC_Sensor: Pines GPIO inválidos configurados\n");
        return -ENODEV;
    }

    if (gpio_request(GPIO_SENAL_0, "sdec_in_0") < 0) {
        printk(KERN_ERR "SdeC_Sensor: Error solicitando GPIO %d\n", GPIO_SENAL_0);
        return -EBUSY;
    }
    gpio_direction_input(GPIO_SENAL_0); // Configurarlo como entrada

    if (gpio_request(GPIO_SENAL_1, "sdec_in_1") < 0) {
        printk(KERN_ERR "SdeC_Sensor: Error solicitando GPIO %d\n", GPIO_SENAL_1);
        gpio_free(GPIO_SENAL_0); // Liberamos el primero si el segundo falla
        return -EBUSY;
    }
    gpio_direction_input(GPIO_SENAL_1); // Configurarlo como entrada

    // 2. Registro del Driver de Caracteres (VFS)
    if ((ret = alloc_chrdev_region(&first, 0, 1, "SdeC_Sensor")) < 0) {
        gpio_free(GPIO_SENAL_0);
        gpio_free(GPIO_SENAL_1);
        return ret;
    }
    
    if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv"))) {
        unregister_chrdev_region(first, 1);
        gpio_free(GPIO_SENAL_0);
        gpio_free(GPIO_SENAL_1);
        return PTR_ERR(cl);
    }
    
    if (IS_ERR(device_create(cl, NULL, first, NULL, "SdeC_Sensor"))) {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        gpio_free(GPIO_SENAL_0);
        gpio_free(GPIO_SENAL_1);
        return PTR_ERR(cl);
    }

    cdev_init(&c_dev, &sensor_fops);
    if ((ret = cdev_add(&c_dev, first, 1)) < 0) {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        gpio_free(GPIO_SENAL_0);
        gpio_free(GPIO_SENAL_1);
        return ret;
    }
    return 0;
}

static void __exit sensor_exit(void) {
    // Liberar los recursos de los caracteres
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);

    // LIBERAR LOS PINES GPIO (Crucial para que otros drivers puedan usarlos después)
    gpio_free(GPIO_SENAL_0);
    gpio_free(GPIO_SENAL_1);

    printk(KERN_INFO "SdeC_Sensor: Hardware liberado y modulo removido\n");
}

module_init(sensor_init);
module_exit(sensor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saqib Daniel");
MODULE_DESCRIPTION("Driver CDD que lee entradas digitales desde GPIO17 y GPIO27");