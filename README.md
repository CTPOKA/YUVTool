# YUV Overlay Tool  

## Описание  
YUV Overlay Tool — это утилита для наложения изображения в формате BMP (24 бит) на видео в формате YUV420 с использованием SIMD (AVX) и многопоточной обработки.  

## Функционал
- Наложение BMP-изображения на YUV-видео.
- Сохранение результата в указаный файл.
- Настройка прозрачности изображения.
- Настройка положения изображения (смещение по X и Y).

## Сборка и запуск  

### Требования  
- ОС Linux
- Компилятор с поддержкой C++11 (GCC или Clang).  
- Поддержка AVX2.  
- CMake 3.16+ для сборки проекта.  

### Сборка  
```sh  
mkdir build && cd build  
cmake ..  
make  
```  

### Запуск  
```sh  
./yuv_overlay input.yuv output.yuv width height image.bmp [-x X] [-y Y] [-a OPACITY] [--yes]  
```

**Параметры:**  
- `input.yuv` — входной YUV-файл (формат YUV420).  
- `output.yuv` — выходной YUV-файл.  
- `width` и `height` — разрешение видео.  
- `image.bmp` — BMP-изображение, которое будет наложено.  
- `-x X` и `-y Y` — смещение изображения (по умолчанию 0, 0).  
- `-a OPACITY` — прозрачность (по умолчанию 1.0).  
- `--yes` — автоматическое подтверждение всех запросов.  
- `-h, --help` — справка по параметрам.  

## Используемые подходы  
- **SIMD (AVX)** для быстрой обработки пикселей.  
- **Многопоточная обработка** при конвертации BMP → YUV.
