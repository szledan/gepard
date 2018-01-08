g++ -std=c++11 -fPIC -shared -o pygepard.so pygepard.cpp -Ibuild/release/include -Lbuild/release/lib -lgepard `pkg-config --cflags --libs python egl glesv2 x11`
