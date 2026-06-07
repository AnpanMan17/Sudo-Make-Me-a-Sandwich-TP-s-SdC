#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

static dev_t first; 
static struct cdev c_dev; 
static struct class *cl; 

// Variables para simular los dos canales
static int canal_seleccionado = 0; 
static int contador_clicks = 0;

static int my_open(struct inode *i, struct file *f) {
    printk(KERN_INFO "SdeC_Prueba: open() convocado\n");
    return 0;
}

static int my_close(struct inode *i, struct file *f) {
    printk(KERN_INFO "SdeC_Prueba: close() convocado\n");
    return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    char mensaje_usuario[32];
    int longitud_msg;
    int valor_simulado;

    if (*off > 0) return 0; // Indica Fin de Archivo (EOF) a la aplicación

    // Generamos datos simulados predecibles según el canal para verificar que cambie
    if (canal_seleccionado == 0) {
        valor_simulado = 100 + contador_clicks; // Canal 0 arranca en 100
    } else {
        valor_simulado = 500 + contador_clicks; // Canal 1 arranca en 500
    }
    
    contador_clicks++; 

    // Convertimos el número a texto plano
    longitud_msg = snprintf(mensaje_usuario, sizeof(mensaje_usuario), "%d\n", valor_simulado);

    // Transfiere los datos de forma segura desde el espacio de kernel al de usuario
    if (copy_to_user(buf, mensaje_usuario, longitud_msg) != 0) {
        return -EFAULT;
    }

    *off += longitud_msg;
    return longitud_msg; 
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
    char opcion;

    // Transfiere la opción enviada por el usuario al espacio de kernel
    if (copy_from_user(&opcion, buf, 1) != 0) {
        return -EFAULT;
    }

    if (opcion == '0') {
        canal_seleccionado = 0;
        printk(KERN_INFO "SdeC_Prueba: Conmutado a Canal 0\n");
    } else if (opcion == '1') {
        canal_seleccionado = 1;
        printk(KERN_INFO "SdeC_Prueba: Conmutado a Canal 1\n");
    } else {
        printk(KERN_INFO "SdeC_Prueba: Opción inválida ignorada (enviar '0' o '1')\n");
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
    printk(KERN_INFO "SdeC_Prueba: Registrando modulo de testeo preliminar\n");

    if ((ret = alloc_chrdev_region(&first, 0, 1, "SdeC_Prueba")) < 0) return ret;
    
    if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv"))) {
        unregister_chrdev_region(first, 1);
        return PTR_ERR(cl);
    }
    
    if (IS_ERR(device_create(cl, NULL, first, NULL, "SdeC_Prueba"))) {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return PTR_ERR(cl);
    }

    cdev_init(&c_dev, &prueba_fops);
    if ((ret = cdev_add(&c_dev, first, 1)) < 0) {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return ret;
    }
    return 0;
}

static void __exit prueba_exit(void) {
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "SdeC_Prueba: Modulo de testeo removido del sistema\n");
}

module_init(prueba_init);
module_exit(prueba_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saqib Daniel");
MODULE_DESCRIPTION("Driver CDD de prueba para validacion de pipeline");