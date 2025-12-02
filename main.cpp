#include <string>

#include "include/app/App.h"
#include "include/utils/PathResolver.h"

int main() {
    app::App app{};

    while (!app.should_close()) {
        app.run();
    }

    app.stop();
    return 0;
}
