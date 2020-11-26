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
mpwd [-h|--help] – вивести поточний шлях
mcd <path> [-h|--help]  -- перейти до шляху <path>
mexit [код завершення] [-h|--help]  – вийти із myshell
mecho [-h|--help] [text|$<var_name>] [text|$<var_name>]  [text|$<var_name>]
mexport var_name=VAL  Додати глобальну змінну -- поміщається в блоку змінних середовища для дочірніх процесів.
. <scriptname.msh> Запуск скрипта в поточному інтерпретаторі.
```


Help flags `-h`/`--help` support is available.
