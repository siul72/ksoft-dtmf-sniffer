win32 {
    HOMEDIR += $$(USERPROFILE)
}
else {
    HOMEDIR += $$(HOME)
}


ARDUINO_LIBS = "$${HOMEDIR}\.platformio\packages\framework-arduinoavr\libraries"
ARDUINO_CORE_LIBS = "$${ARDUINO_LIBS}\__cores__\arduino"


INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoavr\cores\arduino"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoavr\variants\standard"
INCLUDEPATH += "$${ARDUINO_CORE_LIBS}\EEPROM\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoavr\libraries\__cores__\arduino\HID\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoavr\libraries\__cores__\arduino\SPI\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoavr\libraries\__cores__\arduino\SoftwareSerial\src"
INCLUDEPATH += "$${ARDUINO_CORE_LIBS}\Wire\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\toolchain-atmelavr\avr\include"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\toolchain-atmelavr\lib\gcc\avr\4.9.2\include"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\toolchain-atmelavr\lib\gcc\avr\4.9.2\include-fixed"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\tool-unity"
INCLUDEPATH += "$${ARDUINO_LIBS}\Adafruit_SSD1306"
INCLUDEPATH += "$${ARDUINO_LIBS}\Adafruit-GFX-Library"


DEFINES += "PLATFORMIO=30600"
DEFINES += "ARDUINO_AVR_UNO"
DEFINES += "F_CPU=16000000L"
DEFINES += "ARDUINO_ARCH_AVR"
DEFINES += "ARDUINO=10805"
DEFINES += "__AVR_ATmega328P__"

OTHER_FILES += platformio.ini

SOURCES += \
    src/main.c

DISTFILES += \
    platformio.ini

