#include "./server/server.h"

int main(){
    Server server(6379);
    server.start();
    server.run();
    return 0;
}