# TURKSAT-RTOS-CORE
Bu uçuş yazılımı TÜRKSAT Model Uydu yarışması kategorisinde yarışacak takımlar için özel olarak tasarlanmıştır.

## STM32 Proje Ayarları

C/C++ Build-> Settings -> MCU GCC Compiler -> Settings kısmına flag olarak

-fshort-enums

flagini eklemeyi unutmayınız.

Amaç : oluşturulacak enum'lar minimum boyutta olacaktır.

