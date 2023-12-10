#include "myos.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MyOS w;
    w.show();
    return a.exec();
}
