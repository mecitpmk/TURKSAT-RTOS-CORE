# TURKSAT-RTOS-CORE
Bu uçuş yazılımı TÜRKSAT Model Uydu yarışması kategorisinde yarışacak takımlar için özel olarak tasarlanmıştır.

## STM32 Proje Ayarları

C/C++ Build-> Settings -> MCU GCC Compiler -> Settings kısmına flag olarak

-fshort-enums

flagini eklemeyi unutmayınız.

Amaç : oluşturulacak enum'lar minimum boyutta olacaktır.


# Satellite Package Frame

![payload_mechn](https://user-images.githubusercontent.com/63451008/156889344-194af94c-6a60-462d-b00c-e0dc2aed6b4e.png)
