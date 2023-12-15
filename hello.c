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

// Структура my_node представляє собою елемент списку з часовими мітками.
struct my_node {
    struct list_head node;
    ktime_t start_time;
    ktime_t end_time;
};

// Голова списку my_list.
static LIST_HEAD(my_list);

// Змінна myCounter використовується як параметр модуля.
static uint myCounter = 1;
// module_param визначає myCounter як параметр модуля, який може змінюватися з командного рядка.
module_param(myCounter, uint, 0444);
// MODULE_PARM_DESC вказує опис параметра для виведення при виведенні інформації про модуль.
MODULE_PARM_DESC(myCounter, "Counter (p)");

static int __init my_module_init(void) {
    uint i = 0;
    struct my_node *new_node;

    // Виведення інформації про myCounter.
    pr_info("Counter: %d\n", myCounter);

    // Перевірка, чи myCounter не дорівнює 0.
    if (!myCounter)
        pr_warn("Parameter is 0\n");

    // Перевірка, чи myCounter більше 7.
    if (myCounter > 7)
        pr_warn("Parameter is %d, greater than 7. Enter a number less than or equal to 7\n", myCounter);

    // Перевірка, чи myCounter не більше 10 за допомогою макросу BUG_ON.
    BUG_ON(myCounter > 10);

    // Цикл для створення myCounter елементів списку.
    for (i = 0; i < myCounter; i++) {
        // Виділення пам'яті для нового елемента списку.
        new_node = kmalloc(sizeof(struct my_node), GFP_KERNEL);

        // Якщо і == 7, робимо new_node рівним NULL.
        if (i == 7)
            new_node = NULL;

        // Перевірка на помилку виділення пам'яті.
        if (ZERO_OR_NULL_PTR(new_node))
            goto memory_exception;

        // Запис часових міток для виведення потім.
        new_node->start_time = ktime_get();
        pr_info("Hello, World!\n");
        new_node->end_time = ktime_get();

        // Додавання нового елемента в кінець списку.
        list_add_tail(&(new_node->node), &my_list);
    }

    return 0;

    // Обробник помилки для випадку, коли відбувається помилка виділення пам'яті.
    memory_exception:
    {
        struct my_node *entry, *temp;

        // Виведення повідомлення про вичерпання пам'яті.
        pr_err("Out of memory...\n");

        // Проходження по списку та звільнення ресурсів.
        list_for_each_entry_safe(entry, temp, &my_list, node) {
            list_del(&entry->node);
            kfree(entry);
        }

        // Виклик макросу BUG, що призведе до виведення повідомлення про помилку та відмови в завантаженні модуля.
        BUG();
        return -ENOMEM;
    }
}

static void __exit my_module_exit(void) {
    struct my_node *entry, *temp;

    // Проходження по списку та виведення часу вивантаження кожного елемента.
    list_for_each_entry_safe(entry, temp, &my_list, node) {
        pr_info("Time: %lld\n", entry->end_time - entry->start_time);

        // Видалення елемента зі списку та звільнення ресурсів.
        list_del(&entry->node);
        kfree(entry);
    }

    // Перевірка, чи список порожній за допомогою макросу BUG_ON.
    BUG_ON(!list_empty(&my_list));
}

// Вказівка на функцію ініціалізації модуля.
module_init(my_module_init);
// Вказівка на функцію вивантаження модуля.
module_exit(my_module_exit);
