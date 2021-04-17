/*
external code for main
*/
constexpr float pi = 3.14159;
constexpr float diam = 0.675; // wheel diameter
constexpr float circ = diam * pi; // wheel circumference

struct LedMgr {
    bool state = false;
    int pin = -1;
    void (*pSetter)(int, int) = nullptr;
    LedMgr(void hw_setter(int, int), int pinNum) {
        /* hw_setter models after digitalWrite(int pinNum,int value) */
        pSetter = hw_setter;
        pin = pinNum;
    }
    void on() { state = true; pSetter(pin, state); }
    void off() { state = false; pSetter(pin, state); }
    void toggle() { state = !state; pSetter(pin, state); }
    void blink(int n = 1, int millis = 200) {
        /* blink n number of times, default 1 */
        for (int i = 0; i < n; i++) {
            toggle();
            delay(millis / 2);
            toggle();
            delay(millis / 2);
        }

    }
};

struct Spedometer {
    unsigned int tprev = 0;
    unsigned int elapsed = 800000;
    unsigned int tStart = 0;
    float elapsedMin = 8e5; // minimum elapsed time
    int counter = 0; // total rotations since start
    unsigned int stopThresh = 2500; // if elapsed>thresh, effectively stopped
    unsigned int (*pMills)() = nullptr;
    Spedometer(unsigned int mills()) {
        /* mills models after millis() */
        pMills = mills;
    }
    void lap() {
        /* sensor detects a full rotation */
        counter++; // update 'total' variables
        // update 'current' variables
        elapsed = pMills() - tprev;
        tprev = pMills();
        if (elapsed < elapsedMin) elapsedMin = elapsed;
    }
    void startTimer() { tStart = pMills(); }
    float getVelCurr() {
        /* return current speed, m/s */
        //if (counter == 0) return 0; // prevent noise output
        if (pMills() - tprev > stopThresh) return 0; // have detected approximate stop
        else return circ / elapsed * 1000;
    }
    float getDistTotal() { return circ * counter; } // meters
    float getTimeTotal() { return (pMills() / 1000 - tStart / 1000); } // seconds
    float getVelTotal() { return circ * counter * 1000 / (pMills() - tStart); } // m/s, average
    float getVelMax() { return circ / elapsedMin * 1000; } // m/s
    void print() {
        std::printf("vel_curr %5.2f\t vel_max %5.2f \n", getVelCurr(), getVelMax());
        std::printf("vel_tot %5.2f\t dist_tot %5.2f\t time_tot %5.2f \n",
            getVelTotal(), getDistTotal(), getTimeTotal());
    }
};

struct Braking {
    bool stateL = false;
    bool stateR = false;
    int pinL = -1;
    int pinR = -1;
    int (*pReader)(int) = nullptr;
    Braking(int hw_reader(int), int pinLeft, int pinRight) {
        /* hw_reader modeled after digitalRead(int pin) */
        pinL = pinLeft;
        pinR = pinRight;
        pReader = hw_reader;
    }
    void update() {
        stateL = pReader(pinL);
        stateR = pReader(pinR);
    }

    void print() {
        std::printf("Left: %d | Right: %d\n", stateL, stateR);
    }
};

struct CsvSimple {
    /* simple, per-item / per-line csv writer */
    std::ofstream fout;
    CsvSimple() {}; // be able to initialize without opening an actual file
    CsvSimple(std::string filename) { fout.open(filename); }
    ~CsvSimple() { close(); }
    void open(std::string filename) { fout.open(filename); } // if not initialized as open
    void addval(int val) { fout << val << ','; }
    void addval(float val) { fout << val << ','; }
    void addval(double val) { fout << val << ','; }
    void addval(std::string val) { fout << val.c_str() << ','; }
    void endl() { fout << std::endl; }
    void close() { endl(); fout.close(); }
};

