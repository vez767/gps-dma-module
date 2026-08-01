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

## Architectural Constraints & Limitations (Known Issues)

<img width="900" height="65" alt="Screenshot 2026-08-01 151804" src="https://github.com/user-attachments/assets/c6d5a1dd-4df4-410a-af3b-59a0c9474b0b" />

**screenshot of PuTTY terminal showing a clean coordinate string**

<img width="489" height="63" alt="Screenshot 2026-08-01 152250" src="https://github.com/user-attachments/assets/211e0c14-f2ff-4244-bfef-1c118a66608f" />

**screenshot of PuTTY terminal showing a fragmented/corrupted NMEA string**

* **Data Overwrite Corruption:** The current architecture utilizes a strict 100-byte circular DMA buffer.
* **The Hardware Failure:** If a burst of incoming NMEA sentences exceeds 100 bytes before the IDLE line interrupt can signal the RTOS task, the DMA controller wraps around and overwrites unread memory.
* **The Result:** This results in fragmented NMEA sentences and occasional data corruption, as demonstrated in the terminal output above.

## Future Roadmap & ISO 14971 Safety Fallbacks (v2.0)
* **Ring Buffer Integration:** The 100-byte constraint will be resolved by implementing a standard FIFO Ring Buffer.
* **HT/TC Interrupts:** The architecture will be upgraded to utilize DMA Half-Transfer (HT) and Transfer-Complete (TC) interrupts, ensuring the CPU safely extracts data from the first half of the buffer while the DMA fills the second half, preventing overwrite corruption.
* **Safe State Handling:** Future implementations will include timeout fallbacks if the physical Rx wire is disconnected, preventing task starvation within the RTOS ecosystem.
