#include "BaleineEngine.h"

int main() {
    BaleineEngine engine;

    engine.init();

    engine.run();

    engine.cleanup();

    return 0;
}
