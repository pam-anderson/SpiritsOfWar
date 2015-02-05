/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"

/*int main()
{
  printf("Hello!\n");

  // Initialize
  alt_up_char_buffer_dev *char_buffer;
  char_buffer = alt_up_char_buffer_open_dev("/dev/char_drawer");
  alt_up_char_buffer_init(char_buffer);
  // Write some text
  alt_up_char_buffer_string(char_buffer, "EECE 381", 40, 30);

  alt_up_pixel_buffer_dma_dev* pixel_buffer;
  // Use the name of your pixel buffer DMA core
  pixel_buffer = alt_up_pixel_buffer_dma_open_dev("/dev/pixel_buffer_dma");
  // Set the background buffer address – Although we don’t use the background,
  // they only provide a function to change the background buffer address, so
  // we must set that, and then swap it to the foreground.
  alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer, PIXEL_BUFFER_BASE);
  // Swap background and foreground buffers
  alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
  // Wait for the swap to complete
  while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
  // Clear the screen
  alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
  // Draw a white line to the foreground buffer


  return 0;
}*/
