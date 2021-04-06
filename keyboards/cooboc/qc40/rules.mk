#BOOTMAGIC_ENABLE = yes	# Virtual DIP switch configuration
EXTRAKEY_ENABLE = yes	# Audio control and System control
CONSOLE_ENABLE = no	# Console for debug
COMMAND_ENABLE = no    # Commands for debug and configuration
SLEEP_LED_ENABLE = no  # Breathing sleep LED during USB suspend
NKRO_ENABLE = no	    # USB Nkey Rollover
CUSTOM_MATRIX = yes 	# Custom matrix file
MOUSEKEY_ENABLE = yes
COMBO_ENABLE = no
# AUDIO_ENABLE = yes # keyboard make sound

# DEFAULT_FOLDER = cooboc/qc40/bluepill70

# MCU name
MCU = STM32F103

# Enter lower-power sleep mode when on the ChibiOS idle thread
OPT_DEFS += -DCORTEX_ENABLE_WFI_IDLE=TRUE

# project specific files
SRC = matrix.c \
      led.c \
	  buzzer.c \
	  debounce.c

