# Lab 2 Option 3: Myshell

## Team

 - [Usachova Victoria](https://github.com/Vizzzka)

## Prerequisites

 - **C++ compiler** - needs to support **C++17** standard
 - **CMake** 3.15+
 
Dependencies (such as development libraries) can be found in the [dependencies folder](./dependencies) in the form of the text files with package names for different package managers.

## Installing

1. Clone the project.
    ```bash
    git clone git@github.com:chernetskyi/cpp-template.git
    ```
2. Install required packages.

   On Ubuntu:
   ```bash
   [[ -r dependencies/apt.txt ]] && sed 's/#.*//' dependencies/apt.txt | xargs sudo apt-get install -y
   ```
   On MacOS:
   ```bash
   [[ -r dependencies/homebrew.txt ]] && sed 's/#.*//' dependencies/homebrew.txt | xargs brew install
   ```
   Use Conan on Windows.
3. Build.
    ```bash
    cmake -Bbuild
    cmake --build build
    ```

## Usage

```
merrno [-h|--help]  – вивести код завершення останньої програми чи команди
Повертає нуль, якщо ще жодна програма не виконувалася.
Після неї самої merrno повертає нуль, крім випадку, коли було передано невірні опції.



Увага! mpwd [-h|--help] – вивести поточний шлях
Після неї merrno повертає нуль, крім випадку, коли було передано невірні опції.



Critical mcd <path> [-h|--help]  -- перейти до шляху <path>
Повинна підтримувати «.» і «..». («~» -- на ваш розсуд).

Після неї merrno повертає нуль, якщо вдалося перейти в нову директорію, не нуль -- якщо не вдалося або було передано невірні опції.



Увага! mexit [код завершення] [-h|--help]  – вийти із myshell
Якщо не передано код завершення -- вийти із кодом 0.



Увага! mecho [-h|--help] [text|$<var_name>] [text|$<var_name>]  [text|$<var_name>] ...
Без аргументів не робить нічого. Аргументів може бути довільна кількість.

Якщо аргумент починається не з $ -- просто виводить його на консоль.

Якщо з $ -- шукає відповідну змінну та виводить її вміст. Якщо такої змінної не існує -- не виводить нічого. (Можна давати попередження в stderr).

Після неї merrno повертає нуль.

Попередження mexport var_name=VAL
Додати глобальну змінну -- поміщається в блоку змінних середовища для дочірніх процесів.

Виклик

mexport var_name=
створює порожню змінну -- це не помилка.

Після неї merrno повертає нуль, якщо створити змінну вдалося, не нуль -- якщо ні (наприклад, закінчилася пам'ять).

Попередження . <scriptname.msh>
Запуск скрипта в поточному інтерпретаторі.
```

If less than two numbers provided, zeroes are used instead. If more - an error occurs.

Help flags `-h`/`--help` support is available.
