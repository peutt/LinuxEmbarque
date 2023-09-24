# Pour faire fonctionner le programme sur buildroot :
make xconfig
ajouter curl, sdl2, jsoncpp
make
Ensuite il faut compiler main.cpp avec le g++ de buildroot et les libraries sdl2, jsoncpp et libcurl de buildroot :

/home/aymeric/Downloads/buildroot-2023.02.4/output/host/bin/aarch64-linux-g++ -o programLinuxEmbarque main.cpp -lcurl -lSDL2 -ljsoncpp -I<Buildroot Path>/buildroot-2023.02.4/output/build/sdl2-2.26.5/include -I<Buildroot Path>/buildroot-2023.02.4/output/build/jsoncpp-1.9.5/include -I<Buildroot Path>/buildroot-2023.02.4/output/build/libcurl-8.2.1/include

Ensuite, il faut copier le programme ainsi crée dans le dossier overlay/build de buildroot pour ajouter le programme dans la rasberry emulé.
On refait la commmande make.
On lance qemu (la raseberry emulé):

