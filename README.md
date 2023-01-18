# Processor & Assembler

## Как насчет собственного языка ассемблера и собственного виртуального процессора, на котором он будет исполняться?

Принцип работы схож с обычным ассемблером, только команд меньше. Вот их список (язык, кстати, **не** регистро-зависимый)


## Список команд


- HLT завершает программу
- PUSH добавляет значение в стек
- OUT печатает последний элемент стека в консоль
- ADD складывает два элемента и добавляет значение в стек
- SUB вычитает из последнего предпоследний и добавляет значение в стек
- MUL перемножает два элемента и добавляет значение в стек
- DIV делит последний на предпоследний и добавляет значение в стек
- JMP перемещается на указанную строку
- DUP дублирует последнее значение в стеке
- POP записывает значение из стека в регистр или оперативную память
- JB осуществляет прыжок, если последнее значение в стеке < предпоследнего
- JA осуществляет прыжок, если последнее значение в стеке > предпоследнего
- JE осуществляет прыжок, если последнее значение в стеке == предпоследнему
- JNE осуществляет прыжок, если последнее значение в стеке != предпоследнему
- JAE осуществляет прыжок, если последнее значение в стеке >= предпоследнего
- JBE осуществляет прыжок, если последнее значение в стеке <= предпоследнего
- CALL осуществляет прыжок и запоминает строку на которой находился CALL
- RET возвращется на следующую строку после CALL
- SQRT вычисляет квадратный корень из элемента стека
- IN добавляет в стек число из потока ввода
- SHOW выводит на экран прямоугольник размером SCREEN_WIDTH x SCREEN_HEIGHT
- CLR очищает консоль


## Переменные


Переменные объявляются с помощью знака "=". Например
```
x = 10
```
После этого значение переменной будет просто подставляться в код во время компиляции


## Метки


Метки похожи на переменные, но испольхуют в качетстве значения номер строки, на которой находились. Они объявляются с помощью знака ":". Например
```
label_example:
```
После этого значение метки будет просто подставляться в код во время компиляции


## Особенности push


push 10 - добавляет 10 в стек

push RAX - добавляет в стек значение из регистра RAX

push [10] - добавляет в стек значение из десятой ячейка оперативной памяти

push [10 + RAX] добавляет в стек значение из ячейки оперативной памяти номер (10 + значение из регистра RAX)


## Особенности pop


pop RAX - добавляет значение из стека в регистр RAX

pop [10] - добавляет значение из стека в десятую ячейку оперативной памяти

pop [10 + RAX] добавляет значение из стека в десятую ячейку оперативной памяти номер (10 + значение из регистра RAX)


## Комментарии


Для создания комментария используйте символ "#". Все что будет идти дальше до конца строки будет проигнорированно при компиляции.


## Параметры процессора


Количество регистров, размер оперативной памяти и сторон экрана задается непосредственно в коде процессора.


## Компиляция и использование


Для компиляции исполняемых файлов используйте команду
```sh
make
```


Для компиляции ассемблерного кода в бинарный файл используйте команду
```sh
.\asm.exe -i <asm-source-file> -o <binary-file> 
```


Для исполнения бинарного файла используйте команду
```sh
.\cpu.exe -i <binary-file> 
```

*Все команды оснащены параметром -h или --help*
