Для того, чтобы расширить ОЗУ нужно:
1. В STVD зайти в Project - Settings - Linker - Input - Ram - Заменить 1ff на 2ff
2. В lkf файле удалить #<BEGIN DEFINED_VARIABLES> и #<END DEFINED_VARIABLES>
3. В lkf файле заменить __endmem=0x1ff на __endmem=0x2ff

При этом в lkf файле строка
+seg .data -b 0x100 -m 0x100 -n .data
должна измениться на
+seg .data -b 0x100 -m 0x200 -n .data

Для вывода данных о памяти в консоль нужно скопировать в корень проекта memory-report.vbs и в stvd зайти в Project - Settings - Post Build и добавить в Commands: cscript //NoLogo memory-report.vbs
Скрипт берёт лимиты памяти из Debug/*.lkf и занятую память из Debug/*.map