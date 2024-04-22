# Компилятор эзотерического языка

## Введение
Данный проект представляет из себя компилятор эзотерического языка, синтаксис которого основан на типичном содержании поста группы 'Физтех.Confessions'. Репозиторий включает в себя компиляторные front end, back end и middle end. Компиляция происходит под архитектуру виртуального процессора, представленную в соответствующем [репозитории](https://github.com/Iprime111/Processor). 

## Установка и сборка
На данный момент корректная работа программы гарантируется только для семейства ОС GNU/Linux при работе на архитектуре x86_64. Для сборки используется утилита [cmake](https://cmake.org/).

### Необходимые команды
```bash
$ git clone https://github.com/Iprime111/Language
$ cd Language
$ mkdir build && cd build
$ cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
```

> [!NOTE]
> В данном примере в качестве генератора используется [ninja](https://ninja-build.org/) из-за поддержки многопоточной сборки, однако допускается использование любых систем, поддериваемых cmake

## Использование программы

### Примеры команд
Компиляция написанной программы производится путем последовательного исполнения трех составляющих компилятора. Ниже представлен пример использования компилятора (предполагается, что пользователь находится в папке `build`):

``` 
$ ./bin/frontend <source filename>
$ ./bin/middleend ./SyntaxTree.tmp ./NameTables.tmp
$ ./bin/backend ./SyntaxTree.tmp ./NameTables.tmp > out.asm
```

Данная последовательность команд компилирует программу с заданным именем и помещает получившийся ассемблерный код в файл `out.asm`.

### Вывод ошибок
При переводе программы во внутреннее представление (во время работы фронтенда) происходит генерация html файла, содеражащего список всех ошибок, стилизованный под комментарии к записи в социальной сети. Каждый из них содержит краткую информацию о возникшей ошибке и номер строки, на которой она произошла.

TODO: скрин