ifneq ($(KERNELRELEASE),)
	# kbuild part of makefile

	# obj-m вказує, що маємо збирати з'єднувач модулів ядра hello.o
	obj-m := hello.o

	# ccflags-y додає прапорці компілятора, у цьому випадку -g (включити відлагодження) і -DDEBUG
	# для визначення макроса DEBUG.
	ccflags-y += -g -DDEBUG
else
	# normal makefile

	# KDIR вказує шлях до директорії зі схожими хедерами і Makefile ядра.
	# Якщо вона не визначена, використовується стандартний шлях /lib/modules/`uname -r`/build.
	KDIR ?= /lib/modules/`uname -r`/build

	default:
		# Викликаємо зовнішній make в директорії ядра, передаючи параметр M з поточною директорією.
		$(MAKE) -C $(KDIR) M=$$PWD

		# Копіюємо скомпільований модуль ядра hello.ko із збереженням символів в hello.ko.unstripped.
		cp hello.ko hello.ko.unstripped

		# Знімаємо зайві символи із hello.ko, залишаючи лише необхідні для відладки.
		$(CROSS_COMPILE)strip -g hello.ko

	clean:
		# Викликаємо зовнішній make в директорії ядра для очищення.
		$(MAKE) -C $(KDIR) M=$$PWD clean

	%.s %.i: %.c
		# Викликаємо зовнішній make в директорії ядра для генерації файлів %.s і %.i
		$(MAKE) -C $(KDIR) M=$$PWD $@
endif
