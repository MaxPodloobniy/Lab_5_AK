#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

MODULE_AUTHOR("Max Podloobniy <max.podloobniy@gmail.com>");
MODULE_DESCRIPTION("Lab 3 AK");
MODULE_LICENSE("Dual BSD/GPL");

struct my_node {
    struct list_head node;
    ktime_t start_time;
    ktime_t end_time;
};

static LIST_HEAD(my_list);

static uint myCounter = 1;
module_param(myCounter, uint, 0444);
MODULE_PARM_DESC(myCounter, "Counter (p)");

static int __init my_module_init(void) {
    uint i = 0;
    struct my_node *new_node;

    pr_info("Counter: %d\n", myCounter);

    if (!myCounter)
        pr_warn("Parameter is 0\n");

    if (myCounter > 7)
        pr_warn("Parameter is %d, greater than 7. Enter a number less than or equal to 7\n", myCounter);

    BUG_ON(myCounter > 10);

    for (i = 0; i < myCounter; i++) {
        new_node = kmalloc(sizeof(struct my_node), GFP_KERNEL);

        if (i == 7)
            new_node = NULL;

        if (ZERO_OR_NULL_PTR(new_node))
            goto memory_exception;

        new_node->start_time = ktime_get();
        pr_info("Hello, World!\n");
        new_node->end_time = ktime_get();

        list_add_tail(&(new_node->node), &my_list);
    }

    return 0;

    memory_exception:
    {
        struct my_node *entry, *temp;

        pr_err("Out of memory...\n");

        list_for_each_entry_safe(entry, temp, &my_list, node) {
            list_del(&entry->node);
            kfree(entry);
        }

        BUG();
        return -ENOMEM;
    }
}

static void __exit my_module_exit(void) {
    struct my_node *entry, *temp;

    list_for_each_entry_safe(entry, temp, &my_list, node) {
        pr_info("Time: %lld\n", entry->end_time - entry->start_time);

        list_del(&entry->node);
        kfree(entry);
    }

    BUG_ON(!list_empty(&my_list));
}

module_init(my_module_init);
module_exit(my_module_exit);
