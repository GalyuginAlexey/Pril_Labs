# Лабораторная работа №3 #

Выполнил: Галюгин Алексей, Группа - М8О-103М-20

## Сборка проекта ##

```bash
cmake configure .
cmake .
cmake --build ./
```

## Файл для создания шардов ##

```bash
./data_shard_splitter
```

## Запуск Docker-окружения ##

```bash
sudo docker-compose up -d
```

## Остановка Docker-окружения ##

```bash
sudo docker-compose stop
```

Заполнение базы данных с помощью скрипта:

```bash
mysql -u test -p pzjqUkMnc7vfNHET -h 127.0.0.1 -P 6033 --comments
source sql_commands/shard_init.sql;
source sql_commands/shard_fill.sql;
```

Файл генерирует скрипт shard_fill.sql .

## Запуск сервера ##

Для запуска сервера необходимо выполнить команду:

```bash
sudo sh ./start.sh
```

## Тестирование с помощью gtest ##

Запуск тестов   осуществляется с  помощью команды:

```bash
./gtests
```

## Тестирование с помощью wrk ##

Нагрузочное тестирование осуществлялось с помощью команды wrk:

```bash
wrk -t 6 -c 50 -d 30s http://localhost:8080/person?login=731-07-5834 
```

Нагрузочное тестирование производилось для 1, 2, 6, а также 10 потоков при 50 подключениях в течение 30 секунд. Полученные данные (Requests/sec - количество запросов в секунду, Latency(ms) - задержка в миллисекундах):

Threads | Requests/sec | Latency(ms)
---     | ---          | ---
1       | 2605.07      | 7.12
2       | 2700.37      | 6.02
6       | 2611.14      | 6.53
10      | 2495.55      | 6.71
