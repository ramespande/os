int dummy_main(int argc, char **argv);
int main(int argc, char **argv) {
    /* You can add any code here you want to support your SimpleScheduler implementation*/
    raise(SIGSTOP);
    int ret = dummy_main(argc, argv);
    return ret;
}
#define main dummy_main
