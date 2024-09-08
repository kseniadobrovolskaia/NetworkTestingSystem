# Network Testing System 

> [!NOTE]
> Система тестирования по сети основана на обмене информацией в интернет-домене, используя протокол TCP :smile:


> [!IMPORTANT]
> * **Server** - программа, позволяющая создавать и сохранять тесты в формате *.json* и затем запускать их для получения и обработки результатов от *Client* ов.
> * **Client** - позволяет пройти тест, присланный от *Server*, отправить ответы и получить результаты. Используется интерфейс библиотеки графического интерфейса *gtkmm*.


 
![example](Pictures/KnowledgeTest.png) 


## Содержание

[1. Пример использования сервера](#1)

Запуск сервера для создания тестов и обслуживания клиентов

[2. Пример использования клиента](#2)

Подключение к серверу для прохождения тестирования по сети

[3. Сборка ](#3)

Компиляция *Server* и *Client* 



<a name="1"></a>
 ## Пример использования сервера

  Взаимодействие с сервером интуитивно-понятно:

```
$ ./Server
```
  ```
Какое действие сделать? (Напишите число)
  1 - Создать тест и запустить его
  2 - Создать тест и сохранить в файле
  3 - Запустить тест из файлa
  ```

#### Запуск обработки запросов клиентов
 
Для того, чтобы клиенты могли подключаться к серверу нужно задать 
номер свободного порта. Это число от 2000 до 65535:

1. Если порт **занят** операционной системой, то будет получено сообщение:
```
$ Введите номер порта:
$ 9876
$ ERROR on binding
```
  и программа завершится. В этом случае нужно попробовать указать другой номер порта.
2. Успешный **запуск**:
```
$ Введите номер порта:
$ 9876
$ Тест запущен!
```


-----------------------------------------------------------------------------

 
 <a name="2"></a>
 ## Пример использования клиента
 
 Все доступные опции можно узнать:
 ```
$ ./Client   --help
 ```
```
USAGE:     ./Client   [options]

OPTIONS: 
	   -h	 --help
	   -p	 --port
	   -s	 --server-hostname
```


| Опция  | Алиас |Описание         |
|:------------- |:----------|:----------------------------:|
|  **--help**       |  **-h**          | Распечатать справку.|
| **--port**        |  **-p**        | Номер порта сервера, к которому нужно подключиться. |
| **--server-hostname**        |  **-s**        | Название устройства, на котором работает сервер. Его можно узнать в Linux командой *hostname*. Значение по умолчанию **localhost** -- это верно, если сервер запущен на том же устройстве, что и клиент.|



-----------------------------------------------------------------------------

 
 <a name="3"></a>
 ## Сборка 
 
