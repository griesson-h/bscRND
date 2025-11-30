#pragma once

#define VK_PRESENT_MODE_MAILBOX_KHR_IN_USE
//#define VK_PRESENT_MODE_FIFO_RELAXED_KHR_IN_USE
//#define VK_PRESENT_MODE_FIFO_KHR_IN_USE
//#define VK_PRESENT_MODE_IMMEDIATE_KHR_IN_USE



/*
    VK_PRESENT_MODE_IMMEDIATE_KHR_IN_USE (default): Images submitted by your application are transferred to the screen right away, which may result in tearing.

    VK_PRESENT_MODE_FIFO_KHR_IN_USE: The swap chain is a queue where the display takes an image from the front of the queue when the display is refreshed and the program inserts rendered images at the back of the queue. If the queue is full then the program has to wait. This is most similar to vertical sync as found in modern games. The moment that the display is refreshed is known as "vertical blank".

    VK_PRESENT_MODE_FIFO_RELAXED_KHR_IN_USE: This mode only differs from the previous one if the application is late and the queue was empty at the last vertical blank. Instead of waiting for the next vertical blank, the image is transferred right away when it finally arrives. This may result in visible tearing.

    VK_PRESENT_MODE_MAILBOX_KHR_IN_USE: This is another variation of the second mode. Instead of blocking the application when the queue is full, the images that are already queued are simply replaced with the newer ones. This mode can be used to render frames as fast as possible while still avoiding tearing, resulting in fewer latency issues than standard vertical sync. This is commonly known as "triple buffering", although the existence of three buffers alone does not necessarily mean that the framerate is unlocked.

    source: https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
*/

//  If your device doesn't support selected present mode, it will use the default one
//  (e.g. my device doesn't support VK_PRESENT_MODE_FIFO_RELAXED_KHR, so when i chose it, i get VK_PRESENT_MODE_IMMEDIATE_KHR, which is default)
