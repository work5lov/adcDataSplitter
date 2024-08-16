# adcDataSplitter
 Разделитель данных АЦП

Данная программа предназначена для разделения файлов, получаемых при работе АЦП фирмы ООО "Р-Тех" USB-1402.



![MIT License](https://img.shields.io/badge/License-MIT-green.svg)![GitHub top language](https://img.shields.io/github/languages/top/work5lov/adcDataSplitter)


## Описание
Программа выполняет разделение исходного файла с данными АЦП на 2 файла, для каждого из каналов по отдельности. Формат выходных файлов .bin.
## Лицензия

[MIT](https://choosealicense.com/licenses/mit/)


## Планы развития проекта

- Добавить поддержку управления нагрузкой на ПК при обработке файлов (управление выделением ОЗУ и регулирование количества используемых потоков)

- Добавить шкалу прогресса выполнения обработки файла (на данный момент не придумал как это реализовать)

- Добавить возможность сохранения файлов в формате .txt (скорость записи примерно в 12 раз меньше, размер выходного файла в 2,3 раза больше) (+)

- Добавить высвечивание подсказок при наведении на элементы интерфейса (опционально)

- Добавить иконку приложения (+/-)

- Добавить сохранение и загрузку настроек из файла (+)

- Реализовать систему drag and drop (для входных файлов)