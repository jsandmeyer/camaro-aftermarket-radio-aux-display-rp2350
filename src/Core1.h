#ifndef CORE1_H
#define CORE1_H

#include <Adafruit_SSD1306.h>
#include <vector>
#include <pico/util/queue.h>

#include "Renderer.h"

class Core1 {
    Adafruit_SSD1306* display;
    Renderer* lastRenderer = nullptr;
    std::vector<Renderer *> renderers;
    queue_t* messageQueue;
public:
    explicit Core1(queue_t* messageQueue);
    void processMessage();
    void renderDisplay();
};

#endif //CORE1_H
