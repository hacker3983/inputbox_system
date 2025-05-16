echo "Building input box system..."
gcc main.c app.c appstring.c inputbox.c -lSDL2 -lSDL2_ttf -o inputbox_system
echo "Running input box system..."
./inputbox_system