# Проект "ComputerClub"

## Компиляция и запуск

1. Откройте терминал и перейдите в директорию build

```sh
cd build
```

2. Запустите cmake для генерации Makefile.

```sh
cmake ..
```

3. Скомпилируйте код:

```sh
make
```

4. После успешной компиляции вы можете запустить программу:

```sh
./ComputerClub InitialTest.txt
```

(вместо InitialTest.txt нужно вставить название своего тестового файла)

В файлах test1.txt, test2.txt, test3.txt представлены комплексные тесты программы. 

В файле test1.txt представлен случай, когда очередь ожидания заполнена, а также посадка
клиента при освобождении стола.

В файле test2.txt клиент (5) приходит, садится за стол, затем начинает ожидать, потом уходит из клуба. Проверка на то, что при уходе из клуба клиент удаляется из очереди ожидания.

В файле test3.txt клиент приходит после окончания рабочего дня. При этом все события выводятся 
по возрастанию времени, то есть сначала выводятся исходящие события конца дня, затем обработка
прихода клиента после окончания рабочего дня.

В файле test4.txt клиенты 1 и 3 ожидают, когда есть свободные столы (ICanWaitNoLonger!),
клиенты 2 и 3 уходят и освобождают столы для очереди ожидания.

В файлах TestClubIsNotOpen.txt, testInvalidNames.txt, testInvalidTimeFormat.txt, testTimeOutOfOrder.txt - некорректные случаи,
соответствующие названиям файлов, проверка вывода ошибок.

InitialTest.txt - тест из примера файла с заданием.

Ожидаемый вывод test1.txt:

```
08:00
07:55 1 client1 
07:55 13 NotOpenYet 
08:05 1 client2 
08:07 1 client1 
08:10 2 client1 1
08:15 2 client2 2
08:20 1 client3 
08:25 2 client3 3
08:30 1 client4 
08:35 2 client4 4
08:40 1 client5 
08:45 2 client5 5
09:00 1 client6 
09:01 1 client7 
09:02 1 client8 
09:03 1 client9 
09:04 1 client10 
09:05 1 client11 
09:05 1 client12 
09:06 3 client6 
09:07 3 client7 
09:08 3 client8 
09:09 3 client9 
09:10 3 client10 
09:11 3 client11 
09:12 3 client12 
09:12 11 client12 
12:00 4 client1 
12:00 12 client6 1
12:05 4 client2 
12:05 12 client7 2
12:10 4 client3 
12:10 12 client8 3
12:15 4 client4 
12:15 12 client9 4
12:20 4 client5 
12:20 12 client10 5
20:00 11 client10 
20:00 11 client11 
20:00 11 client6 
20:00 11 client7 
20:00 11 client8 
20:00 11 client9 
20:00
1 204 11:50
2 204 11:45
3 204 11:35
4 204 11:25
5 204 11:15
```

Ожидаемый вывод test2.txt:
```
09:00
08:55 1 client_1 
08:55 13 NotOpenYet 
09:05 1 client-2 
09:10 2 client_1 1
09:10 13 ClientUnknown 
09:15 2 client-2 2
09:20 1 client3 
09:25 2 client3 3
09:30 1 client_4 
09:35 2 client_4 4
09:40 1 client-5 
09:45 2 client-5 1
10:00 3 client-5 
10:05 4 client-5 
10:10 1 client6 
10:15 2 client6 2
10:15 13 PlaceIsBusy 
10:20 1 client_7 
10:25 2 client_7 3
10:25 13 PlaceIsBusy 
10:30 1 client-8 
10:35 2 client-8 4
10:35 13 PlaceIsBusy 
17:00 4 client6 
17:05 4 client_7 
18:00 11 client-2 
18:00 11 client-8 
18:00 11 client3 
18:00 11 client_4 
18:00
1 20 00:20
2 180 08:45
3 180 08:35
4 180 08:25
```

Ожидаемый вывод test3.txt:

```
10:00
09:55 1 client_1 
09:55 13 NotOpenYet 
10:05 1 client-2 
10:10 2 client_1 1
10:10 13 ClientUnknown 
10:15 2 client-2 2
10:20 1 client3 
10:25 2 client3 3
14:00 4 client_1 
14:00 13 ClientUnknown 
14:05 4 client-2 
14:10 4 client3 
21:55 1 client-4 
22:00 11 client-4 
22:05 1 client5 
22:05 13 NotOpenYet 
22:00
1 0 00:00
2 44 03:50
3 44 03:45
```

Ожидаемый вывод test4.txt:
```
08:00
07:55 1 client1 
07:55 13 NotOpenYet 
08:00 1 client2 
08:01 1 client1 
08:05 2 client1 1
08:10 2 client2 2
08:15 1 client3 
08:20 2 client3 3
08:25 1 client4 
08:30 2 client4 4
08:35 1 client5 
08:40 2 client5 1
08:40 13 PlaceIsBusy 
08:45 3 client1 
08:45 13 ICanWaitNoLonger! 
08:50 4 client2 
09:00 1 client6 
09:05 2 client6 2
09:10 3 client3 
09:10 13 ICanWaitNoLonger! 
09:15 1 client7 
09:20 2 client7 5
09:25 3 client4 
09:30 4 client3 
09:30 12 client4 3
09:35 1 client8 
22:00 11 client1 
22:00 11 client4 
22:00 11 client5 
22:00 11 client6 
22:00 11 client7 
22:00 11 client8 
22:00
1 294 13:55
2 294 13:35
3 315 13:40
4 21 01:00
5 273 12:40
```

Ожидаемый вывод TestClubIsNotOpen.txt:

```
Could not open file testClubsNotOpen.txt
```

Ожидаемый вывод testInvalidNames.txt:

```
Invalid input format: 07:55 1 Alice@1
```

Ожидаемый вывод testInvalidTimeFormat.txt:

```
Invalid input format: 0855 1 tom_1
```

Ожидаемый вывод testTimeOutOfOrder.txt:

```
Invalid input format: 09:50 2 harry_1 1
```