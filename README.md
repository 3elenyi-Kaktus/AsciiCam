# ASCIICam

## Основная задача:

Если человек делает очень много домашки, то он через некоторое время устает. А как мы знаем, самый лучший способ приободриться после очередного контеста по АКОСу - это общение с друзьями. А если после написания команды kks submit и получения ОКа по очередной таске можно увидеть друга прямо не выходя из терминала - жизнь превращается в сплошное удовольствие.

### Реализовать:

* Стабильное P2P-подключение между двумя пользователями.
* Подключение через терминал к веб-камере.
* Realtime конвертация получаемого изображения в ASCII.
* Отображение своего изображения и изображения собеседника в терминале.
* Базовый UI/командный функционал приложения для видеозвонков.

### Крутые идеи для возможного масштабирования проекта:
###### ~~_(возможно, некоторые из них будут реализованы)_~~
###### ~~_(идеи - означает то, что это будет реализовано только идейно)_~~
###### ~~_(а вероятнее всего, не будут реализованы даже идейно)_~~

* Обрезка статичного фона у юзеров (фильтр).
* Дополнительные фильтры (ex: Разноцветные ascii-символы).
* Подключение 3 и более юзеров.
* Элементы realtime-чата (текстовый чат, возможность общаться ascii-стикерами).

### Ожидаемый пользовательский интерфейс:

Юзер с помощью базового интерфейса меню может взаимодействовать с приложением:
* Первый юзер может создать новую комнату в качестве хоста и получить данные для подключения к ней.
* Второй юзер подключается к этой комнате с помощью полученных данных от первого юзера.
* Можно управлять статусом веб-камеры, включать ее и выключать.
* Позже хост может отключить созданную комнату.


### Распределение задач по участникам

* Антон Коротков (tg: @askoro) - Взаимодействие с веб-камерой.
* Ия Волкова (tg: @yhekr) - P2P соединение.
* Фёдор Фаткуллин (tg: @Holpow) - Алгоритм преобразования и интерфейс.