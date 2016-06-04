#COVER LETTER

##elf.c

* `run_elf` -- запустить `elf` файл по пути `path` 

* `elf_syscall` -- эта функция -- системный вызов. Если `rax = 0`, то работает как `write`, а на `rax = 1` как `fork`

* `load_elf` -- прочитать `elf` файл, загрузить его в память, настроить стек

* `load_program_header` -- загрузить в память `program header`

* `create_stack` -- создание стека

* `read_buffer` -- прочитать из файла в буфер

* `debug_print_elf_header`, `debug_print_program_header` -- функции чисто для отладки (выводит информацию про elf-файл, чтобы это можно потом было сравнить с `readelf`)

##threads.c

* `setup_userspace` -- настраивает TSS

Также в `place_thread` добавилось сохранение `tss.rsp[0]`.

##entry.S

* `elf_syscall_wrapper` -- обертка для `elf_syscall`

##interrupt.c

В `setup_ints` добавилось наше новое прерывание для `test_elf_irq`

##run.sh

Просто скрипт для компиляции и запуска

##initramfs/simple\_elf

###elf.c

Собственно, наш код для userspace'a
