#pragma once
#define __MEMORY_H


class Memory {

  public:

    void printInfo() {
      log_n("[HEAP] Size: %d, Free: %d, Min Free: %d, Max Alloc: %d",
            ESP.getHeapSize(), //total heap size
            ESP.getFreeHeap(), //available heap
            ESP.getMinFreeHeap(), //lowest level of free heap since boot
            ESP.getMaxAllocHeap() //largest block of heap that can be allocated at once
      );
      if( psramInit() ) {
        log_n("[PSRAM] Size: %d, Free: %d, Min Free: %d, Max Alloc: %d",
              ESP.getPsramSize(),
              ESP.getFreePsram(),
              ESP.getMinFreePsram(),
              ESP.getMaxAllocPsram()
        );
      }
    }

};


