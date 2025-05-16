#include "app.h"

int main(int argc, char** argv) {
    app_t app = {0};
    app_create(&app);
    app_run(&app);
    app_destroy(&app);
    return 0;
}