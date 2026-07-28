<div align="center">

# ⚡ MakuTweaker: Linux Edition 🐧

### *Легендарный твикер, адаптированный под суровые реалии Linux*

![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Qt6](https://img.shields.io/badge/Qt6-41CD52?style=for-the-badge&logo=qt&logoColor=white)
![License GPLv3](https://img.shields.io/badge/License-GPLv3-blue.svg?style=for-the-badge)
![NO AI](https://img.shields.io/badge/100%25-NO%20AI%20INSIDE-red?style=for-the-badge)

<p align="center">
  <a href="#-особенности">Особенности</a> •
  <a href="#-поддерживаемые-дистрибутивы">Дистрибутивы</a> •
  <a href="#-быстрый-запуск">Сборка и Запуск</a> •
  <a href="#-пасхалки">Пасхалки</a> •
  <a href="#-лицензия">Лицензия</a>
</p>

---

</div>

## 🌟 О проекте

**MakuTweaker: Linux Edition** — это глубокое переосмысление популярного твикера для Windows, воссозданное на C++ и Qt6 специального для пользователей Linux. Нативный Fluent-интерфейс в тёмных тонах, плавные анимации и максимальная адаптация под системные утилиты POSIX.

> 🚫 **Никакого ИИ-шлака!** Написано вручную на чистых С++ / Qt6 с душой, любовью и легкой ненавистью к неразрывным пробелам.

---

## 🔥 Особенности

- 🎨 **Идеальный WinUI 3 / Fluent Design:** Темная фиолетовая гамма (`#0B0014`), кастомные анимированные тумблеры и затухание страниц.
- 🚀 **Механизм «МАКС КАЧАЛО 100%»:** Умный установщик софта. Сначала ищет пакеты в AUR (`yay`/`paru`), APT или DNF, а если их нет — автоматически подключает **Flatpak / Flathub** и качает оттуда.
- 🛠️ **Глубокая оптимизация:** Отключение телеметрии systemd, оптимизация ядра (`sysctl`), управление службами и очистка кэшей.
- 🎯 **Универсальность:** Автоматическое определение дистрибутива и подмена системных команд на лету.
- 💻 **Реальный детект железа:** Чтение информации о CPU, GPU и RAM напрямую через системные файлы ядра и `lspci`.
- 🔑 **Активация Linux:** Уникальный метод активации системы прямо через сайт *Free Software Foundation*.

---

## 🐧 Поддерживаемые дистрибутивы

Программа автоматически адаптирует команды под твой дистрибутив:

| Дистрибутив | Пакетный менеджер | AUR / Доп. источники |
| :--- | :--- | :--- |
| **Arch Linux / Manjaro** | `pacman` | `yay` / `paru` / `flatpak` |
| **Debian / Ubuntu / Pop!_OS** | `apt` | `flatpak` |
| **Fedora / RHEL / Nobara** | `dnf` | `flatpak` |

---

## 🛠️ Быстрый запуск

### 1. Установка зависимостей (Arch Linux)
```bash
sudo pacman -S --needed base-devel cmake qt6-base
