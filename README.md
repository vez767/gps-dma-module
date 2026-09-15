# NEO-6M GPS Driver: CMSIS & FreeRTOS Integration


> <video src="https://github.com/user-attachments/assets/6a7020ae-e540-4358-8fcb-d628fe2938e5" width="600" controls></video>

This module implements a non-blocking, register-level driver for the NEO-6M GPS module using an STM32F4 microcontroller. It marks the transition from raw register manipulation to industry-standard CMSIS architecture, utilizing Direct Memory Access (DMA) and FreeRTOS for efficient telemetry acquisition.

## System Architecture
* **Peripheral:** USART6 configured at 9600 Baud (NMEA standard).
* **Hardware Interface:** PA12 configured for Alternate Function (AF8) Rx.
* **Data Pipeline:** DMA2 Stream 1 (Circular Mode) linked directly to the USART6 Data Register.
* **RTOS Synchronization:** FreeRTOS Task Notifications triggered by the USART6 IDLE line interrupt.

## The CMSIS Implementation
This driver utilizes standard STMicroelectronics CMSIS headers (e.g., `USART_SR_IDLE`, `DMA2_Stream1->CR`) to configure the hardware. NMEA sentences are piped directly from the USART Data Register to a designated memory buffer via DMA without waking the CPU. 

Once a transmission burst finishes, the hardware asserts an IDLE line flag. The `USART6_IRQHandler` intercepts this flag, clears it via a dummy read, and executes a deferred FreeRTOS task notification (`vTaskNotifyGiveFromISR`). The scheduler then unblocks the data processing task.

## Resolved Limitations (v2.0 Update)

> <img width="489" height="63" alt="Screenshot 2026-09-14 134336" src="https://github.com/user-attachments/assets/fee4c553-ac7c-4038-9866-ff785f09bf5b" />




**screenshot of PuTTY terminal showing a clean, unfragmented coordinate string**

* **The Hardware Constraint:** The v1.0 architecture utilized a strict 100-byte DMA buffer with a linear read loop, which caused data overwrite corruption and fragmented NMEA strings when the DMA wrapped around to index 0 before the CPU could extract the payload.


* **The Modulo Ring Buffer Fix:** `RX_BUFFER_SIZE` was expanded to 512 bytes. The extraction loop was decoupled from the hardware via a software Ring Buffer. The hardware head is now dynamically calculated using the DMA's countdown register: `(RX_BUFFER_SIZE - DMA2_Stream1->NDTR) % RX_BUFFER_SIZE`. A software tail safely chases this head across the array boundary, completely eliminating NMEA fragmentation.

## ISO 14971 Safety Fallbacks (Safe State)
**Memory Protection:** A strict bounds limit (`if (parse_index < 99)`) protects the RTOS ecosystem. If a disconnected wire or hardware glitch floods the UART with characters without a \n terminator, the system securely caps the parsing array, preventing a buffer overflow from triggering a HardFault or corrupting adjacent tasks.

## Future Roadmap
* **Unit Testing (Ceedling/Unity):** Implement Test-Driven Development (TDD) by writing offline unit tests to validate C string manipulation (strstr, strtok) before flashing to the MCU.

* **Data Parsing:** Extract precise Latitude and Longitude variables from the raw `$GPGGA` payload to route into the master tracking system.

