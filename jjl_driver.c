#include <linux/init.h>    //
#include <linux/module.h>  // module_init(), module_exit(), THIS_MODULE, ...
#include <linux/kernel.h>  // printk(), KERN_INFO, KERN_WARNING, ...
#include <linux/fs.h>      // alloc_chrdev_region(), MAJOR(), MINOR(), file_operations, file, inode, ...
#include <linux/cdev.h>    // cdev_init()
MODULE_LICENSE("GPL");
MODULE_AUTHOR("James Lawson");

// [1]: kernel-space data about the state of the device
// [2]: use file_operations struct to bind device driver functions to the device file,
//      so that io system calls the device file like read() and write() will call *our* functions
dev_t dev_nums;
/* struct jjl_device { */
/*     struct cdev cdev;      // stores kernel character device info */
/* }; */
/* struct jjl_device jjl_dev; */
/* struct file_operations jjl_fops = { .owner = THIS_MODULE, .open  = jjl_open, .read  = jjl_read, .release = jjl_release, }; */
/* int jjl_open(struct inode *inode, struct file *filp) { return 0; } */
/* int jjl_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) { return 0; } */
/* int jjl_release(struct inode *inode, struct file *filp) { return 0; } */


static int __init jjl_driver_register(void) {
    // request new (major, minor) device numbers
    printk(KERN_INFO "[jjl] Loading Module 'jjl_driver'...");
    if (alloc_chrdev_region(&dev_nums, 0, 1, "jjl")) {
        printk(KERN_NOTICE "could not allocate device numbers\n");
        return -1;
    }

    /* if (cdev_init(jjl_dev.cdev, &jjl_fops)) { */
    /*     printk(KERN_WARNING "could not allocate new character device"); */
    /*     return -1; */
    /* } */
    /* jjl_dev.ops = &jjl_fops; */
    /* jjl_dev.owner = THIS_MODULE; */
    /* if (cdev_add(&dev->cdev, devno, 1)) { */
    /*     printk(KERN_NOTICE "could not add newly allocated character device"); */
    /*     return -1; */
    /* } */

    printk(KERN_INFO "[jjl] Allocated device numbers (major, minor) = (%d,%d)\n", MAJOR(dev_nums), MINOR(dev_nums));
    printk(KERN_INFO "[jjl] Inserted Module 'jjl_driver'");
    return 0;
}

static void __exit jjl_driver_unregister(void) {
    // free device numbers that alloc_chrdev_region originally allocated for us
    /* cdev_del(&jjl_dev->cdev); */
    unregister_chrdev_region(MAJOR(dev_nums), 1);
    printk(KERN_INFO "Removed Module 'jjl_driver'\n");
}

module_init(jjl_driver_register);
module_exit(jjl_driver_unregister);
