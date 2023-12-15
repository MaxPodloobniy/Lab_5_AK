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

static uint counter_param = 1;
module_param(counter_param, uint, 0444);
MODULE_PARM_DESC(counter_param, "Counter (p)");

static int __init my_module_init(void) {
    uint i = 0;
    struct my_node *new_node;

    pr_info("Counter: %d\n", counter_param);

    if (!counter_param)
        pr_warn("The parameter is 0\n");

    if (counter_param >= 5 && counter_param <= 10)
        pr_warn("The parameter is %d, between 5 and 10. Enter a number less than 5\n", counter_param);

    BUG_ON(counter_param > 10);

    for (i = 0; i < counter_param; i++) {
        new_node = kmalloc(sizeof(struct my_node), GFP_KERNEL);

        if (i == 5)
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
