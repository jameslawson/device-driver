#include <linux/init.h>    //
#include <linux/module.h>  // module_init(), module_exit(), THIS_MODULE, ...
#include <linux/kernel.h>  // printk(), KERN_INFO, KERN_WARNING, ...
#include <linux/fs.h>      // alloc_chrdev_region(), MAJOR(), MINOR(), file_operations, file, inode, ...
#include <linux/cdev.h>    // cdev_init()
#include <asm/uaccess.h>   // copy_to_user
MODULE_LICENSE("GPL");
MODULE_AUTHOR("James Lawson");

dev_t dev_nums;
struct jjl_device {
    struct cdev cdev;      // stores kernel character device info
    struct semaphore sem;  // mutex semaphore
};
struct jjl_device jjl_dev;

int jjl_open(struct inode *inode, struct file *filp) {
    struct jjl_device *dev = container_of(inode->i_cdev, struct jjl_device, cdev);
    filp->private_data = dev;
    printk(KERN_INFO "[jjl] jjl_open done.");
    return 0;
}

ssize_t num_bytes_read;

ssize_t jjl_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    char tmp[] = "hello read\n";
    struct jjl_device *dev = filp->private_data;

    if (num_bytes_read > 0) { return 0; }
    printk(KERN_INFO "[jjl] jjl_read: entering critical section ...");
    if (down_interruptible(&dev->sem)) return -ERESTARTSYS;

    if (copy_to_user(buf, &tmp, strlen(tmp))) {
        printk(KERN_INFO "[jjl] jjl_read: copy_to_user faulted! ...");
        up(&dev->sem);
        return -EFAULT;
    }

    up(&dev->sem);
    num_bytes_read = strlen(tmp);
    printk(KERN_INFO "[jjl] jjl_read: leaving critical section ...");
    printk(KERN_INFO "[jjl] jjl_read: copied data to userspace ...");
    printk(KERN_INFO "[jjl] jjl_read done.");
    return num_bytes_read;
}

int jjl_release(struct inode *inode, struct file *filp) { return 0; }
struct file_operations jjl_fops = {
    .owner = THIS_MODULE,
    .open  = jjl_open,
    .read  = jjl_read,
    .release = jjl_release
};


static int __init jjl_driver_register(void) {
    // request new (major, minor) device numbers
    // request kernel create a new character device kernel data structure
    printk(KERN_INFO "[jjl] Loading Module 'jjl_driver'...");
    if (alloc_chrdev_region(&dev_nums, 0, 1, "jjl")) {
        printk(KERN_NOTICE "[jjl] Could not allocate device numbers\n");
        return -1;
    }

    cdev_init(&(jjl_dev.cdev), &jjl_fops);
    init_MUTEX(&(jjl_dev.sem));
    num_bytes_read = 0;

    jjl_dev.cdev.ops = &jjl_fops;
    jjl_dev.cdev.owner = THIS_MODULE;
    if (cdev_add(&(jjl_dev.cdev), dev_nums, 1)) {
        printk(KERN_NOTICE "[jjl] Could not add newly allocated character device");
        return -1;
    }

    printk(KERN_INFO "[jjl] Allocated device numbers (major, minor) = (%d,%d)\n", MAJOR(dev_nums), MINOR(dev_nums));
    printk(KERN_INFO "[jjl] Inserted Module 'jjl_driver'");
    return 0;
}

static void __exit jjl_driver_unregister(void) {
    // free up the kernel data for our chracter device
    // free up the device numbers that alloc_chrdev_region originally allocated for us
    cdev_del(&(jjl_dev.cdev));
    unregister_chrdev_region(dev_nums, 1);
    printk(KERN_INFO "Removed Module 'jjl_driver'\n");
}

module_init(jjl_driver_register);
module_exit(jjl_driver_unregister);
