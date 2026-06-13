#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

#define GPIO_SEÑAL_0 535  // GPIO 23 (512 + 23)
#define GPIO_SEÑAL_1 536  // GPIO 24 (512 + 24)

static dev_t first; 
static struct cdev c_dev; 
static struct class *cl; 
static int canal_seleccionado = 0; 

static int my_open(struct inode *i, struct file *f) { return 0; }
static int my_close(struct inode *i, struct file *f) { return 0; }

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    char mensaje_usuario[8];
    int longitud_msg;
    int estado_pin;

    if (*off > 0) return 0; // EOF

    // LEER HARDWARE CRUDO: El driver solo captura el bit físico (0 o 1)
    if (canal_seleccionado == 0) {
        estado_pin = gpio_get_value(GPIO_SEÑAL_0);
    } else {
        estado_pin = gpio_get_value(GPIO_SEÑAL_1);
    }

    // Enviamos el bit crudo ("0\n" o "1\n") al espacio de usuario
    longitud_msg = snprintf(mensaje_usuario, sizeof(mensaje_usuario), "%d\n", estado_pin);

    if (copy_to_user(buf, mensaje_usuario, longitud_msg) != 0) return -EFAULT;

    *off += longitud_msg;
    return longitud_msg; 
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
    char opcion;
    if (copy_from_user(&opcion, buf, 1) != 0) return -EFAULT;

    if (opcion == '0') {
        canal_seleccionado = 0;
    } else if (opcion == '1') {
        canal_seleccionado = 1;
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
    if ((ret = gpio_request(GPIO_SEÑAL_0, "SdeC_Sig0")) < 0) return ret;
    gpio_direction_input(GPIO_SEÑAL_0);

    if ((ret = gpio_request(GPIO_SEÑAL_1, "SdeC_Sig1")) < 0) {
        gpio_free(GPIO_SEÑAL_0);
        return ret;
    }
    gpio_direction_input(GPIO_SEÑAL_1);

    if ((ret = alloc_chrdev_region(&first, 0, 1, "SdeC_SudoMakeMe")) < 0) {
        gpio_free(GPIO_SEÑAL_0); gpio_free(GPIO_SEÑAL_1); return ret;
    }
    if (IS_ERR(cl = class_create("chardrv"))) {
        unregister_chrdev_region(first, 1); gpio_free(GPIO_SEÑAL_0); gpio_free(GPIO_SEÑAL_1); return PTR_ERR(cl);
    }
    if (IS_ERR(device_create(cl, NULL, first, NULL, "SdeC_SudoMakeMe"))) {
        class_destroy(cl); unregister_chrdev_region(first, 1); gpio_free(GPIO_SEÑAL_0); gpio_free(GPIO_SEÑAL_1); return PTR_ERR(cl);
    }
    cdev_init(&c_dev, &prueba_fops);
    cdev_add(&c_dev, first, 1);
    return 0;
}

static void __exit prueba_exit(void) {
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    gpio_free(GPIO_SEÑAL_0);
    gpio_free(GPIO_SEÑAL_1);
}

module_init(prueba_init); module_exit(prueba_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saqib Daniel");
MODULE_DESCRIPTION("Driver CDD Transparente");